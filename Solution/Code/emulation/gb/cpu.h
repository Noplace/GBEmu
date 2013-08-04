#pragma once

namespace emulation {
namespace gb {

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
	uint64_t cycles;
  Cpu();
  ~Cpu();
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  void Tick();
  void Step(double dt);
 private: 
  typedef void (Cpu::*Instruction)();
  double dt;
  Instruction instructions[0xFF];
  CpuRegisters reg;
  Memory* mem_;
  bool ime;
  uint8_t opcode;
  void updateCpuFlagC(uint8_t a,uint8_t b,int mode) {
    if (mode == 0) {
      uint16_t r2 = (a) + (b);
      reg.F.C = r2>0xFF?1:0;
    } else {
      reg.F.C = a < b ? 1 : 0;
    }
  }
  
  void updateCpuFlagH(uint8_t a,uint8_t b,int mode) {
    if (mode == 0) {
      uint8_t r1 = (a&0xF) + (b&0xF);
      reg.F.H = r1>0xF?1:0;
    } else {
      reg.F.H = (a&0xF) < (b&0xF) ? 1 : 0;
    }
  }

  void updateCpuFlagZ(uint8_t r) {
    reg.F.Z = r == 0?1:0;
  }

  void push(uint8_t data) {
    mem_->Write8(--reg.SP,data);
  }

  uint8_t pop() {
    return mem_->Read8(reg.SP++);
  }

  template<uint8_t dest,uint8_t src,int mode>
  void arithmeticMode(uint8_t& a, uint8_t& b) {
	  if (mode == 0) {
      a = reg.raw8[dest];
      b = reg.raw8[src];
    } else if (mode == 1) {
      a = reg.raw8[dest];
      b = mem_->Read8(reg.raw16[src]);
		  cycles += 4;
    } else if (mode == 2) {
      a = reg.raw8[dest];
      b = mem_->Read8(reg.PC++);
		  cycles += 4;
    }
  }

  void pushPC() {
    push((reg.PC&0xFF00)>>8);
    push(reg.PC&0xFF);
  }
 
  void NOP(); 
  void ILLEGAL();
  void RST();
	template<uint8_t dest,uint8_t src,int mode>
	void LD();
	template<uint8_t dest,uint8_t src,int mode>
	void ADD();
  template<uint8_t dest,uint8_t src>
  void ADD_16bit();
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

	void HALT();
  void CPL();
	void PREFIX_CB();
  template<CpuFlags condbit,bool inv>
  void JR();

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
};

}
}