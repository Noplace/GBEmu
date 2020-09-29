/*****************************************************************************************************************
* Copyright (c) 2013 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/
#pragma once
#include <RecCore/Lib/reccore.h>

namespace emulation {
namespace gb {

template<typename A, typename B>
union anycast {
    A a;
    B b;
};

//#pragma pack(1)
union CpuFlagRegister {
  struct {
    uint8_t _unused:4;
    uint8_t  C:1;
    uint8_t  H:1;
    uint8_t  N:1;
    uint8_t  Z:1;
  };
  uint8_t raw;
};

enum CpuRegisterNames8 { RegF,RegA,RegC,RegB,RegE,RegD,RegL,RegH,RegSPl,RegSPh,RegPCl,RegPCh };
enum CpuRegisterNames16 { RegAF,RegBC,RegDE,RegHL,RegSP,RegPC };
enum CpuFlags { CpuFlagsUnused0 = 0, CpuFlagsC = 4,CpuFlagsH=5,CpuFlagsN=6,CpuFlagsZ=7 };
enum CpuMode { CpuModeNormal,CpuModeHalt,CpuModeStop, CpuModeUndefined };
//#pragma pack(1)
struct CpuRegisters {
  union {
    struct {
      union {
        uint16_t AF;
        struct {
          CpuFlagRegister F;
          uint8_t A;
        };
      };
      union {
        uint16_t BC;
        struct {
          uint8_t C;
          uint8_t B;
        };
      };
      union {
        uint16_t DE;
        struct {
          uint8_t E;
          uint8_t D;
        };
      };
      union {
        uint16_t HL;
        struct {
          uint8_t L;
          uint8_t H;
        };
      };
      uint16_t SP;
      uint16_t PC;
    };
    uint8_t raw8[12];
    uint16_t raw16[6];
  };
};

class Cpu : public Component {
 public:

  uint16_t opcode_pc;
 // uint8_t sprite_bug;
  uint32_t ticks_to_switchspeed;
  virtual void Initialize(Emu* emu) = 0;
  virtual void Deinitialize() = 0;
  virtual void Reset() = 0;
  virtual void Step() = 0;
  void Wake() {
    cpumode_ = CpuMode::CpuModeNormal;
  }
  void SkipBootROM() {
    reg.AF = 0x1180;// 0x01B0;
    reg.BC = 0x0000;// 0x0013;
    reg.DE = 0xff56;// 0x00D8;
    reg.HL = 0x000d;//0x014D;
    reg.SP =  0xFFFE;
    reg.PC = 0x0100;

  }
  CpuMode cpumode() { return cpumode_; }
  const CpuRegisters& regs() { return reg; }
 protected:
  double dt;
  CpuRegisters reg;
  Memory* mem_;
  bool ime;
  uint8_t opcode;
  CpuMode cpumode_;
};

class CpuInterpreter : public Cpu {
 public:
  CpuInterpreter();
  ~CpuInterpreter();
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  void Step();


 private: 
  typedef void (CpuInterpreter::*Instruction)();
  Instruction instructions[0x100];
  void HandleInterrupts();
  void ExecuteInstruction();
  void simulateSpriteBug(uint16_t value);
  inline void updateCpuFlagC(uint8_t a,uint8_t b,int mode) {
    if (mode == 0) {
      uint16_t r2 = (a) + (b);
      reg.F.C = r2>0xFF?1:0;
    } else {
      reg.F.C = a < b ? 1 : 0;
    }
  }
  
  inline void updateCpuFlagH(uint8_t a,uint8_t b,int mode) {
    /*if((dest^b^a)&0x10)
      reg.F.H=1;
    else
      reg.F.H=0;*/
    if (mode == 0) {
      uint8_t r1 = (a&0xF) + (b&0xF);
      reg.F.H = r1>0xF?1:0;
    } else {
      reg.F.H = (a&0xF) < (b&0xF) ? 1 : 0;
    }
  }

  inline void updateCpuFlagZ(uint8_t r) {
    reg.F.Z = r == 0?1:0;
  }

  inline void push(uint8_t data) {
    mem_->ClockedWrite8(--reg.SP,data);
  }

  inline uint8_t pop() {
    return mem_->ClockedRead8(reg.SP++);
  }

  template<uint8_t dest,uint8_t src,int mode>
  inline void arithmeticMode(uint8_t& a, uint8_t& b) {
    if (mode == 0) {
      a = reg.raw8[dest];
      b = reg.raw8[src];
    } else if (mode == 1) {
      a = reg.raw8[dest];
      b = mem_->ClockedRead8(reg.raw16[src]);
    } else if (mode == 2) {
      a = reg.raw8[dest];
      b = mem_->ClockedRead8(reg.PC++);
    }
  }

  template<uint8_t dest,uint8_t src>
  inline void arithmeticMode0(uint8_t& a, uint8_t& b) {
      a = reg.raw8[dest];
      b = reg.raw8[src];
  }

  template<uint8_t dest,uint8_t src>
  inline void arithmeticMode1(uint8_t& a, uint8_t& b) {
      a = reg.raw8[dest];
      b = mem_->ClockedRead8(reg.raw16[src]);
  }

  template<uint8_t dest,uint8_t src>
  inline void arithmeticMode2(uint8_t& a, uint8_t& b) {
      a = reg.raw8[dest];
      b = mem_->ClockedRead8(reg.PC++);
  }

  void pushPC() {
    push((reg.PC&0xFF00)>>8);
    push(reg.PC&0xFF);
  }
 

  void pushSP() {
    push((reg.SP&0xFF00)>>8);
    push(reg.SP&0xFF);
  }

  void NOP(); 
  void ILLEGAL();
  void RST();
  template<uint8_t dest,uint8_t src>
  void LDrr();
  template<uint8_t dest,uint8_t src>
  void LD$rr();
  template<uint8_t dest,uint8_t src>
  void LDr$r();
  template<uint8_t dest,uint8_t src>
  void LD$FF00rr();
  template<uint8_t dest,uint8_t src>
  void LDr$FF00r();
  template<uint8_t dest>
  void LDrd16();
  template<uint8_t dest,uint8_t src>
  void LDI$regreg();
  template<uint8_t dest,uint8_t src>
  void LDD$regreg();
  template<uint8_t dest,uint8_t src>
  void LDIreg$reg();
  template<uint8_t dest,uint8_t src>
  void LDDreg$reg();

  template<uint8_t dest,uint8_t src,int mode>
  void LD();

  void LDSPHL();
  void LDHLSPr8();
  void LDa16SP();
  template<uint8_t dest,uint8_t src,int mode>
  void ADD();
  template<uint8_t dest,uint8_t src>
  void ADD_16bit();
  void ADD_SPr8();
  template<uint8_t dest,uint8_t src,int mode>
  void ADC();
  template<uint8_t dest,uint8_t src,int mode>
  void SUB();
  template<uint8_t dest,uint8_t src,int mode>
  void SBC();
  template<uint8_t dest,uint8_t src,int mode>
  void AND();
  template<uint8_t dest,uint8_t src,int mode>
  void XOR();
  template<uint8_t dest,uint8_t src,int mode>
  void OR();
  void SCF();
  void CCF();
  void HALT();
  void STOP();
  void CPL();
  void PREFIX_CB();
  void JR();
  template<CpuFlags condbit,bool inv>
  void JR_cc();

  template<uint8_t dest,int mode>
  void INC_8bit();
  template<uint8_t dest>
  void INC_16bit();

  template<uint8_t dest,int mode>
  void DEC_8bit();
  template<uint8_t dest>
  void DEC_16bit();

  void JP();
  template<CpuFlags condbit,bool inv>
  void JP_cc();
  void JP_HL();

  void CALL();
  template<CpuFlags condbit,bool inv>
  void CALL_cc();

  void RET();
  template<CpuFlags condbit,bool inv>
  void RET_cc();

  template<uint8_t src>
  void PUSH();
  template<uint8_t dest>
  void POP();

  void RLCA();
  void RRCA();
  void RLA();
  void RRA();

  void CP(uint8_t a, uint8_t b);
  template<CpuRegisterNames8 r>
  void CP_reg();
  void CP_d8();
  void CP_HL();

  void DI();
  void EI();

  void RETI();

  void DAA();
};


