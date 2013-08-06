#include "gb.h"
/*


*/

#define StopAt(x) { if (x == reg.PC) DebugBreak(); } 

namespace emulation {
namespace gb {



Cpu::Cpu() {
	instructions[0x00] = &Cpu::NOP;
	instructions[0x01] = &Cpu::LD<RegBC,0,ModeRegD16>;
	instructions[0x02] = &Cpu::LD<RegBC,RegA,Mode$RegReg>;
	instructions[0x03] = &Cpu::INC_16bit<RegBC>;
	instructions[0x04] = &Cpu::INC_8bit<RegB,0>;
	instructions[0x05] = &Cpu::DEC_8bit<RegB,0>;
	instructions[0x06] = &Cpu::LD<RegB,0,10>;
	instructions[0x07] = &Cpu::RLCA;
	instructions[0x08] = nullptr;
	instructions[0x09] = &Cpu::ADD_16bit<RegHL,RegBC>;
	instructions[0x0A] = &Cpu::LD<RegA,RegBC,ModeReg$Reg>;
	instructions[0x0B] = &Cpu::DEC_16bit<RegBC>;
	instructions[0x0C] = &Cpu::INC_8bit<RegC,0>;
	instructions[0x0D] = &Cpu::DEC_8bit<RegC,0>;
	instructions[0x0E] = &Cpu::LD<RegC,0,10>;
	instructions[0x0F] = &Cpu::RRCA;

	instructions[0x10] = nullptr;
	instructions[0x11] = &Cpu::LD<RegDE,0,ModeRegD16>;
	instructions[0x12] = &Cpu::LD<RegDE,RegA,Mode$RegReg>;
	instructions[0x13] = &Cpu::INC_16bit<RegDE>;
	instructions[0x14] = &Cpu::INC_8bit<RegD,0>;
	instructions[0x15] = &Cpu::DEC_8bit<RegD,0>;
	instructions[0x16] = &Cpu::LD<RegD,0,10>;
	instructions[0x17] = &Cpu::RLA;
	instructions[0x18] = &Cpu::JR;
	instructions[0x19] = &Cpu::ADD_16bit<RegHL,RegDE>;
	instructions[0x1A] = &Cpu::LD<RegA,RegDE,ModeReg$Reg>;
	instructions[0x1B] = &Cpu::DEC_16bit<RegDE>;
	instructions[0x1C] = &Cpu::INC_8bit<RegE,0>;
	instructions[0x1D] = &Cpu::DEC_8bit<RegE,0>;
	instructions[0x1E] = &Cpu::LD<RegE,0,10>;
	instructions[0x1F] = &Cpu::RRA;
	
	instructions[0x20] = &Cpu::JR_cc<CpuFlagsZ,1>;
	instructions[0x21] = &Cpu::LD<RegHL,0,ModeRegD16>;
	instructions[0x22] = &Cpu::LD<RegHL,RegA,Mode$RegRegLDI>;
	instructions[0x23] = &Cpu::INC_16bit<RegHL>;
	instructions[0x24] = &Cpu::INC_8bit<RegH,0>;
	instructions[0x25] = &Cpu::DEC_8bit<RegH,0>;
	instructions[0x26] = &Cpu::LD<RegH,0,10>;
	instructions[0x27] = nullptr;
	instructions[0x28] = &Cpu::JR_cc<CpuFlagsZ,0>;
	instructions[0x29] = &Cpu::ADD_16bit<RegHL,RegHL>;
	instructions[0x2A] = &Cpu::LD<RegA,RegHL,ModeReg$RegLDI>;
	instructions[0x2B] = &Cpu::DEC_16bit<RegHL>;
	instructions[0x2C] = &Cpu::INC_8bit<RegL,0>;
	instructions[0x2D] = &Cpu::DEC_8bit<RegL,0>;
	instructions[0x2E] = &Cpu::LD<RegL,0,10>;
	instructions[0x2F] = &Cpu::CPL;

	instructions[0x30] = &Cpu::JR_cc<CpuFlagsC,1>;
	instructions[0x31] = &Cpu::LD<RegSP,0,ModeRegD16>;
	instructions[0x32] = &Cpu::LD<RegHL,RegA,Mode$RegRegLDD>;
	instructions[0x33] = &Cpu::INC_16bit<RegSP>;
	instructions[0x34] = &Cpu::INC_8bit<RegHL,1>;
	instructions[0x35] = &Cpu::DEC_8bit<RegHL,1>;
	instructions[0x36] = &Cpu::LD<RegHL,0,11>;
	instructions[0x37] = nullptr;
	instructions[0x38] = &Cpu::JR_cc<CpuFlagsC,0>;
	instructions[0x39] = &Cpu::ADD_16bit<RegHL,RegSP>;
	instructions[0x3A] = &Cpu::LD<RegA,RegHL,ModeReg$RegLDD>;
	instructions[0x3B] = &Cpu::DEC_16bit<RegSP>;
	instructions[0x3C] = &Cpu::INC_8bit<RegA,0>;
	instructions[0x3D] = &Cpu::DEC_8bit<RegA,0>;
	instructions[0x3E] = &Cpu::LD<RegA,0,10>;
	instructions[0x3F] = nullptr;

	instructions[0x40] = &Cpu::LD<RegB,RegB,ModeRegReg>;
	instructions[0x41] = &Cpu::LD<RegB,RegC,ModeRegReg>;
	instructions[0x42] = &Cpu::LD<RegB,RegD,ModeRegReg>;
	instructions[0x43] = &Cpu::LD<RegB,RegE,ModeRegReg>;
	instructions[0x44] = &Cpu::LD<RegB,RegH,ModeRegReg>;
	instructions[0x45] = &Cpu::LD<RegB,RegL,ModeRegReg>;
	instructions[0x46] = &Cpu::LD<RegB,RegHL,ModeReg$Reg>;
	instructions[0x47] = &Cpu::LD<RegB,RegA,ModeRegReg>;
	instructions[0x48] = &Cpu::LD<RegC,RegB,ModeRegReg>;
	instructions[0x49] = &Cpu::LD<RegC,RegC,ModeRegReg>;
	instructions[0x4A] = &Cpu::LD<RegC,RegD,ModeRegReg>;
	instructions[0x4B] = &Cpu::LD<RegC,RegE,ModeRegReg>;
	instructions[0x4C] = &Cpu::LD<RegC,RegH,ModeRegReg>;
	instructions[0x4D] = &Cpu::LD<RegC,RegL,ModeRegReg>;
	instructions[0x4E] = &Cpu::LD<RegC,RegHL,ModeReg$Reg>;
	instructions[0x4F] = &Cpu::LD<RegC,RegA,ModeRegReg>;

	instructions[0x50] = &Cpu::LD<RegD,RegB,ModeRegReg>;
	instructions[0x51] = &Cpu::LD<RegD,RegC,ModeRegReg>;
	instructions[0x52] = &Cpu::LD<RegD,RegD,ModeRegReg>;
	instructions[0x53] = &Cpu::LD<RegD,RegE,ModeRegReg>;
	instructions[0x54] = &Cpu::LD<RegD,RegH,ModeRegReg>;
	instructions[0x55] = &Cpu::LD<RegD,RegL,ModeRegReg>;
	instructions[0x56] = &Cpu::LD<RegD,RegHL,ModeReg$Reg>;
	instructions[0x57] = &Cpu::LD<RegD,RegA,ModeRegReg>;
	instructions[0x58] = &Cpu::LD<RegE,RegB,ModeRegReg>;
	instructions[0x59] = &Cpu::LD<RegE,RegC,ModeRegReg>;
	instructions[0x5A] = &Cpu::LD<RegE,RegD,ModeRegReg>;
	instructions[0x5B] = &Cpu::LD<RegE,RegE,ModeRegReg>;
	instructions[0x5C] = &Cpu::LD<RegE,RegH,ModeRegReg>;
	instructions[0x5D] = &Cpu::LD<RegE,RegL,ModeRegReg>;
	instructions[0x5E] = &Cpu::LD<RegE,RegHL,ModeReg$Reg>;
	instructions[0x5F] = &Cpu::LD<RegE,RegA,ModeRegReg>;

	instructions[0x60] = &Cpu::LD<RegH,RegB,ModeRegReg>;
	instructions[0x61] = &Cpu::LD<RegH,RegC,ModeRegReg>;
	instructions[0x62] = &Cpu::LD<RegH,RegD,ModeRegReg>;
	instructions[0x63] = &Cpu::LD<RegH,RegE,ModeRegReg>;
	instructions[0x64] = &Cpu::LD<RegH,RegH,ModeRegReg>;
	instructions[0x65] = &Cpu::LD<RegH,RegL,ModeRegReg>;
	instructions[0x66] = &Cpu::LD<RegH,RegHL,ModeReg$Reg>;
	instructions[0x67] = &Cpu::LD<RegH,RegA,ModeRegReg>;
	instructions[0x68] = &Cpu::LD<RegL,RegB,ModeRegReg>;
	instructions[0x69] = &Cpu::LD<RegL,RegC,ModeRegReg>;
	instructions[0x6A] = &Cpu::LD<RegL,RegD,ModeRegReg>;
	instructions[0x6B] = &Cpu::LD<RegL,RegE,ModeRegReg>;
	instructions[0x6C] = &Cpu::LD<RegL,RegH,ModeRegReg>;
	instructions[0x6D] = &Cpu::LD<RegL,RegL,ModeRegReg>;
	instructions[0x6E] = &Cpu::LD<RegL,RegHL,ModeReg$Reg>;
	instructions[0x6F] = &Cpu::LD<RegL,RegA,ModeRegReg>;

	instructions[0x70] = &Cpu::LD<RegHL,RegB,Mode$RegReg>;
	instructions[0x71] = &Cpu::LD<RegHL,RegC,Mode$RegReg>;
	instructions[0x72] = &Cpu::LD<RegHL,RegD,Mode$RegReg>;
	instructions[0x73] = &Cpu::LD<RegHL,RegE,Mode$RegReg>;
	instructions[0x74] = &Cpu::LD<RegHL,RegH,Mode$RegReg>;
	instructions[0x75] = &Cpu::LD<RegHL,RegL,Mode$RegReg>;
	instructions[0x76] = &Cpu::HALT;
	instructions[0x77] = &Cpu::LD<RegHL,RegA,Mode$RegReg>;
	instructions[0x78] = &Cpu::LD<RegA,RegB,ModeRegReg>;
	instructions[0x79] = &Cpu::LD<RegA,RegC,ModeRegReg>;
	instructions[0x7A] = &Cpu::LD<RegA,RegD,ModeRegReg>;
	instructions[0x7B] = &Cpu::LD<RegA,RegE,ModeRegReg>;
	instructions[0x7C] = &Cpu::LD<RegA,RegH,ModeRegReg>;
	instructions[0x7D] = &Cpu::LD<RegA,RegL,ModeRegReg>;
	instructions[0x7E] = &Cpu::LD<RegA,RegHL,ModeReg$Reg>;
	instructions[0x7F] = &Cpu::LD<RegA,RegA,ModeRegReg>;

	instructions[0x80] = &Cpu::ADD<RegA,RegB,0>;
	instructions[0x81] = &Cpu::ADD<RegA,RegC,0>;
	instructions[0x82] = &Cpu::ADD<RegA,RegD,0>;
	instructions[0x83] = &Cpu::ADD<RegA,RegE,0>;
	instructions[0x84] = &Cpu::ADD<RegA,RegH,0>;
	instructions[0x85] = &Cpu::ADD<RegA,RegL,0>;
	instructions[0x86] = &Cpu::ADD<RegA,RegHL,1>;
	instructions[0x87] = &Cpu::ADD<RegA,RegA,0>;
	instructions[0x88] = &Cpu::ADC<RegA,RegB,0>;
	instructions[0x89] = &Cpu::ADC<RegA,RegC,0>;
	instructions[0x8A] = &Cpu::ADC<RegA,RegD,0>;
	instructions[0x8B] = &Cpu::ADC<RegA,RegE,0>;
	instructions[0x8C] = &Cpu::ADC<RegA,RegH,0>;
	instructions[0x8D] = &Cpu::ADC<RegA,RegL,0>;
	instructions[0x8E] = &Cpu::ADC<RegA,RegHL,1>;
	instructions[0x8F] = &Cpu::ADC<RegA,RegA,0>;

	instructions[0x90] = &Cpu::SUB<RegA,RegB,0>;
	instructions[0x91] = &Cpu::SUB<RegA,RegC,0>;
	instructions[0x92] = &Cpu::SUB<RegA,RegD,0>;
	instructions[0x93] = &Cpu::SUB<RegA,RegE,0>;
	instructions[0x94] = &Cpu::SUB<RegA,RegH,0>;
	instructions[0x95] = &Cpu::SUB<RegA,RegL,0>;
	instructions[0x96] = &Cpu::SUB<RegA,RegHL,1>;
	instructions[0x97] = &Cpu::SUB<RegA,RegA,0>;
	instructions[0x98] = &Cpu::SBC<RegA,RegB,0>;
	instructions[0x99] = &Cpu::SBC<RegA,RegC,0>;
	instructions[0x9A] = &Cpu::SBC<RegA,RegD,0>;
	instructions[0x9B] = &Cpu::SBC<RegA,RegE,0>;
	instructions[0x9C] = &Cpu::SBC<RegA,RegH,0>;
	instructions[0x9D] = &Cpu::SBC<RegA,RegL,0>;
	instructions[0x9E] = &Cpu::SBC<RegA,RegHL,1>;
	instructions[0x9F] = &Cpu::SBC<RegA,RegA,0>;

	instructions[0xA0] = &Cpu::AND<RegA,RegB,0>;
	instructions[0xA1] = &Cpu::AND<RegA,RegC,0>;
	instructions[0xA2] = &Cpu::AND<RegA,RegD,0>;
	instructions[0xA3] = &Cpu::AND<RegA,RegE,0>;
	instructions[0xA4] = &Cpu::AND<RegA,RegH,0>;
	instructions[0xA5] = &Cpu::AND<RegA,RegL,0>;
	instructions[0xA6] = &Cpu::AND<RegA,RegHL,1>;
	instructions[0xA7] = &Cpu::AND<RegA,RegA,0>;
	instructions[0xA8] = &Cpu::XOR<RegA,RegB,0>;
	instructions[0xA9] = &Cpu::XOR<RegA,RegC,0>;
	instructions[0xAA] = &Cpu::XOR<RegA,RegD,0>;
	instructions[0xAB] = &Cpu::XOR<RegA,RegE,0>;
	instructions[0xAC] = &Cpu::XOR<RegA,RegH,0>;
	instructions[0xAD] = &Cpu::XOR<RegA,RegL,0>;
	instructions[0xAE] = &Cpu::XOR<RegA,RegHL,1>;
	instructions[0xAF] = &Cpu::XOR<RegA,RegA,0>;

	instructions[0xB0] = &Cpu::OR<RegA,RegB,0>;
	instructions[0xB1] = &Cpu::OR<RegA,RegC,0>;
	instructions[0xB2] = &Cpu::OR<RegA,RegD,0>;
	instructions[0xB3] = &Cpu::OR<RegA,RegE,0>;
	instructions[0xB4] = &Cpu::OR<RegA,RegH,0>;
	instructions[0xB5] = &Cpu::OR<RegA,RegL,0>;
	instructions[0xB6] = &Cpu::OR<RegA,RegHL,1>;
	instructions[0xB7] = &Cpu::OR<RegA,RegA,0>;
  instructions[0xB8] = &Cpu::CP_reg<RegB>;
  instructions[0xB9] = &Cpu::CP_reg<RegC>;
  instructions[0xBA] = &Cpu::CP_reg<RegD>;
  instructions[0xBB] = &Cpu::CP_reg<RegE>;
  instructions[0xBC] = &Cpu::CP_reg<RegH>;
  instructions[0xBD] = &Cpu::CP_reg<RegL>;
  instructions[0xBE] = &Cpu::CP_HL;
  instructions[0xBF] = &Cpu::CP_reg<RegA>;
	//-checked above
	instructions[0xC0] = &Cpu::RET_cc<CpuFlagsZ,1>;
	instructions[0xC1] = &Cpu::POP<RegBC>;
	instructions[0xC2] = &Cpu::JP_cc<CpuFlagsZ,1>;
	instructions[0xC3] = &Cpu::JP;
	instructions[0xC4] = &Cpu::CALL_cc<CpuFlagsZ,1>;
	instructions[0xC5] = &Cpu::PUSH<RegBC>;
  instructions[0xC6] = &Cpu::ADD<RegA,0,2>;
	instructions[0xC7] = &Cpu::RST;
	instructions[0xC8] = &Cpu::RET_cc<CpuFlagsZ,0>;
	instructions[0xC9] = &Cpu::RET;
	instructions[0xCA] = &Cpu::JP_cc<CpuFlagsZ,0>;
	instructions[0xCB] = &Cpu::PREFIX_CB;
	instructions[0xCC] = &Cpu::CALL_cc<CpuFlagsZ,0>;
	instructions[0xCD] = &Cpu::CALL;
	instructions[0xCE] = &Cpu::ADC<RegA,0,2>;
	instructions[0xCF] = &Cpu::RST;

	instructions[0xD0] = &Cpu::RET_cc<CpuFlagsC,1>;
	instructions[0xD1] = &Cpu::POP<RegDE>;
	instructions[0xD2] = &Cpu::JP_cc<CpuFlagsC,1>;
	instructions[0xD3] = &Cpu::ILLEGAL;
	instructions[0xD4] = &Cpu::CALL_cc<CpuFlagsC,1>;
	instructions[0xD5] = &Cpu::PUSH<RegDE>;
  instructions[0xD6] = &Cpu::SUB<RegA,0,2>;
	instructions[0xD7] = &Cpu::RST;
	instructions[0xD8] = &Cpu::RET_cc<CpuFlagsC,0>;
	instructions[0xD9] = &Cpu::RETI;
	instructions[0xDA] = &Cpu::JP_cc<CpuFlagsC,0>;
	instructions[0xDB] = &Cpu::ILLEGAL;
	instructions[0xDC] = &Cpu::CALL_cc<CpuFlagsC,0>;
	instructions[0xDD] = &Cpu::ILLEGAL;
	instructions[0xDE] = &Cpu::SBC<RegA,0,2>;
	instructions[0xDF] = &Cpu::RST;

	instructions[0xE0] = &Cpu::LD<0,RegA,12>;
	instructions[0xE1] = &Cpu::POP<RegHL>;
	instructions[0xE2] = &Cpu::LD<RegC,RegA,Mode$FF00RegReg>;
	instructions[0xE3] = &Cpu::ILLEGAL;
	instructions[0xE4] = &Cpu::ILLEGAL;
	instructions[0xE5] = &Cpu::PUSH<RegHL>;
  instructions[0xE6] = &Cpu::AND<RegA,0,2>;
	instructions[0xE7] = &Cpu::RST;
	instructions[0xE8] = &Cpu::ADD_SPr8;
	instructions[0xE9] = &Cpu::JP_HL;
	instructions[0xEA] = &Cpu::LD<0,RegA,14>;
	instructions[0xEB] = &Cpu::ILLEGAL;
	instructions[0xEC] = &Cpu::ILLEGAL;
	instructions[0xED] = &Cpu::ILLEGAL;
	instructions[0xEE] = &Cpu::XOR<RegA,0,2>;
	instructions[0xEF] = &Cpu::RST;

	instructions[0xF0] = &Cpu::LD<RegA,0,13>;
	instructions[0xF1] = &Cpu::POP<RegAF>;
	instructions[0xF2] = &Cpu::LD<RegA,RegC,ModeReg$FF00Reg>;
	instructions[0xF3] = &Cpu::DI;
	instructions[0xF4] = &Cpu::ILLEGAL;
	instructions[0xF5] = &Cpu::PUSH<RegAF>;
  instructions[0xF6] = &Cpu::OR<RegA,0,2>;
	instructions[0xF7] = &Cpu::RST;
	instructions[0xF8] = nullptr;
	instructions[0xF9] = nullptr;
	instructions[0xFA] = &Cpu::LD<RegA,0,15>;
	instructions[0xFB] = &Cpu::EI;
	instructions[0xFC] = &Cpu::ILLEGAL;
	instructions[0xFD] = &Cpu::ILLEGAL;
	instructions[0xFE] = &Cpu::CP_d8;
  instructions[0xFF] = &Cpu::RST;
}

Cpu::~Cpu() {

}

void Cpu::Initialize(Emu* emu) {
  Component::Initialize(emu);
  mem_ = emu_->memory();
  Reset();
}

void Cpu::Deinitialize() {

}

void Cpu::Reset() {
	memset(&reg,0,sizeof(reg));
  reg.PC = 0;
  cycles = 0;
  ime = false;
}

void Cpu::Tick() {
  ++cycles;
  emu_->lcd_driver()->Step(dt);
	emu_->sc()->Step(dt);
}

void Cpu::Step(double dt) {
  this->dt = dt;
	cycles = 0;
  reg.F._unused = 0;//always 0 according to docs
  if (reg.PC == 0x100) {
//    DebugBreak();
  /* mem_->Write8(0xFF05,0x00); // TIMA
   mem_->Write8(0xFF06,0x00); // TMA
   mem_->Write8(0xFF07,0x00); // TAC
   mem_->Write8(0xFF10,0x80); // NR10
   mem_->Write8(0xFF11,0xBF); // NR11
   mem_->Write8(0xFF12,0xF3); // NR12
   mem_->Write8(0xFF14,0xBF); // NR14
   mem_->Write8(0xFF16,0x3F); // NR21
   mem_->Write8(0xFF17,0x00); // NR22
   mem_->Write8(0xFF19,0xBF); // NR24
   mem_->Write8(0xFF1A,0x7F); // NR30
   mem_->Write8(0xFF1B,0xFF); // NR31
   mem_->Write8(0xFF1C,0x9F); // NR32
   mem_->Write8(0xFF1E,0xBF); // NR33
   mem_->Write8(0xFF20,0xFF); // NR41
   mem_->Write8(0xFF21,0x00); // NR42
   mem_->Write8(0xFF22,0x00); // NR43
   mem_->Write8(0xFF23,0xBF); // NR30
   mem_->Write8(0xFF24,0x77); // NR50
   mem_->Write8(0xFF25,0xF3); // NR51
   mem_->Write8(0xFF26,0xF1); //NR52
   mem_->Write8(0xFF40,0x91); // LCDC
   mem_->Write8(0xFF42,0x00); // SCY
   mem_->Write8(0xFF43,0x00); // SCX
   mem_->Write8(0xFF45,0x00); // LYC
   mem_->Write8(0xFF47,0xFC); // BGP
   mem_->Write8(0xFF48,0xFF); // OBP0
   mem_->Write8(0xFF49,0xFF); // OBP1
   mem_->Write8(0xFF4A,0x00); // WY
   mem_->Write8(0xFF4B,0x00); // WX
   mem_->Write8(0xFFFF,0x00); // IE*/
	}
//	StopAt(0x0073);
  opcode = emu_->memory()->Read8(reg.PC++);
  (this->*(instructions[opcode]))();
  if (ime) {
    uint8_t test = emu_->memory()->interrupt_enable() & emu_->memory()->interrupt_flag();
		if (test) {
			ime = false;
			pushSP();
			if (test & 0x1) { //vblank
				reg.PC = 0x0040;
				emu_->memory()->interrupt_flag() &= ~0x1;
			} else if (test & 0x2) {
				reg.PC = 0x0048; //lcdc status
				emu_->memory()->interrupt_flag() &= ~0x2;
			} else if (test & 0x4) {
				reg.PC = 0x0050; //timer overflow
				emu_->memory()->interrupt_flag() &= ~0x4;
			} else if (test & 0x8) {
				reg.PC = 0x0058; //serial transfer
				emu_->memory()->interrupt_flag() &= ~0x8;
			} else if (test & 0x10) {
				reg.PC = 0x0060; //hi-lo p10-p13
				emu_->memory()->interrupt_flag() &= ~0x10;
			}
		}
  }
}

void Cpu::NOP() { 
  //Tick();
  //Tick();
  //Tick();
  //Tick();
}

void Cpu::ILLEGAL() {
	int a = 1;
}

void Cpu::RST() {
  pushPC();
  uint8_t t = (opcode&0x38)>>3;
  reg.PC = t*8;
  Tick();Tick();Tick();Tick();
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::LD() {
	if (mode == 0) { //reg reg
		reg.raw8[dest] = reg.raw8[src];
	}	else if (mode == 1) { //(dest), src
		mem_->Write8(reg.raw16[dest],reg.raw8[src]);
	} else if (mode == 2) { //dest, (src)
		reg.raw8[dest] = mem_->Read8(reg.raw16[src]);
	} else if (mode == 3) { //0xFF00+dest src
		mem_->Write8(0xFF00+reg.raw8[dest],reg.raw8[src]);
	} else if (mode == 4) { //dest,0xFF00+src 
		reg.raw8[dest] = mem_->Read8(0xFF00+reg.raw8[src]);
	} else if (mode == 5) { //dest,d16
		reg.raw16[dest] = mem_->Read8(reg.PC++);
		reg.raw16[dest] |= (mem_->Read8(reg.PC++))<<8;
	} else if (mode == 6) { //(dest),src - ldi
		mem_->Write8(reg.raw16[dest],reg.raw8[src]);
		++reg.raw16[dest];
	} else if (mode == 7) { //(dest),src - ldd
		mem_->Write8(reg.raw16[dest],reg.raw8[src]);
		--reg.raw16[dest];
	} else if (mode == 8) { //dest,(src) - ldi
		reg.raw8[dest] = mem_->Read8(reg.raw16[src]);
		++reg.raw16[src];
	} else if (mode == 9) { //src,(dest) - ldd
		reg.raw8[dest] = mem_->Read8(reg.raw16[src]);
		--reg.raw16[src];
	} else if (mode == 10) { //dest,d8
		reg.raw8[dest] = mem_->Read8(reg.PC++);
	}else if (mode == 11) { //(dest),d8
		auto d8 = mem_->Read8(reg.PC++);
		mem_->Write8(reg.raw16[dest],d8);
	} else if (mode == 12) { //0xFF00+d8 src
    uint8_t dest = mem_->Read8(reg.PC++);
		mem_->Write8(0xFF00+dest,reg.raw8[src]);
	} else if (mode == 13) { //dest,0xFF00+d8 
    uint8_t src = mem_->Read8(reg.PC++);
		reg.raw8[dest] = mem_->Read8(0xFF00+src);
	} else if (mode == 14) { //(d16),src
		uint16_t d16 = mem_->Read8(reg.PC++);
		d16 |= (mem_->Read8(reg.PC++))<<8;
    mem_->Write8(d16,reg.raw8[src]);
	} else if (mode == 15) { //src,(d16)
		uint16_t d16 = mem_->Read8(reg.PC++);
		d16 |= (mem_->Read8(reg.PC++))<<8;
    reg.raw8[dest] = mem_->Read8(d16);
	}

 // for (int i=0;i<cycles;++i)
    //Tick();
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::ADD() {
  reg.F.N = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);

  reg.raw8[dest] = a + b;
  updateCpuFlagC(a,b,0);
  updateCpuFlagH(a,b,0);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src>
void Cpu::ADD_16bit() {
  reg.F.N = 0;
  uint16_t a=0,b=0;
  a = reg.raw16[dest];
  b = reg.raw16[src];
  reg.raw16[dest] = a + b;
  uint16_t r1 = (a&0xFFF) + (b&0xFFF);
  reg.F.H = r1>0xFFF?1:0;
  uint32_t r2 = (a&0xFFFF) + (b&0xFFFF);
  reg.F.C = r2>0xFFFF?1:0;
	Tick();Tick();Tick();Tick();
}

void Cpu::ADD_SPr8() {
  reg.F.N  = reg.F.Z = 0;
	uint16_t a = reg.SP;
	int8_t r8 = mem_->Read8(reg.PC++);
	reg.SP += r8;

  uint16_t r1 = (a&0xFFF) + (r8&0xFFF);
  reg.F.H = r1>0xFFF?1:0;
  uint32_t r2 = (a&0xFFFF) + (r8&0xFFFF);
  reg.F.C = r2>0xFFFF?1:0;
	Tick();Tick();Tick();Tick();
	Tick();Tick();Tick();Tick();
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::ADC() {
  reg.F.N = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  b += reg.F.C;
  reg.raw8[dest] = a + b;
  updateCpuFlagC(a,b,0);
  updateCpuFlagH(a,b,0);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::SUB() {
  reg.F.N = 1;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a - b;
  updateCpuFlagC(a,b,1);
  updateCpuFlagH(a,b,1);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::SBC() {
  reg.F.N = 1;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  b += reg.F.C;
  reg.raw8[dest] = a - b;
  updateCpuFlagC(a,b,1);
  updateCpuFlagH(a,b,1);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::AND() {
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a & b;
  reg.F.H = 1;
  updateCpuFlagZ(reg.raw8[dest]);
}


template<uint8_t dest,uint8_t src,int mode>
void Cpu::XOR() {
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a ^ b;
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::OR() {
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a | b;
  updateCpuFlagZ(reg.raw8[dest]);
}


void Cpu::HALT() {
  int a = 1;
}

void Cpu::CPL() {
  reg.F.H = 1;
  reg.F.N = 1;
  reg.A = ~reg.A;
}
const CpuRegisterNames8 reg_index[8] = {
    RegB,RegC,RegD,RegE,RegH,RegL,RegH,RegA,
  };

void Cpu::PREFIX_CB() {

  

	uint8_t code = emu_->memory()->Read8(reg.PC++);

  auto getr = [=]() {
    if ((code&0x7) != 6) {
      return reg.raw8[reg_index[code&0x7]];
    } else {
      return mem_->Read8(reg.HL);
    }
  };

  auto setr = [=](uint8_t r) {
    if ((code&0x7) != 6) {
      reg.raw8[reg_index[code&0x7]] = r;
    
    } else {
      mem_->Write8(reg.HL,r);
     
    }
  };

  if ((code & 0xF8) == 0x30) { //swap
    uint8_t r = getr();
    uint8_t n0 = r&0xF;
    uint8_t n1 = r&0xF0;
    r = n1 | (n0<<4);
    setr(r);
    updateCpuFlagZ(r);
  } else if ((code & 0xC0) == 0x40) {
    uint8_t test = getr();
    uint8_t bitshift = (code&0x38) >> 3;
    reg.F.Z = (~(((test&(1<<bitshift))>>bitshift))&0x1);
    reg.F.H = 1;
  } else if ((code&0xF8) == 0) { //RLC r
    uint8_t r = getr();
    reg.F.C = (r&0x80)!=0?1:0;
    r = r << 1;
    r |= reg.F.C;
    setr(r);
    updateCpuFlagZ(r);
  } else if ((code&0xF8) == 0x10) { //RL r
    uint8_t r = getr();
    uint8_t oldC = reg.F.C;
    reg.F.C = (r&0x80)!=0?1:0;
    r = r << 1;
    r |= oldC;
    setr(r);
    updateCpuFlagZ(r);
  } else if ((code&0xF8) == 0x18) { //RR r
    uint8_t r = getr();
    uint8_t oldC = reg.F.C;
    reg.F.C = r&1;
    r = r >> 1;
    r |= oldC<<7;
    setr(r);
    updateCpuFlagZ(r);

  } else if ((code & 0xF8) == 0x38) {
    auto r = getr();

    reg.F.C = r&1;
    r >>= 1;
    setr(r);
    updateCpuFlagZ(r);
  }
  else {
    int a = 1;
  }
  Tick();Tick();Tick();Tick();
}

void Cpu::JR() {
	 int8_t disp8 = mem_->Read8(reg.PC++);
   reg.PC += disp8;
   Tick();Tick();Tick();Tick();
}

template<CpuFlags condbit,bool inv>
void Cpu::JR_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
		JR();
  } else {
    int8_t disp8 = mem_->Read8(reg.PC++);
  }

}

template<uint8_t dest,int mode>
void Cpu::INC_8bit() {
  reg.F.N = 0;

  if (mode==0) {
    updateCpuFlagH(reg.raw8[dest],1,0);
    ++reg.raw8[dest];
  } else {
    uint8_t data = mem_->Read8(reg.HL);
		updateCpuFlagH(data,1,0);
    ++data;
    mem_->Write8(reg.HL,data);
  }
    
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest>
void Cpu::INC_16bit() {
  ++reg.raw16[dest];
  Tick();Tick();Tick();Tick();
}

template<uint8_t dest,int mode>
void Cpu::DEC_8bit() {
  reg.F.N = 1;

  if (mode==0) {
    updateCpuFlagH(reg.raw8[dest],1,1);
    --reg.raw8[dest];
  } else {
    uint8_t data = mem_->Read8(reg.HL);
    updateCpuFlagH(data,1,1);
    --data;
    mem_->Write8(reg.HL,data);
  }
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest>
void Cpu::DEC_16bit() {
  --reg.raw16[dest];
  Tick();Tick();Tick();Tick();
}

void Cpu::JP() {
  uint16_t nn;
  nn = mem_->Read8(reg.PC++);
  nn |= (mem_->Read8(reg.PC++))<<8;
  reg.PC = nn;

  Tick();Tick();Tick();Tick();
}

template<CpuFlags condbit,bool inv>
void Cpu::JP_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    JP();
  } else {
    uint16_t nn;
    nn = mem_->Read8(reg.PC++);
    nn |= (mem_->Read8(reg.PC++))<<8;
  }
}

void Cpu::JP_HL() {
  uint16_t nn;
  nn = mem_->Read8(reg.HL);
  nn |= (mem_->Read8(reg.HL))<<8;
  reg.PC = nn;
}


void Cpu::CALL() {
  uint16_t nn;
  nn = mem_->Read8(reg.PC++);
  nn |= (mem_->Read8(reg.PC++))<<8;
  pushPC();
  reg.PC = nn;

  Tick();Tick();Tick();Tick();
}

template<CpuFlags condbit,bool inv>
void Cpu::CALL_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    CALL();
  } else {
    uint16_t nn;
    nn = mem_->Read8(reg.PC++);
    nn |= (mem_->Read8(reg.PC++))<<8;
  }
}

void Cpu::RET() {
  reg.PC = pop();
  reg.PC |= pop() << 8;
  Tick();Tick();Tick();Tick();
}

template<CpuFlags condbit,bool inv>
void Cpu::RET_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    RET();
  } else {
   
  }
   Tick();Tick();Tick();Tick();
}


template<uint8_t src>
void Cpu::PUSH() {
  push(reg.raw16[src]>>8);
  push(reg.raw16[src]&0xFF);
  Tick();Tick();Tick();Tick();
}

template<uint8_t dest>
void Cpu::POP() {
  reg.raw16[dest] = pop();
  reg.raw16[dest] |= pop() << 8;

}

void Cpu::RLCA() {
  uint8_t& r = reg.A;
  reg.F.C = (r&0x80)!=0?1:0;
  r = r << 1;
  r |= reg.F.C;
}

void Cpu::RRCA() {
  uint8_t& r = reg.A;
  reg.F.C = (r&0x80)!=0?1:0;
  r = r >> 1;
  r |= reg.F.C<<7;
}

void Cpu::RLA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = (r&0x80)>>7;
  r = r << 1;
  r |= oldC;
}

void Cpu::RRA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = r & 1;
  r = r >> 1;
  r |= oldC << 7;
}

void Cpu::CP(uint8_t a, uint8_t b) {
  reg.F.N = 1;
  updateCpuFlagC(a,b,1);
  updateCpuFlagH(a,b,1);
  updateCpuFlagZ(a-b);
}

template<CpuRegisterNames8 r>
void Cpu::CP_reg() {
  CP(reg.A,reg.raw8[r]);
}

void Cpu::CP_d8() {
  CP(reg.A,mem_->Read8(reg.PC++));
}

void Cpu::CP_HL() {
  CP(reg.A,mem_->Read8(reg.HL));
}

void Cpu::DI() {
  ime = false;
}

void Cpu::EI() {
  ime = true;
}


void Cpu::RETI() {
	EI();
	RET();
}

}
}