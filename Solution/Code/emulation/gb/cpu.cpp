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
#include "gb.h"
/*


*/

#define StopAt(x) { if (x == reg.PC) DebugBreak(); } 

namespace emulation {
namespace gb {


#define MachineCycle { Tick();Tick();Tick();Tick(); }

Cpu::Cpu() {
  instructions[0x00] = &Cpu::NOP;
  instructions[0x01] = &Cpu::LDrd16<RegBC>;
  instructions[0x02] = &Cpu::LD$rr<RegBC,RegA>;
  instructions[0x03] = &Cpu::INC_16bit<RegBC>;
  instructions[0x04] = &Cpu::INC_8bit<RegB,0>;
  instructions[0x05] = &Cpu::DEC_8bit<RegB,0>;
  instructions[0x06] = &Cpu::LD<RegB,0,10>;
  instructions[0x07] = &Cpu::RLCA;
  instructions[0x08] = &Cpu::LDa16SP;
  instructions[0x09] = &Cpu::ADD_16bit<RegHL,RegBC>;
  instructions[0x0A] = &Cpu::LDr$r<RegA,RegBC>;
  instructions[0x0B] = &Cpu::DEC_16bit<RegBC>;
  instructions[0x0C] = &Cpu::INC_8bit<RegC,0>;
  instructions[0x0D] = &Cpu::DEC_8bit<RegC,0>;
  instructions[0x0E] = &Cpu::LD<RegC,0,10>;
  instructions[0x0F] = &Cpu::RRCA;

  instructions[0x10] = &Cpu::STOP;
  instructions[0x11] = &Cpu::LDrd16<RegDE>;
  instructions[0x12] = &Cpu::LD$rr<RegDE,RegA>;
  instructions[0x13] = &Cpu::INC_16bit<RegDE>;
  instructions[0x14] = &Cpu::INC_8bit<RegD,0>;
  instructions[0x15] = &Cpu::DEC_8bit<RegD,0>;
  instructions[0x16] = &Cpu::LD<RegD,0,10>;
  instructions[0x17] = &Cpu::RLA;
  instructions[0x18] = &Cpu::JR;
  instructions[0x19] = &Cpu::ADD_16bit<RegHL,RegDE>;
  instructions[0x1A] = &Cpu::LDr$r<RegA,RegDE>;
  instructions[0x1B] = &Cpu::DEC_16bit<RegDE>;
  instructions[0x1C] = &Cpu::INC_8bit<RegE,0>;
  instructions[0x1D] = &Cpu::DEC_8bit<RegE,0>;
  instructions[0x1E] = &Cpu::LD<RegE,0,10>;
  instructions[0x1F] = &Cpu::RRA;
  
  instructions[0x20] = &Cpu::JR_cc<CpuFlagsZ,1>;
  instructions[0x21] = &Cpu::LDrd16<RegHL>;
  instructions[0x22] = &Cpu::LDI$regreg<RegHL,RegA>;
  instructions[0x23] = &Cpu::INC_16bit<RegHL>;
  instructions[0x24] = &Cpu::INC_8bit<RegH,0>;
  instructions[0x25] = &Cpu::DEC_8bit<RegH,0>;
  instructions[0x26] = &Cpu::LD<RegH,0,10>;
  instructions[0x27] = &Cpu::DAA;
  instructions[0x28] = &Cpu::JR_cc<CpuFlagsZ,0>;
  instructions[0x29] = &Cpu::ADD_16bit<RegHL,RegHL>;
  instructions[0x2A] = &Cpu::LDIreg$reg<RegA,RegHL>;
  instructions[0x2B] = &Cpu::DEC_16bit<RegHL>;
  instructions[0x2C] = &Cpu::INC_8bit<RegL,0>;
  instructions[0x2D] = &Cpu::DEC_8bit<RegL,0>;
  instructions[0x2E] = &Cpu::LD<RegL,0,10>;
  instructions[0x2F] = &Cpu::CPL;

  instructions[0x30] = &Cpu::JR_cc<CpuFlagsC,1>;
  instructions[0x31] = &Cpu::LDrd16<RegSP>;
  instructions[0x32] = &Cpu::LDD$regreg<RegHL,RegA>;
  instructions[0x33] = &Cpu::INC_16bit<RegSP>;
  instructions[0x34] = &Cpu::INC_8bit<RegHL,1>;
  instructions[0x35] = &Cpu::DEC_8bit<RegHL,1>;
  instructions[0x36] = &Cpu::LD<RegHL,0,11>;
  instructions[0x37] = &Cpu::SCF;
  instructions[0x38] = &Cpu::JR_cc<CpuFlagsC,0>;
  instructions[0x39] = &Cpu::ADD_16bit<RegHL,RegSP>;
  instructions[0x3A] = &Cpu::LDDreg$reg<RegA,RegHL>;
  instructions[0x3B] = &Cpu::DEC_16bit<RegSP>;
  instructions[0x3C] = &Cpu::INC_8bit<RegA,0>;
  instructions[0x3D] = &Cpu::DEC_8bit<RegA,0>;
  instructions[0x3E] = &Cpu::LD<RegA,0,10>;
  instructions[0x3F] = &Cpu::CCF;

  instructions[0x40] = &Cpu::LDrr<RegB,RegB>;
  instructions[0x41] = &Cpu::LDrr<RegB,RegC>;
  instructions[0x42] = &Cpu::LDrr<RegB,RegD>;
  instructions[0x43] = &Cpu::LDrr<RegB,RegE>;
  instructions[0x44] = &Cpu::LDrr<RegB,RegH>;
  instructions[0x45] = &Cpu::LDrr<RegB,RegL>;
  instructions[0x46] = &Cpu::LDr$r<RegB,RegHL>;
  instructions[0x47] = &Cpu::LDrr<RegB,RegA>;
  instructions[0x48] = &Cpu::LDrr<RegC,RegB>;
  instructions[0x49] = &Cpu::LDrr<RegC,RegC>;
  instructions[0x4A] = &Cpu::LDrr<RegC,RegD>;
  instructions[0x4B] = &Cpu::LDrr<RegC,RegE>;
  instructions[0x4C] = &Cpu::LDrr<RegC,RegH>;
  instructions[0x4D] = &Cpu::LDrr<RegC,RegL>;
  instructions[0x4E] = &Cpu::LDr$r<RegC,RegHL>;
  instructions[0x4F] = &Cpu::LDrr<RegC,RegA>;

  instructions[0x50] = &Cpu::LDrr<RegD,RegB>;
  instructions[0x51] = &Cpu::LDrr<RegD,RegC>;
  instructions[0x52] = &Cpu::LDrr<RegD,RegD>;
  instructions[0x53] = &Cpu::LDrr<RegD,RegE>;
  instructions[0x54] = &Cpu::LDrr<RegD,RegH>;
  instructions[0x55] = &Cpu::LDrr<RegD,RegL>;
  instructions[0x56] = &Cpu::LDr$r<RegD,RegHL>;
  instructions[0x57] = &Cpu::LDrr<RegD,RegA>;
  instructions[0x58] = &Cpu::LDrr<RegE,RegB>;
  instructions[0x59] = &Cpu::LDrr<RegE,RegC>;
  instructions[0x5A] = &Cpu::LDrr<RegE,RegD>;
  instructions[0x5B] = &Cpu::LDrr<RegE,RegE>;
  instructions[0x5C] = &Cpu::LDrr<RegE,RegH>;
  instructions[0x5D] = &Cpu::LDrr<RegE,RegL>;
  instructions[0x5E] = &Cpu::LDr$r<RegE,RegHL>;
  instructions[0x5F] = &Cpu::LDrr<RegE,RegA>;

  instructions[0x60] = &Cpu::LDrr<RegH,RegB>;
  instructions[0x61] = &Cpu::LDrr<RegH,RegC>;
  instructions[0x62] = &Cpu::LDrr<RegH,RegD>;
  instructions[0x63] = &Cpu::LDrr<RegH,RegE>;
  instructions[0x64] = &Cpu::LDrr<RegH,RegH>;
  instructions[0x65] = &Cpu::LDrr<RegH,RegL>;
  instructions[0x66] = &Cpu::LDr$r<RegH,RegHL>;
  instructions[0x67] = &Cpu::LDrr<RegH,RegA>;
  instructions[0x68] = &Cpu::LDrr<RegL,RegB>;
  instructions[0x69] = &Cpu::LDrr<RegL,RegC>;
  instructions[0x6A] = &Cpu::LDrr<RegL,RegD>;
  instructions[0x6B] = &Cpu::LDrr<RegL,RegE>;
  instructions[0x6C] = &Cpu::LDrr<RegL,RegH>;
  instructions[0x6D] = &Cpu::LDrr<RegL,RegL>;
  instructions[0x6E] = &Cpu::LDr$r<RegL,RegHL>;
  instructions[0x6F] = &Cpu::LDrr<RegL,RegA>;

  instructions[0x70] = &Cpu::LD$rr<RegHL,RegB>;
  instructions[0x71] = &Cpu::LD$rr<RegHL,RegC>;
  instructions[0x72] = &Cpu::LD$rr<RegHL,RegD>;
  instructions[0x73] = &Cpu::LD$rr<RegHL,RegE>;
  instructions[0x74] = &Cpu::LD$rr<RegHL,RegH>;
  instructions[0x75] = &Cpu::LD$rr<RegHL,RegL>;
  instructions[0x76] = &Cpu::HALT;
  instructions[0x77] = &Cpu::LD$rr<RegHL,RegA>;
  instructions[0x78] = &Cpu::LDrr<RegA,RegB>;
  instructions[0x79] = &Cpu::LDrr<RegA,RegC>;
  instructions[0x7A] = &Cpu::LDrr<RegA,RegD>;
  instructions[0x7B] = &Cpu::LDrr<RegA,RegE>;
  instructions[0x7C] = &Cpu::LDrr<RegA,RegH>;
  instructions[0x7D] = &Cpu::LDrr<RegA,RegL>;
  instructions[0x7E] = &Cpu::LDr$r<RegA,RegHL>;
  instructions[0x7F] = &Cpu::LDrr<RegA,RegA>;

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
  instructions[0xE2] = &Cpu::LD$FF00rr<RegC,RegA>;
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
  instructions[0xF2] = &Cpu::LDr$FF00r<RegA,RegC>;
  instructions[0xF3] = &Cpu::DI;
  instructions[0xF4] = &Cpu::ILLEGAL;
  instructions[0xF5] = &Cpu::PUSH<RegAF>;
  instructions[0xF6] = &Cpu::OR<RegA,0,2>;
  instructions[0xF7] = &Cpu::RST;
  instructions[0xF8] = &Cpu::LDHLSPr8;
  instructions[0xF9] = &Cpu::LDSPHL;
  instructions[0xFA] = &Cpu::LD<RegA,0,15>;
  instructions[0xFB] = &Cpu::EI;
  instructions[0xFC] = &Cpu::ILLEGAL;
  instructions[0xFD] = &Cpu::ILLEGAL;
  instructions[0xFE] = &Cpu::CP_d8;
  instructions[0xFF] = &Cpu::RST;
  //-checked above
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
  cpumode_ = CpuModeNormal;
  sprite_bug = 0;
}

void Cpu::Tick() {
  ++cycles;
  emu_->timer()->Tick();
  emu_->memory()->Tick();
  emu_->lcd_driver()->Step(dt);
  emu_->apu()->Step(dt);
}

void Cpu::Step(double dt) {
  this->dt = dt;
  cycles = 0;
  //reg.F._unused = 0;//always 0 according to docs
  //StopAt(0xC47A);
  //StopAt(0x0100);
  //StopAt(0x0073);
  if (cpumode_ == CpuModeStop) {
    cycles = 1;
    return;
  }
  if (cpumode_ == CpuModeNormal) {
    if (sprite_bug!=0) --sprite_bug;
    opcode_pc = reg.PC;
    opcode = emu_->memory()->Read8(reg.PC++);
    (this->*(instructions[opcode]))();
  } else if (cpumode_ == CpuModeHalt) {
    Tick();
  }
  emu_->timer()->Check();

  if (ime) {
    uint8_t test = emu_->memory()->interrupt_enable() & emu_->memory()->interrupt_flag();
    if (test) {
      ime = false;
      cpumode_ = CpuModeNormal;
      pushPC();
      if (test & 0x1) { //vblank
        reg.PC = 0x0040;
        emu_->memory()->interrupt_flag() &= ~0x1;
      } else if (test & 0x2) {
        reg.PC = 0x0048; //lcdc status
        emu_->memory()->interrupt_flag() &= ~0x2;
      } else if (test & 0x4) {
        reg.PC = 0x0050; //timer overflow
        emu_->memory()->interrupt_flag() &= ~0x4;
        //emu_->memory()->interrupt_enable() &= ~0x4;
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

template<uint8_t dest,uint8_t src>
void Cpu::LDrr() {
  reg.raw8[dest] = reg.raw8[src];
}

template<uint8_t dest,uint8_t src>
void Cpu::LD$rr() { //(dest), src
  mem_->Write8(reg.raw16[dest],reg.raw8[src]);
}

template<uint8_t dest,uint8_t src>
void Cpu::LDr$r() {
  reg.raw8[dest] = mem_->Read8(reg.raw16[src]);
}

template<uint8_t dest,uint8_t src>
void Cpu::LD$FF00rr() {
  mem_->Write8(0xFF00+reg.raw8[dest],reg.raw8[src]);
}

template<uint8_t dest,uint8_t src>
void Cpu::LDr$FF00r() {
  reg.raw8[dest] = mem_->Read8(0xFF00+reg.raw8[src]);
}

template<uint8_t dest>
void Cpu::LDrd16() {
  reg.raw16[dest] = mem_->Read8(reg.PC++);
  reg.raw16[dest] |= (mem_->Read8(reg.PC++))<<8;
  if (dest != RegAF && (reg.raw16[dest]>=0xFE00&&reg.raw16[dest]<=0xFEFF) && emu_->lcd_driver()->lcdc().lcd_enable == 1)
    sprite_bug = 2;
}

template<uint8_t dest,uint8_t src>
void Cpu::LDI$regreg() {
  mem_->Write8(reg.raw16[dest],reg.raw8[src]);
  ++reg.raw16[dest];
}

template<uint8_t dest,uint8_t src>
void Cpu::LDD$regreg() {
  mem_->Write8(reg.raw16[dest],reg.raw8[src]);
  --reg.raw16[dest];
}

template<uint8_t dest,uint8_t src>
void Cpu::LDIreg$reg() {
  simulateSpriteBug();
  reg.raw8[dest] = mem_->Read8(reg.raw16[src]);
  ++reg.raw16[src];
}

template<uint8_t dest,uint8_t src>
void Cpu::LDDreg$reg() {
  simulateSpriteBug();
  reg.raw8[dest] = mem_->Read8(reg.raw16[src]);
  --reg.raw16[src];
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::LD() {
  if (mode == 10) { //dest,d8
    reg.raw8[dest] = mem_->Read8(reg.PC++);
  }else if (mode == 11) { //(dest),d8
    auto d8 = mem_->Read8(reg.PC++);
    mem_->Write8(reg.raw16[dest],d8);
  } else if (mode == 12) { //0xFF00+d8 src
    uint8_t a8 = mem_->Read8(reg.PC++);
    mem_->Write8(0xFF00+a8,reg.raw8[src]);
  } else if (mode == 13) { //dest,0xFF00+d8 
    uint8_t a8 = mem_->Read8(reg.PC++);
    reg.raw8[dest] = mem_->Read8(0xFF00+a8);
  } else if (mode == 14) { //(d16),src
    uint16_t d16 = mem_->Read8(reg.PC++);
    d16 |= (mem_->Read8(reg.PC++))<<8;
    mem_->Write8(d16,reg.raw8[src]);
  } else if (mode == 15) { //src,(d16)
    uint16_t d16 = mem_->Read8(reg.PC++);
    d16 |= (mem_->Read8(reg.PC++))<<8;
    reg.raw8[dest] = mem_->Read8(d16);
  }
}

void Cpu::LDSPHL() {
  reg.SP = reg.HL;
  Tick();Tick();Tick();Tick();
}

void Cpu::LDHLSPr8() {



  reg.F.N  = reg.F.Z = 0;
  uint16_t a = reg.SP;
  int8_t r8 = mem_->Read8(reg.PC++);
  reg.HL = (reg.SP + r8);
  updateCpuFlagC(a&0xFF,r8,0);
  updateCpuFlagH(a&0xFF,r8,0);
  /*uint16_t r1 = (a&0xFFF) + (r8&0xFFF);
  reg.F.H = r1>0xFFF?1:0;
  uint32_t r2 = (a&0xFFFF) + (r8&0xFFFF);
  reg.F.C = r2>0xFFFF?1:0;*/
  Tick();Tick();Tick();Tick();
}

void Cpu::LDa16SP() {
  uint16_t a16 = mem_->Read8(reg.PC++);
  a16 |= (mem_->Read8(reg.PC++))<<8;
  mem_->Write8(a16,reg.SP&0xFF);
  mem_->Write8(a16+1,reg.SP>>8);
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

  updateCpuFlagC(a&0xFF,r8,0);
  updateCpuFlagH(a&0xFF,r8,0);
  Tick();Tick();Tick();Tick();
  Tick();Tick();Tick();Tick();
}

template<uint8_t dest,uint8_t src,int mode>
void Cpu::ADC() {
  reg.F.N = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  uint8_t carry = reg.F.C;
  reg.raw8[dest] = a + b + carry;

  updateCpuFlagC(a,carry,0);
  if (reg.F.C==0)
    updateCpuFlagC(a+carry,b,0);
  updateCpuFlagH(a,carry,0);
  if (reg.F.H==0)
    updateCpuFlagH(a+carry,b,0);
  updateCpuFlagZ(reg.raw8[dest]);

  if (opcode == 0x8E){
          Tick();Tick();Tick();Tick();
          Tick();Tick();Tick();Tick();
  }
  
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
  uint8_t carry = reg.F.C;
  reg.raw8[dest] = a - b - carry;
  updateCpuFlagC(a,carry,1);
  if (reg.F.C==0)
    updateCpuFlagC(a-carry,b,1);
  updateCpuFlagH(a,carry,1);
  if (reg.F.H==0)
    updateCpuFlagH(a-carry,b,1);
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

void Cpu::SCF() {
  reg.F.H = reg.F.N = 0;
  reg.F.C = 1;
}

void Cpu::CCF() {
  reg.F.H = reg.F.N = 0;
  reg.F.C = ~reg.F.C;
}

void Cpu::HALT() {
  cpumode_ = CpuModeHalt;
}

void Cpu::STOP() {
  cpumode_ = CpuModeStop;
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

   if ((code & 0xC0) == 0x40) { //bit
    uint8_t test = getr();
    uint8_t bitshift = (code&0x38) >> 3;
    reg.F.Z = (~(((test&(1<<bitshift))>>bitshift))&0x1);
    reg.F.H = 1;
    reg.F.N = 0;

  } else if ((code & 0xC0) == 0x80) { //res
    uint8_t test = getr();
    uint8_t bitshift = (code&0x38) >> 3;
    test &= ~(1<<bitshift);
    setr(test);
  } else if ((code & 0xC0) == 0xC0) { //set
    uint8_t test = getr();
    uint8_t bitshift = (code&0x38) >> 3;
    test |= (1<<bitshift);
    setr(test);
  } else  if ((code & 0xF8) == 0x30) { //swap
    uint8_t r = getr();
    uint8_t n0 = r&0xF;
    uint8_t n1 = (r&0xF0)>>4;
    r = n1 | (n0<<4);
    setr(r);
    reg.F.H  = reg.F.N = reg.F.C = 0;
    updateCpuFlagZ(r);
  } else if ((code&0xF8) == 0) { //RLC r
    uint8_t r = getr();
    reg.F.C = (r&0x80)!=0?1:0;
    r = r << 1;
    r |= reg.F.C;
    setr(r);
    reg.F.H  = reg.F.N = 0;
    updateCpuFlagZ(r);
  } else if ((code&0xF8) == 0x08) { //RRC r
    uint8_t r = getr();
    reg.F.C = r&1;
    r = r >> 1;
    r |= reg.F.C<<7;
    setr(r);
    reg.F.H  = reg.F.N = 0;
    updateCpuFlagZ(r);
  } else if ((code&0xF8) == 0x10) { //RL r
    uint8_t r = getr();
    uint8_t oldC = reg.F.C;
    reg.F.C = ((r&0x80)>>7);
    r = r << 1;
    r |= oldC;
    setr(r);
    reg.F.H  = reg.F.N = 0;
    updateCpuFlagZ(r);
  } else if ((code&0xF8) == 0x18) { //RR r
    uint8_t r = getr();
    uint8_t oldC = reg.F.C;
    reg.F.C = r&1;
    r = r >> 1;
    r |= oldC<<7;
    setr(r);
    reg.F.H  = reg.F.N = 0;
    updateCpuFlagZ(r);

  } else if ((code&0xF8) == 0x20) { //SLA r
    uint8_t r = getr();
    reg.F.C = ((r&0x80)>>7);
    r = r << 1;
    r &= ~0x01;
    setr(r);
    reg.F.H  = reg.F.N = 0;
    updateCpuFlagZ(r);
  } else if ((code&0xF8) == 0x28) { //SRA r
    uint8_t r = getr();
    reg.F.C = r&1;
    r = (r&0x80) + (r >> 1);
    setr(r);
    reg.F.H  = reg.F.N = 0;
    updateCpuFlagZ(r);
  } else if ((code & 0xF8) == 0x38) { //SRL n
    auto r = getr();
    reg.F.C = r&1;
    r >>= 1;
    setr(r);
    reg.F.H  = reg.F.N = 0;
    updateCpuFlagZ(r);
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
    updateCpuFlagZ(reg.raw8[dest]);
  } else {
    uint8_t data = mem_->Read8(reg.HL);
    updateCpuFlagH(data,1,0);
    ++data;
    mem_->Write8(reg.HL,data);
     updateCpuFlagZ(data);
  }
    
 
}

template<uint8_t dest>
void Cpu::INC_16bit() {
  simulateSpriteBug();
  ++reg.raw16[dest];
  Tick();Tick();Tick();Tick(); 
}

template<uint8_t dest,int mode>
void Cpu::DEC_8bit() {
  reg.F.N = 1;

  if (mode==0) {
    //updateCpuFlagH(reg.raw8[dest],1,1);
    --reg.raw8[dest];
    if ((reg.raw8[dest]&0xF)==0xF)
      reg.F.H = 1;
    else 
      reg.F.H = 0;
    updateCpuFlagZ(reg.raw8[dest]);
  } else {
    uint8_t data = mem_->Read8(reg.HL);

    //updateCpuFlagH(data,1,1);
    --data;
    if ((data&0xF)==0xF)
      reg.F.H = 1;
    else
      reg.F.H = 0;
    updateCpuFlagZ(data);
    mem_->Write8(reg.HL,data);

  }
  
}

template<uint8_t dest>
void Cpu::DEC_16bit() {
  simulateSpriteBug();
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
  reg.PC = reg.HL;
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
  simulateSpriteBug();
  push(reg.raw16[src]>>8);
  push(reg.raw16[src]&0xFF);
  Tick();Tick();Tick();Tick();
}

template<uint8_t dest>
void Cpu::POP() {
  simulateSpriteBug();
  reg.raw16[dest] = pop();
  reg.raw16[dest] |= pop() << 8;
  if (dest == RegAF)
    reg.F._unused = 0;
}

void Cpu::RLCA() {
  uint8_t& r = reg.A;
  reg.F.C = (r&0x80)>>7;
  r = r << 1;
  r |= reg.F.C;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
 // updateCpuFlagZ(r);
}

void Cpu::RRCA() {
  uint8_t& r = reg.A;
  reg.F.C = r&1;
  r = r >> 1;
  r |= reg.F.C<<7;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void Cpu::RLA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = (r&0x80)>>7;
  r = r << 1;
  r |= oldC;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void Cpu::RRA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = r & 1;
  r = r >> 1;
  r |= oldC << 7;
  reg.F.H = reg.F.N = 0;
  reg.F.Z = 0;
  //updateCpuFlagZ(r);
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

void Cpu::DAA() {
 int a = reg.A;

  if (!reg.F.N)
  {
      if (reg.F.H || (a & 0xF) > 9)
          a += 0x06;
      if (reg.F.C || a > 0x9F)
          a += 0x60;
  }
  else
  {
    if (reg.F.H)
      a = (a - 6) & 0xFF;
      if (reg.F.C)
          a -= 0x60;
  }


  if ((a & 0x100) == 0x100)
      reg.F.C = 1;

  a &= 0xFF;

  updateCpuFlagZ(a);
  reg.F.H = 0;
  reg.A = (uint8_t)a;
}

void Cpu::simulateSpriteBug() {
   
  if (sprite_bug == 1) {
    emu_->lcd_driver()->sprite_bug_counter = 80;//20cycles
    //auto oam = emu_->memory()->oam();
    //for (int i=8;i<0xA0;++i)
    //  oam[i] = rand()&0xFF;
    sprite_bug = 0;
  }
  
}

}
}