class CpuRecompiler : public Cpu {
 public:
  CpuRecompiler();
  ~CpuRecompiler();
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  void Step();
 private: 
  typedef void (CpuRecompiler::*Instruction)();
  Instruction instructions[0x100];

  reccore::CodeBlock* block;
  reccore::Emitter e;

  void simulateSpriteBug(uint16_t value);
  inline void updateCpuFlagC(uint8_t a,uint8_t b,int mode) {
    if (mode == 0) {
      uint16_t r2 = (a) + (b);
      reg.F.C = r2>0xFF?1:0;
    } else {
      reg.F.C = a < b ? 1 : 0;
    }
  }
  
  inline void updateCpuFlagH(uint8_t a,uint8_t b,int mode) {
    /*if((dest^b^a)&0x10)
      reg.F.H=1;
    else
      reg.F.H=0;*/
    if (mode == 0) {
      uint8_t r1 = (a&0xF) + (b&0xF);
      reg.F.H = r1>0xF?1:0;
    } else {
      reg.F.H = (a&0xF) < (b&0xF) ? 1 : 0;
    }
  }

  inline void updateCpuFlagZ(uint8_t r) {
    reg.F.Z = r == 0?1:0;
  }

  inline void updateCpuFlagZREC(uint8_t* r)
  {
      /*using namespace reccore::intel;
      IA32 ia32(&e);
      ia32.MOV(DL,EA("[disp32]",int32_t(&reg.F.raw)));
      ia32.AND(DL,127);
      ia32.MOV(AL,EA("[disp32]",int32_t(r)));
      ia32.CMP(AL,0);
      ia32.SETcc(kCCE,EA(AL));
      ia32.SHL(AL,7);    
      ia32.OR(AL,EA(DL));
      ia32.MOV(EA("[disp32]",int32_t(&reg.F.raw)),AL);
      */
  }


