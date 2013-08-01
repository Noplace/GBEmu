#pragma once

namespace emulation {
namespace gb {

//#pragma pack(1)
union CpuFlagRegister {
	struct {
		uint8_t _unused:4;
		uint8_t  carry:1;
		uint8_t  bcd_halfcarry:1;
		uint8_t  bcd_addsub:1;
		uint8_t  zero:1;
	};
	uint8_t raw;
};

enum CpuRegisterNames8 { RegF,RegA,RegC,RegB,RegE,RegD,RegL,RegH,RegSPl,RegSPh,RegPCl,RegPCh };
enum CpuRegisterNames16 { RegAF,RegBC,RegDE,RegHL,RegSP,RegPC };
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
  void Step();
 private: 
  typedef void (Cpu::*Instruction)();
  Instruction instructions[0xFF];
  CpuRegisters reg;
  Memory* mem_;
  void NOP(); 
	template<uint8_t dest,uint8_t src,int dmode,uint8_t cycles>
	void LD();
	template<uint8_t dest,uint8_t src,int mode>
	void ADD();
	template<uint8_t dest,uint8_t src,int mode>
	void AND();
	template<uint8_t dest,uint8_t src,int mode>
	void XOR();
	template<uint8_t dest,uint8_t src,int mode>
	void OR();

	void HALT();

	void PREFIX_CB();

};

}
}