#include "gb.h"
/*


*/

namespace emulation {
namespace gb {



Cpu::Cpu() {
	instructions[0x00] = &Cpu::NOP;


	instructions[0x01] = &Cpu::LD<RegBC,0,5,12>;
	instructions[0x11] = &Cpu::LD<RegDE,0,5,12>;
	instructions[0x21] = &Cpu::LD<RegHL,0,5,12>;
	instructions[0x31] = &Cpu::LD<RegSP,0,5,12>;

	
	instructions[0x02] = &Cpu::LD<RegBC,RegA,1,8>;
	instructions[0x12] = &Cpu::LD<RegDE,RegA,1,8>;
	instructions[0x22] = &Cpu::LD<RegHL,RegA,6,8>;
	instructions[0x32] = &Cpu::LD<RegHL,RegA,7,8>;

	instructions[0x06] = &Cpu::LD<RegB,0,10,8>;
	instructions[0x16] = &Cpu::LD<RegD,0,10,8>;
	instructions[0x26] = &Cpu::LD<RegH,0,10,8>;
	instructions[0x36] = &Cpu::LD<RegHL,0,11,12>;

	instructions[0x0A] = &Cpu::LD<RegA,RegBC,2,8>;
	instructions[0x1A] = &Cpu::LD<RegA,RegDE,2,8>;
	instructions[0x2A] = &Cpu::LD<RegA,RegHL,8,8>;
	instructions[0x3A] = &Cpu::LD<RegA,RegHL,9,8>;

	instructions[0x0E] = &Cpu::LD<RegC,0,10,8>;
	instructions[0x1E] = &Cpu::LD<RegE,0,10,8>;
	instructions[0x2E] = &Cpu::LD<RegL,0,10,8>;
	instructions[0x3E] = &Cpu::LD<RegA,0,10,8>;

	instructions[0x40] = &Cpu::LD<RegB,RegB,0,4>;
	instructions[0x41] = &Cpu::LD<RegB,RegC,0,4>;
	instructions[0x42] = &Cpu::LD<RegB,RegD,0,4>;
	instructions[0x43] = &Cpu::LD<RegB,RegE,0,4>;
	instructions[0x44] = &Cpu::LD<RegB,RegH,0,4>;
	instructions[0x45] = &Cpu::LD<RegB,RegL,0,4>;
	instructions[0x46] = &Cpu::LD<RegB,RegHL,2,8>;
	instructions[0x47] = &Cpu::LD<RegB,RegA,0,4>;
	instructions[0x48] = &Cpu::LD<RegC,RegB,0,4>;
	instructions[0x49] = &Cpu::LD<RegC,RegC,0,4>;
	instructions[0x4A] = &Cpu::LD<RegC,RegD,0,4>;
	instructions[0x4B] = &Cpu::LD<RegC,RegE,0,4>;
	instructions[0x4C] = &Cpu::LD<RegC,RegH,0,4>;
	instructions[0x4D] = &Cpu::LD<RegC,RegL,0,4>;
	instructions[0x4E] = &Cpu::LD<RegC,RegHL,2,8>;
	instructions[0x4F] = &Cpu::LD<RegC,RegA,0,4>;

	instructions[0x50] = &Cpu::LD<RegD,RegB,0,4>;
	instructions[0x51] = &Cpu::LD<RegD,RegC,0,4>;
	instructions[0x52] = &Cpu::LD<RegD,RegD,0,4>;
	instructions[0x53] = &Cpu::LD<RegD,RegE,0,4>;
	instructions[0x54] = &Cpu::LD<RegD,RegH,0,4>;
	instructions[0x55] = &Cpu::LD<RegD,RegL,0,4>;
	instructions[0x56] = &Cpu::LD<RegD,RegHL,2,8>;
	instructions[0x57] = &Cpu::LD<RegD,RegA,0,4>;
	instructions[0x58] = &Cpu::LD<RegE,RegB,0,4>;
	instructions[0x59] = &Cpu::LD<RegE,RegC,0,4>;
	instructions[0x5A] = &Cpu::LD<RegE,RegD,0,4>;
	instructions[0x5B] = &Cpu::LD<RegE,RegE,0,4>;
	instructions[0x5C] = &Cpu::LD<RegE,RegH,0,4>;
	instructions[0x5D] = &Cpu::LD<RegE,RegL,0,4>;
	instructions[0x5E] = &Cpu::LD<RegE,RegHL,2,8>;
	instructions[0x5F] = &Cpu::LD<RegE,RegA,0,4>;

	instructions[0x60] = &Cpu::LD<RegH,RegB,0,4>;
	instructions[0x61] = &Cpu::LD<RegH,RegC,0,4>;
	instructions[0x62] = &Cpu::LD<RegH,RegD,0,4>;
	instructions[0x63] = &Cpu::LD<RegH,RegE,0,4>;
	instructions[0x64] = &Cpu::LD<RegH,RegH,0,4>;
	instructions[0x65] = &Cpu::LD<RegH,RegL,0,4>;
	instructions[0x66] = &Cpu::LD<RegH,RegHL,2,8>;
	instructions[0x67] = &Cpu::LD<RegH,RegA,0,4>;
	instructions[0x68] = &Cpu::LD<RegL,RegB,0,4>;
	instructions[0x69] = &Cpu::LD<RegL,RegC,0,4>;
	instructions[0x6A] = &Cpu::LD<RegL,RegD,0,4>;
	instructions[0x6B] = &Cpu::LD<RegL,RegE,0,4>;
	instructions[0x6C] = &Cpu::LD<RegL,RegH,0,4>;
	instructions[0x6D] = &Cpu::LD<RegL,RegL,0,4>;
	instructions[0x6E] = &Cpu::LD<RegL,RegHL,2,8>;
	instructions[0x6F] = &Cpu::LD<RegL,RegA,0,4>;

	instructions[0x70] = &Cpu::LD<RegHL,RegB,1,8>;
	instructions[0x71] = &Cpu::LD<RegHL,RegC,1,8>;
	instructions[0x72] = &Cpu::LD<RegHL,RegD,1,8>;
	instructions[0x73] = &Cpu::LD<RegHL,RegE,1,8>;
	instructions[0x74] = &Cpu::LD<RegHL,RegH,1,8>;
	instructions[0x75] = &Cpu::LD<RegHL,RegL,1,8>;
	instructions[0x76] = &Cpu::HALT;
	instructions[0x77] = &Cpu::LD<RegHL,RegA,1,8>;
	instructions[0x78] = &Cpu::LD<RegA,RegB,0,4>;
	instructions[0x79] = &Cpu::LD<RegA,RegC,0,4>;
	instructions[0x7A] = &Cpu::LD<RegA,RegD,0,4>;
	instructions[0x7B] = &Cpu::LD<RegA,RegE,0,4>;
	instructions[0x7C] = &Cpu::LD<RegA,RegH,0,4>;
	instructions[0x7D] = &Cpu::LD<RegA,RegL,0,4>;
	instructions[0x7E] = &Cpu::LD<RegA,RegHL,2,8>;
	instructions[0x7F] = &Cpu::LD<RegA,RegA,0,4>;

	instructions[0xE2] = &Cpu::LD<RegC,RegA,3,8>;

	instructions[0x80] = &Cpu::ADD<RegA,RegB,0>;
	instructions[0x81] = &Cpu::ADD<RegA,RegC,0>;
	instructions[0x82] = &Cpu::ADD<RegA,RegD,0>;
	instructions[0x83] = &Cpu::ADD<RegA,RegE,0>;
	instructions[0x84] = &Cpu::ADD<RegA,RegH,0>;
	instructions[0x85] = &Cpu::ADD<RegA,RegL,0>;
	instructions[0x86] = &Cpu::ADD<RegA,RegHL,1>;
	instructions[0x87] = &Cpu::ADD<RegA,RegA,0>;

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

	instructions[0xCB] = &Cpu::PREFIX_CB;
	

}

Cpu::~Cpu() {

}

void Cpu::Initialize(Emu* emu) {
  Component::Initialize(emu);
	memset(&reg,0,sizeof(reg));
  reg.PC = 0;
  cycles = 0;
  mem_ = emu_->memory();
	
  
}

void Cpu::Deinitialize() {

}

void Cpu::Step() {
	cycles = 0;
  uint8_t opcode = emu_->memory()->Read8(reg.PC++);
  (this->*(instructions[opcode]))();
}

void Cpu::NOP() { 
  cycles += 4;
}

template<uint8_t dest,uint8_t src,int mode,uint8_t cycles>
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
	}
	this->cycles += cycles;
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::ADD() {
	uint16_t result;
	if (mode == 0)
		result = reg.raw8[dest] + reg.raw8[src];
	else {
		result = reg.raw8[dest] + mem_->Read8(reg.raw16[src]);
		cycles += 4;
	}
	reg.raw8[dest] = result & 0xFF;
	reg.F.raw = 0;
	reg.F.bcd_halfcarry = result & 0x10 >> 4;
	if (result > 0xFF)
		reg.F.carry = 1;
	if (result == 0)
		reg.F.zero = 1;
	cycles += 4;
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::AND() {
	if (mode == 0)
		reg.raw8[dest] = reg.raw8[dest] & reg.raw8[src];
	else {
		reg.raw8[dest] = reg.raw8[dest] & mem_->Read8(reg.raw16[src]);
		cycles += 4;
	}
	reg.F.raw = 0;
	reg.F.bcd_halfcarry = 1;
	if (reg.raw8[dest] == 0)
		reg.F.zero = 1;
	cycles += 4;
}


template<uint8_t dest,uint8_t src,int mode>
void Cpu::XOR() {
	if (mode == 0)
		reg.raw8[dest] = reg.raw8[dest] ^ reg.raw8[src];
	else {
		reg.raw8[dest] = reg.raw8[dest] ^ mem_->Read8(reg.raw16[src]);
		cycles += 4;
	}
	reg.F.raw = 0;
	if (reg.raw8[dest] == 0)
		reg.F.zero = 1;
	cycles += 4;
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::OR() {
	if (mode == 0)
		reg.raw8[dest] = reg.raw8[dest] | reg.raw8[src];
	else {
		reg.raw8[dest] = reg.raw8[dest] | mem_->Read8(reg.raw16[src]);
		cycles += 4;
	}
	reg.F.raw = 0;
	if (reg.raw8[dest] == 0)
		reg.F.zero = 1;
	cycles += 4;
}


void Cpu::HALT() {
	int a = 1;
	cycles += 4;
}

void Cpu::PREFIX_CB() {


	uint8_t opcode = emu_->memory()->Read8(reg.PC++);


	cycles += 4;
}

}
}