  inline void push(uint8_t data) {
    mem_->ClockedWrite8(--reg.SP,data);
  }

  inline uint8_t pop() {
    return mem_->ClockedRead8(reg.SP++);
  }

  template<uint8_t dest,uint8_t src,int mode>
  inline void arithmeticMode(uint8_t& a, uint8_t& b) {
    if (mode == 0) {
      a = reg.raw8[dest];
      b = reg.raw8[src];
    } else if (mode == 1) {
      a = reg.raw8[dest];
      b = mem_->ClockedRead8(reg.raw16[src]);
    } else if (mode == 2) {
      a = reg.raw8[dest];
      b = mem_->ClockedRead8(reg.PC++);
    }
  }

  template<uint8_t dest,uint8_t src,int mode>
  inline void arithmeticModeREC() { //AL,DL
   /* using namespace reccore::intel;
    IA32 ia32(&e);
    anycast<uint8_t (Memory::*)(uint16_t),void*> cast1;
    cast1.a =  &Memory::ClockedRead8;
    if (mode == 0) {
      ia32.MOV(AL,EA("[disp32]",int32_t(&reg.raw8[dest])));
      ia32.MOV(DL,EA("[disp32]",int32_t(&reg.raw8[src])));
    } else if (mode == 1) {
      //a = reg.raw8[dest];
      //b = mem_->ClockedRead8(reg.raw16[src]);
      ia32.LEA(ECX,mem_);
      ia32.MOV(AX,EA("[disp32]",int32_t(&reg.raw16[src])));
      ia32.PUSH(AX);
      ia32.CALL(cast1.b);
      ia32.MOV(DL,AL);

      ia32.MOV(AL,EA("[disp32]",int32_t(&reg.raw8[dest])));//a
    } else if (mode == 2) {

      //a = reg.raw8[dest];
      //b = mem_->ClockedRead8(reg.PC++);

      ia32.LEA(ECX,mem_);
      ia32.MOV(AX,EA("[disp32]",int32_t(&reg.PC)));
      ia32.PUSH(AX);
      ia32.INC(AX);
      ia32.MOV(EA("[disp32]",int32_t(&reg.PC)),AX);
      ia32.CALL(cast1.b);
      ia32.MOV(DL,AL);

      ia32.MOV(AL,EA("[disp32]",int32_t(&reg.raw8[dest])));//a

    }*/
  }


