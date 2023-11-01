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


namespace emulation {
namespace gb {

extern const char* reg_names[];
extern const char* reg16_names[];


class Disassembler : public Cpu {
public:
  Disassembler();
  ~Disassembler();
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset() {}
  void Step() {}
  void set_pc(uint16_t pc) {
    reg.PC = pc;
  }
  void FormatInstruction();

  const char* current_str() { return current_str_; }
private:

  char current_str_[100], prefix_str_[50];
  typedef void (Disassembler::* Instruction)();
  Instruction instructions[0x100];

  const char* prefix() {
    sprintf_s(prefix_str_, "%s:%s%04X\t", "", "", opcode_pc);
    return prefix_str_;
  }

  


  template<uint8_t dest, uint8_t src, int mode>
  inline void arithmeticMode(char* a, char* b) {
    if (mode == 0) {
      sprintf_s(a, 25, "%s", reg_names[dest]);
      sprintf_s(b, 25, "%s", reg_names[src]);
    } else if (mode == 1) {
      sprintf_s(a, 25, "%s", reg_names[dest]);
      sprintf_s(b, 25, "(%s)", reg16_names[src]);
    } else if (mode == 2) {
      sprintf_s(a, 25, "%s", reg_names[dest]);
      sprintf_s(b, 25, "(%04X)", reg.PC++);
    }
  }

  template<uint8_t dest, uint8_t src>
  inline void arithmeticMode0(uint8_t& a, uint8_t& b) {
    a = reg.raw8[dest];
    b = reg.raw8[src];
  }

  template<uint8_t dest, uint8_t src>
  inline void arithmeticMode1(uint8_t& a, uint8_t& b) {
    a = reg.raw8[dest];
    b = mem_->Read8(reg.raw16[src]);
  }

  template<uint8_t dest, uint8_t src>
  inline void arithmeticMode2(uint8_t& a, uint8_t& b) {
    a = reg.raw8[dest];
    b = mem_->Read8(reg.PC++);
  }


  void NOP();
  void ILLEGAL();
  void RST();
  template<uint8_t dest, uint8_t src>
  void LDrr();
  template<uint8_t dest, uint8_t src>
  void LD$rr();
  template<uint8_t dest, uint8_t src>
  void LDr$r();
  template<uint8_t dest, uint8_t src>
  void LD$FF00rr();
  template<uint8_t dest, uint8_t src>
  void LDr$FF00r();
  template<uint8_t dest>
  void LDrd16();
  template<uint8_t dest, uint8_t src>
  void LDI$regreg();
  template<uint8_t dest, uint8_t src>
  void LDD$regreg();
  template<uint8_t dest, uint8_t src>
  void LDIreg$reg();
  template<uint8_t dest, uint8_t src>
  void LDDreg$reg();

  template<uint8_t dest, uint8_t src, int mode>
  void LD();

  void LDSPHL();
  void LDHLSPr8();
  void LDa16SP();
  template<uint8_t dest, uint8_t src, int mode>
  void ADD();
  template<uint8_t dest, uint8_t src>
  void ADD_16bit();
  void ADD_SPr8();
  template<uint8_t dest, uint8_t src, int mode>
  void ADC();
  template<uint8_t dest, uint8_t src, int mode>
  void SUB();
  template<uint8_t dest, uint8_t src, int mode>
  void SBC();
  template<uint8_t dest, uint8_t src, int mode>
  void AND();
  template<uint8_t dest, uint8_t src, int mode>
  void XOR();
  template<uint8_t dest, uint8_t src, int mode>
  void OR();
  void SCF();
  void CCF();
  void HALT();
  void STOP();
  void CPL();
  void PREFIX_CB();
  void JR();
  template<CpuFlags condbit, bool inv>
  void JR_cc();

  template<uint8_t dest, int mode>
  void INC_8bit();
  template<uint8_t dest>
  void INC_16bit();

  template<uint8_t dest, int mode>
  void DEC_8bit();
  template<uint8_t dest>
  void DEC_16bit();

  void JP();
  template<CpuFlags condbit, bool inv>
  void JP_cc();
  void JP_HL();

  void CALL();
  template<CpuFlags condbit, bool inv>
  void CALL_cc();

  void RET();
  template<CpuFlags condbit, bool inv>
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