  void pushPC() {
    push((reg.PC&0xFF00)>>8);
    push(reg.PC&0xFF);
  }
 

  void pushSP() {
    push((reg.SP&0xFF00)>>8);
    push(reg.SP&0xFF);
  }

  void NOP(); 
  void ILLEGAL();
  void RST();
  template<uint8_t dest,uint8_t src>
  void LDrr();
  template<uint8_t dest,uint8_t src>
  void LD$rr();
  template<uint8_t dest,uint8_t src>
  void LDr$r();
  template<uint8_t dest,uint8_t src>
  void LD$FF00rr();
  template<uint8_t dest,uint8_t src>
  void LDr$FF00r();
  template<uint8_t dest>
  void LDrd16();
  template<uint8_t dest,uint8_t src>
  void LDI$regreg();
  template<uint8_t dest,uint8_t src>
  void LDD$regreg();
  template<uint8_t dest,uint8_t src>
  void LDIreg$reg();
  template<uint8_t dest,uint8_t src>
  void LDDreg$reg();

  template<uint8_t dest,uint8_t src,int mode>
  void LD();

  void LDSPHL();
  void LDHLSPr8();
  void LDa16SP();
  template<uint8_t dest,uint8_t src,int mode>
  void ADD();
  template<uint8_t dest,uint8_t src>
  void ADD_16bit();
  void ADD_SPr8();
  template<uint8_t dest,uint8_t src,int mode>
  void ADC();
  template<uint8_t dest,uint8_t src,int mode>
  void SUB();
  template<uint8_t dest,uint8_t src,int mode>
  void SBC();
  template<uint8_t dest,uint8_t src,int mode>
  void AND();
  template<uint8_t dest,uint8_t src,int mode>
  void XOR();
  template<uint8_t dest,uint8_t src,int mode>
  void OR();
  void SCF();
  void CCF();
  void HALT();
  void STOP();
  void CPL();
  void PREFIX_CB();
  void JR();
  template<CpuFlags condbit,bool inv>
  void JR_cc();

  template<uint8_t dest,int mode>
  void INC_8bit();
  template<uint8_t dest>
  void INC_16bit();

  template<uint8_t dest,int mode>
  void DEC_8bit();
  template<uint8_t dest>
  void DEC_16bit();

  void JP();
  template<CpuFlags condbit,bool inv>
  void JP_cc();
  void JP_HL();

  void CALL();
  template<CpuFlags condbit,bool inv>
  void CALL_cc();

  void RET();
  template<CpuFlags condbit,bool inv>
  void RET_cc();

  template<uint8_t src>
  void PUSH();
  template<uint8_t dest>
  void POP();

  void RLCA();
  void RRCA();
  void RLA();
  void RRA();

  void CP(uint8_t a, uint8_t b);
  template<CpuRegisterNames8 r>
  void CP_reg();
  void CP_d8();
  void CP_HL();

  void DI();
  void EI();

  void RETI();

  void DAA();
};

}
}