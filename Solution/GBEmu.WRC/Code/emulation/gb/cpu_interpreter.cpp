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

#define StopAt(x) { if (x == opcode_pc) DebugBreak(); } 

namespace emulation {
namespace gb {


#define MachineCycle { Tick();Tick();Tick();Tick(); }

CpuInterpreter::CpuInterpreter() {
  instructions[0x00] = &CpuInterpreter::NOP;
  instructions[0x01] = &CpuInterpreter::LDrd16<RegBC>;
  instructions[0x02] = &CpuInterpreter::LD$rr<RegBC,RegA>;
  instructions[0x03] = &CpuInterpreter::INC_16bit<RegBC>;
  instructions[0x04] = &CpuInterpreter::INC_8bit<RegB,0>;
  instructions[0x05] = &CpuInterpreter::DEC_8bit<RegB,0>;
  instructions[0x06] = &CpuInterpreter::LD<RegB,0,10>;
  instructions[0x07] = &CpuInterpreter::RLCA;
  instructions[0x08] = &CpuInterpreter::LDa16SP;
  instructions[0x09] = &CpuInterpreter::ADD_16bit<RegHL,RegBC>;
  instructions[0x0A] = &CpuInterpreter::LDr$r<RegA,RegBC>;
  instructions[0x0B] = &CpuInterpreter::DEC_16bit<RegBC>;
  instructions[0x0C] = &CpuInterpreter::INC_8bit<RegC,0>;
  instructions[0x0D] = &CpuInterpreter::DEC_8bit<RegC,0>;
  instructions[0x0E] = &CpuInterpreter::LD<RegC,0,10>;
  instructions[0x0F] = &CpuInterpreter::RRCA;

  instructions[0x10] = &CpuInterpreter::STOP;
  instructions[0x11] = &CpuInterpreter::LDrd16<RegDE>;
  instructions[0x12] = &CpuInterpreter::LD$rr<RegDE,RegA>;
  instructions[0x13] = &CpuInterpreter::INC_16bit<RegDE>;
  instructions[0x14] = &CpuInterpreter::INC_8bit<RegD,0>;
  instructions[0x15] = &CpuInterpreter::DEC_8bit<RegD,0>;
  instructions[0x16] = &CpuInterpreter::LD<RegD,0,10>;
  instructions[0x17] = &CpuInterpreter::RLA;
  instructions[0x18] = &CpuInterpreter::JR;
  instructions[0x19] = &CpuInterpreter::ADD_16bit<RegHL,RegDE>;
  instructions[0x1A] = &CpuInterpreter::LDr$r<RegA,RegDE>;
  instructions[0x1B] = &CpuInterpreter::DEC_16bit<RegDE>;
  instructions[0x1C] = &CpuInterpreter::INC_8bit<RegE,0>;
  instructions[0x1D] = &CpuInterpreter::DEC_8bit<RegE,0>;
  instructions[0x1E] = &CpuInterpreter::LD<RegE,0,10>;
  instructions[0x1F] = &CpuInterpreter::RRA;
  
  instructions[0x20] = &CpuInterpreter::JR_cc<CpuFlagsZ,1>;
  instructions[0x21] = &CpuInterpreter::LDrd16<RegHL>;
  instructions[0x22] = &CpuInterpreter::LDI$regreg<RegHL,RegA>;
  instructions[0x23] = &CpuInterpreter::INC_16bit<RegHL>;
  instructions[0x24] = &CpuInterpreter::INC_8bit<RegH,0>;
  instructions[0x25] = &CpuInterpreter::DEC_8bit<RegH,0>;
  instructions[0x26] = &CpuInterpreter::LD<RegH,0,10>;
  instructions[0x27] = &CpuInterpreter::DAA;
  instructions[0x28] = &CpuInterpreter::JR_cc<CpuFlagsZ,0>;
  instructions[0x29] = &CpuInterpreter::ADD_16bit<RegHL,RegHL>;
  instructions[0x2A] = &CpuInterpreter::LDIreg$reg<RegA,RegHL>;
  instructions[0x2B] = &CpuInterpreter::DEC_16bit<RegHL>;
  instructions[0x2C] = &CpuInterpreter::INC_8bit<RegL,0>;
  instructions[0x2D] = &CpuInterpreter::DEC_8bit<RegL,0>;
  instructions[0x2E] = &CpuInterpreter::LD<RegL,0,10>;
  instructions[0x2F] = &CpuInterpreter::CPL;

  instructions[0x30] = &CpuInterpreter::JR_cc<CpuFlagsC,1>;
  instructions[0x31] = &CpuInterpreter::LDrd16<RegSP>;
  instructions[0x32] = &CpuInterpreter::LDD$regreg<RegHL,RegA>;
  instructions[0x33] = &CpuInterpreter::INC_16bit<RegSP>;
  instructions[0x34] = &CpuInterpreter::INC_8bit<RegHL,1>;
  instructions[0x35] = &CpuInterpreter::DEC_8bit<RegHL,1>;
  instructions[0x36] = &CpuInterpreter::LD<RegHL,0,11>;
  instructions[0x37] = &CpuInterpreter::SCF;
  instructions[0x38] = &CpuInterpreter::JR_cc<CpuFlagsC,0>;
  instructions[0x39] = &CpuInterpreter::ADD_16bit<RegHL,RegSP>;
  instructions[0x3A] = &CpuInterpreter::LDDreg$reg<RegA,RegHL>;
  instructions[0x3B] = &CpuInterpreter::DEC_16bit<RegSP>;
  instructions[0x3C] = &CpuInterpreter::INC_8bit<RegA,0>;
  instructions[0x3D] = &CpuInterpreter::DEC_8bit<RegA,0>;
  instructions[0x3E] = &CpuInterpreter::LD<RegA,0,10>;
  instructions[0x3F] = &CpuInterpreter::CCF;

  instructions[0x40] = &CpuInterpreter::LDrr<RegB,RegB>;
  instructions[0x41] = &CpuInterpreter::LDrr<RegB,RegC>;
  instructions[0x42] = &CpuInterpreter::LDrr<RegB,RegD>;
  instructions[0x43] = &CpuInterpreter::LDrr<RegB,RegE>;
  instructions[0x44] = &CpuInterpreter::LDrr<RegB,RegH>;
  instructions[0x45] = &CpuInterpreter::LDrr<RegB,RegL>;
  instructions[0x46] = &CpuInterpreter::LDr$r<RegB,RegHL>;
  instructions[0x47] = &CpuInterpreter::LDrr<RegB,RegA>;
  instructions[0x48] = &CpuInterpreter::LDrr<RegC,RegB>;
  instructions[0x49] = &CpuInterpreter::LDrr<RegC,RegC>;
  instructions[0x4A] = &CpuInterpreter::LDrr<RegC,RegD>;
  instructions[0x4B] = &CpuInterpreter::LDrr<RegC,RegE>;
  instructions[0x4C] = &CpuInterpreter::LDrr<RegC,RegH>;
  instructions[0x4D] = &CpuInterpreter::LDrr<RegC,RegL>;
  instructions[0x4E] = &CpuInterpreter::LDr$r<RegC,RegHL>;
  instructions[0x4F] = &CpuInterpreter::LDrr<RegC,RegA>;

  instructions[0x50] = &CpuInterpreter::LDrr<RegD,RegB>;
  instructions[0x51] = &CpuInterpreter::LDrr<RegD,RegC>;
  instructions[0x52] = &CpuInterpreter::LDrr<RegD,RegD>;
  instructions[0x53] = &CpuInterpreter::LDrr<RegD,RegE>;
  instructions[0x54] = &CpuInterpreter::LDrr<RegD,RegH>;
  instructions[0x55] = &CpuInterpreter::LDrr<RegD,RegL>;
  instructions[0x56] = &CpuInterpreter::LDr$r<RegD,RegHL>;
  instructions[0x57] = &CpuInterpreter::LDrr<RegD,RegA>;
  instructions[0x58] = &CpuInterpreter::LDrr<RegE,RegB>;
  instructions[0x59] = &CpuInterpreter::LDrr<RegE,RegC>;
  instructions[0x5A] = &CpuInterpreter::LDrr<RegE,RegD>;
  instructions[0x5B] = &CpuInterpreter::LDrr<RegE,RegE>;
  instructions[0x5C] = &CpuInterpreter::LDrr<RegE,RegH>;
  instructions[0x5D] = &CpuInterpreter::LDrr<RegE,RegL>;
  instructions[0x5E] = &CpuInterpreter::LDr$r<RegE,RegHL>;
  instructions[0x5F] = &CpuInterpreter::LDrr<RegE,RegA>;

  instructions[0x60] = &CpuInterpreter::LDrr<RegH,RegB>;
  instructions[0x61] = &CpuInterpreter::LDrr<RegH,RegC>;
  instructions[0x62] = &CpuInterpreter::LDrr<RegH,RegD>;
  instructions[0x63] = &CpuInterpreter::LDrr<RegH,RegE>;
  instructions[0x64] = &CpuInterpreter::LDrr<RegH,RegH>;
  instructions[0x65] = &CpuInterpreter::LDrr<RegH,RegL>;
  instructions[0x66] = &CpuInterpreter::LDr$r<RegH,RegHL>;
  instructions[0x67] = &CpuInterpreter::LDrr<RegH,RegA>;
  instructions[0x68] = &CpuInterpreter::LDrr<RegL,RegB>;
  instructions[0x69] = &CpuInterpreter::LDrr<RegL,RegC>;
  instructions[0x6A] = &CpuInterpreter::LDrr<RegL,RegD>;
  instructions[0x6B] = &CpuInterpreter::LDrr<RegL,RegE>;
  instructions[0x6C] = &CpuInterpreter::LDrr<RegL,RegH>;
  instructions[0x6D] = &CpuInterpreter::LDrr<RegL,RegL>;
  instructions[0x6E] = &CpuInterpreter::LDr$r<RegL,RegHL>;
  instructions[0x6F] = &CpuInterpreter::LDrr<RegL,RegA>;

  instructions[0x70] = &CpuInterpreter::LD$rr<RegHL,RegB>;
  instructions[0x71] = &CpuInterpreter::LD$rr<RegHL,RegC>;
  instructions[0x72] = &CpuInterpreter::LD$rr<RegHL,RegD>;
  instructions[0x73] = &CpuInterpreter::LD$rr<RegHL,RegE>;
  instructions[0x74] = &CpuInterpreter::LD$rr<RegHL,RegH>;
  instructions[0x75] = &CpuInterpreter::LD$rr<RegHL,RegL>;
  instructions[0x76] = &CpuInterpreter::HALT;
  instructions[0x77] = &CpuInterpreter::LD$rr<RegHL,RegA>;
  instructions[0x78] = &CpuInterpreter::LDrr<RegA,RegB>;
  instructions[0x79] = &CpuInterpreter::LDrr<RegA,RegC>;
  instructions[0x7A] = &CpuInterpreter::LDrr<RegA,RegD>;
  instructions[0x7B] = &CpuInterpreter::LDrr<RegA,RegE>;
  instructions[0x7C] = &CpuInterpreter::LDrr<RegA,RegH>;
  instructions[0x7D] = &CpuInterpreter::LDrr<RegA,RegL>;
  instructions[0x7E] = &CpuInterpreter::LDr$r<RegA,RegHL>;
  instructions[0x7F] = &CpuInterpreter::LDrr<RegA,RegA>;

  instructions[0x80] = &CpuInterpreter::ADD<RegA,RegB,0>;
  instructions[0x81] = &CpuInterpreter::ADD<RegA,RegC,0>;
  instructions[0x82] = &CpuInterpreter::ADD<RegA,RegD,0>;
  instructions[0x83] = &CpuInterpreter::ADD<RegA,RegE,0>;
  instructions[0x84] = &CpuInterpreter::ADD<RegA,RegH,0>;
  instructions[0x85] = &CpuInterpreter::ADD<RegA,RegL,0>;
  instructions[0x86] = &CpuInterpreter::ADD<RegA,RegHL,1>;
  instructions[0x87] = &CpuInterpreter::ADD<RegA,RegA,0>;
  instructions[0x88] = &CpuInterpreter::ADC<RegA,RegB,0>;
  instructions[0x89] = &CpuInterpreter::ADC<RegA,RegC,0>;
  instructions[0x8A] = &CpuInterpreter::ADC<RegA,RegD,0>;
  instructions[0x8B] = &CpuInterpreter::ADC<RegA,RegE,0>;
  instructions[0x8C] = &CpuInterpreter::ADC<RegA,RegH,0>;
  instructions[0x8D] = &CpuInterpreter::ADC<RegA,RegL,0>;
  instructions[0x8E] = &CpuInterpreter::ADC<RegA,RegHL,1>;
  instructions[0x8F] = &CpuInterpreter::ADC<RegA,RegA,0>;

  instructions[0x90] = &CpuInterpreter::SUB<RegA,RegB,0>;
  instructions[0x91] = &CpuInterpreter::SUB<RegA,RegC,0>;
  instructions[0x92] = &CpuInterpreter::SUB<RegA,RegD,0>;
  instructions[0x93] = &CpuInterpreter::SUB<RegA,RegE,0>;
  instructions[0x94] = &CpuInterpreter::SUB<RegA,RegH,0>;
  instructions[0x95] = &CpuInterpreter::SUB<RegA,RegL,0>;
  instructions[0x96] = &CpuInterpreter::SUB<RegA,RegHL,1>;
  instructions[0x97] = &CpuInterpreter::SUB<RegA,RegA,0>;
  instructions[0x98] = &CpuInterpreter::SBC<RegA,RegB,0>;
  instructions[0x99] = &CpuInterpreter::SBC<RegA,RegC,0>;
  instructions[0x9A] = &CpuInterpreter::SBC<RegA,RegD,0>;
  instructions[0x9B] = &CpuInterpreter::SBC<RegA,RegE,0>;
  instructions[0x9C] = &CpuInterpreter::SBC<RegA,RegH,0>;
  instructions[0x9D] = &CpuInterpreter::SBC<RegA,RegL,0>;
  instructions[0x9E] = &CpuInterpreter::SBC<RegA,RegHL,1>;
  instructions[0x9F] = &CpuInterpreter::SBC<RegA,RegA,0>;

  instructions[0xA0] = &CpuInterpreter::AND<RegA,RegB,0>;
  instructions[0xA1] = &CpuInterpreter::AND<RegA,RegC,0>;
  instructions[0xA2] = &CpuInterpreter::AND<RegA,RegD,0>;
  instructions[0xA3] = &CpuInterpreter::AND<RegA,RegE,0>;
  instructions[0xA4] = &CpuInterpreter::AND<RegA,RegH,0>;
  instructions[0xA5] = &CpuInterpreter::AND<RegA,RegL,0>;
  instructions[0xA6] = &CpuInterpreter::AND<RegA,RegHL,1>;
  instructions[0xA7] = &CpuInterpreter::AND<RegA,RegA,0>;
  instructions[0xA8] = &CpuInterpreter::XOR<RegA,RegB,0>;
  instructions[0xA9] = &CpuInterpreter::XOR<RegA,RegC,0>;
  instructions[0xAA] = &CpuInterpreter::XOR<RegA,RegD,0>;
  instructions[0xAB] = &CpuInterpreter::XOR<RegA,RegE,0>;
  instructions[0xAC] = &CpuInterpreter::XOR<RegA,RegH,0>;
  instructions[0xAD] = &CpuInterpreter::XOR<RegA,RegL,0>;
  instructions[0xAE] = &CpuInterpreter::XOR<RegA,RegHL,1>;
  instructions[0xAF] = &CpuInterpreter::XOR<RegA,RegA,0>;

  instructions[0xB0] = &CpuInterpreter::OR<RegA,RegB,0>;
  instructions[0xB1] = &CpuInterpreter::OR<RegA,RegC,0>;
  instructions[0xB2] = &CpuInterpreter::OR<RegA,RegD,0>;
  instructions[0xB3] = &CpuInterpreter::OR<RegA,RegE,0>;
  instructions[0xB4] = &CpuInterpreter::OR<RegA,RegH,0>;
  instructions[0xB5] = &CpuInterpreter::OR<RegA,RegL,0>;
  instructions[0xB6] = &CpuInterpreter::OR<RegA,RegHL,1>;
  instructions[0xB7] = &CpuInterpreter::OR<RegA,RegA,0>;
  instructions[0xB8] = &CpuInterpreter::CP_reg<RegB>;
  instructions[0xB9] = &CpuInterpreter::CP_reg<RegC>;
  instructions[0xBA] = &CpuInterpreter::CP_reg<RegD>;
  instructions[0xBB] = &CpuInterpreter::CP_reg<RegE>;
  instructions[0xBC] = &CpuInterpreter::CP_reg<RegH>;
  instructions[0xBD] = &CpuInterpreter::CP_reg<RegL>;
  instructions[0xBE] = &CpuInterpreter::CP_HL;
  instructions[0xBF] = &CpuInterpreter::CP_reg<RegA>;
  
  instructions[0xC0] = &CpuInterpreter::RET_cc<CpuFlagsZ,1>;
  instructions[0xC1] = &CpuInterpreter::POP<RegBC>;
  instructions[0xC2] = &CpuInterpreter::JP_cc<CpuFlagsZ,1>;
  instructions[0xC3] = &CpuInterpreter::JP;
  instructions[0xC4] = &CpuInterpreter::CALL_cc<CpuFlagsZ,1>;
  instructions[0xC5] = &CpuInterpreter::PUSH<RegBC>;
  instructions[0xC6] = &CpuInterpreter::ADD<RegA,0,2>;
  instructions[0xC7] = &CpuInterpreter::RST;
  instructions[0xC8] = &CpuInterpreter::RET_cc<CpuFlagsZ,0>;
  instructions[0xC9] = &CpuInterpreter::RET;
  instructions[0xCA] = &CpuInterpreter::JP_cc<CpuFlagsZ,0>;
  instructions[0xCB] = &CpuInterpreter::PREFIX_CB;
  instructions[0xCC] = &CpuInterpreter::CALL_cc<CpuFlagsZ,0>;
  instructions[0xCD] = &CpuInterpreter::CALL;
  instructions[0xCE] = &CpuInterpreter::ADC<RegA,0,2>;
  instructions[0xCF] = &CpuInterpreter::RST;

  instructions[0xD0] = &CpuInterpreter::RET_cc<CpuFlagsC,1>;
  instructions[0xD1] = &CpuInterpreter::POP<RegDE>;
  instructions[0xD2] = &CpuInterpreter::JP_cc<CpuFlagsC,1>;
  instructions[0xD3] = &CpuInterpreter::ILLEGAL;
  instructions[0xD4] = &CpuInterpreter::CALL_cc<CpuFlagsC,1>;
  instructions[0xD5] = &CpuInterpreter::PUSH<RegDE>;
  instructions[0xD6] = &CpuInterpreter::SUB<RegA,0,2>;
  instructions[0xD7] = &CpuInterpreter::RST;
  instructions[0xD8] = &CpuInterpreter::RET_cc<CpuFlagsC,0>;
  instructions[0xD9] = &CpuInterpreter::RETI;
  instructions[0xDA] = &CpuInterpreter::JP_cc<CpuFlagsC,0>;
  instructions[0xDB] = &CpuInterpreter::ILLEGAL;
  instructions[0xDC] = &CpuInterpreter::CALL_cc<CpuFlagsC,0>;
  instructions[0xDD] = &CpuInterpreter::ILLEGAL;
  instructions[0xDE] = &CpuInterpreter::SBC<RegA,0,2>;
  instructions[0xDF] = &CpuInterpreter::RST;
  
  instructions[0xE0] = &CpuInterpreter::LD<0,RegA,12>;
  instructions[0xE1] = &CpuInterpreter::POP<RegHL>;
  instructions[0xE2] = &CpuInterpreter::LD$FF00rr<RegC,RegA>;
  instructions[0xE3] = &CpuInterpreter::ILLEGAL;
  instructions[0xE4] = &CpuInterpreter::ILLEGAL;
  instructions[0xE5] = &CpuInterpreter::PUSH<RegHL>;
  instructions[0xE6] = &CpuInterpreter::AND<RegA,0,2>;
  instructions[0xE7] = &CpuInterpreter::RST;
  instructions[0xE8] = &CpuInterpreter::ADD_SPr8;
  instructions[0xE9] = &CpuInterpreter::JP_HL;
  instructions[0xEA] = &CpuInterpreter::LD<0,RegA,14>;
  instructions[0xEB] = &CpuInterpreter::ILLEGAL;
  instructions[0xEC] = &CpuInterpreter::ILLEGAL;
  instructions[0xED] = &CpuInterpreter::ILLEGAL;
  instructions[0xEE] = &CpuInterpreter::XOR<RegA,0,2>;
  instructions[0xEF] = &CpuInterpreter::RST;

  instructions[0xF0] = &CpuInterpreter::LD<RegA,0,13>;
  instructions[0xF1] = &CpuInterpreter::POP<RegAF>;
  instructions[0xF2] = &CpuInterpreter::LDr$FF00r<RegA,RegC>;
  instructions[0xF3] = &CpuInterpreter::DI;
  instructions[0xF4] = &CpuInterpreter::ILLEGAL;
  instructions[0xF5] = &CpuInterpreter::PUSH<RegAF>;
  instructions[0xF6] = &CpuInterpreter::OR<RegA,0,2>;
  instructions[0xF7] = &CpuInterpreter::RST;
  instructions[0xF8] = &CpuInterpreter::LDHLSPr8;
  instructions[0xF9] = &CpuInterpreter::LDSPHL;
  instructions[0xFA] = &CpuInterpreter::LD<RegA,0,15>;
  instructions[0xFB] = &CpuInterpreter::EI;
  instructions[0xFC] = &CpuInterpreter::ILLEGAL;
  instructions[0xFD] = &CpuInterpreter::ILLEGAL;
  instructions[0xFE] = &CpuInterpreter::CP_d8;
  instructions[0xFF] = &CpuInterpreter::RST;
  //-checked above
}

CpuInterpreter::~CpuInterpreter() {

}

void CpuInterpreter::Initialize(Emu* emu) {
  Component::Initialize(emu);
  mem_ = emu_->memory();
  Reset();
}

void CpuInterpreter::Deinitialize() {

}

void CpuInterpreter::Reset() {
  memset(&reg,0,sizeof(reg));
  reg.PC = 0;
  ime = false;
  cpumode_ = CpuModeNormal;
  sprite_bug = 0;
}



void CpuInterpreter::Step() {
  this->dt = dt;
  //reg.F._unused = 0;//always 0 according to docs
  //StopAt(0x0100);
  //StopAt(0x0073);
  if (cpumode_ == CpuModeStop) {
    emu_->cpu_cycles_per_step_ = 1;
    return;
  }
  if (cpumode_ == CpuModeNormal) {
    if (sprite_bug!=0) --sprite_bug;
    opcode_pc = reg.PC;
    //if (opcode_pc == 0x100) {
      //if (emu_->mode() == EmuModeGBC) //hack
      //  reg.A = 0x11;
    //}
    opcode = mem_->ClockedRead8(reg.PC++);
    (this->*(instructions[opcode]))();
  } else if (cpumode_ == CpuModeHalt) {
    emu_->ClockTick();
  }

  if (ime) {
    uint8_t test = mem_->interrupt_enable() & mem_->interrupt_flag();
    if (test) {
      ime = false;
      cpumode_ = CpuModeNormal;
      pushPC();

      if (test & 0x1) { //vblank
        reg.PC = 0x0040;
        mem_->interrupt_flag() &= ~0x1;
      } else if (test & 0x2) {
        reg.PC = 0x0048; //lcdc status
        mem_->interrupt_flag() &= ~0x2;
        //emu_->lcd_driver()->int48signal = 0;
      } else if (test & 0x4) {
        reg.PC = 0x0050; //timer overflow
        mem_->interrupt_flag() &= ~0x4;
        //mem_->interrupt_enable() &= ~0x4;
      } else if (test & 0x8) {
        reg.PC = 0x0058; //serial transfer
        mem_->interrupt_flag() &= ~0x8;
      } else if (test & 0x10) {
        reg.PC = 0x0060; //hi-lo p10-p13
        mem_->interrupt_flag() &= ~0x10;
      }

    }
  }
}

void CpuInterpreter::NOP() { 
  //Tick();
  //Tick();
  //Tick();
  //Tick();
}

void CpuInterpreter::ILLEGAL() {
  int a = 1;
}

void CpuInterpreter::RST() {
  pushPC();
  uint8_t t = (opcode&0x38)>>3;
  reg.PC = t*8;
  emu_->MachineTick();
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LDrr() {
  reg.raw8[dest] = reg.raw8[src];
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LD$rr() { //(dest), src
  mem_->ClockedWrite8(reg.raw16[dest],reg.raw8[src]);
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LDr$r() {
  reg.raw8[dest] = mem_->ClockedRead8(reg.raw16[src]);
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LD$FF00rr() {
  mem_->ClockedWrite8(0xFF00+reg.raw8[dest],reg.raw8[src]);
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LDr$FF00r() {
  reg.raw8[dest] = mem_->ClockedRead8(0xFF00+reg.raw8[src]);
}

template<uint8_t dest>
void CpuInterpreter::LDrd16() {
  reg.raw16[dest] = mem_->ClockedRead8(reg.PC++);
  reg.raw16[dest] |= (mem_->ClockedRead8(reg.PC++))<<8;
  //if (dest != RegAF && (reg.raw16[dest]>=0xFDFF&&reg.raw16[dest]<=0xFEFF) && emu_->lcd_driver()->lcdc().lcd_enable == 1)
  //  sprite_bug = 2;
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LDI$regreg() {
  mem_->ClockedWrite8(reg.raw16[dest],reg.raw8[src]);
  ++reg.raw16[dest];
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LDD$regreg() {
  mem_->ClockedWrite8(reg.raw16[dest],reg.raw8[src]);
  --reg.raw16[dest];
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LDIreg$reg() {
  simulateSpriteBug(reg.raw16[src]);
  reg.raw8[dest] = mem_->ClockedRead8(reg.raw16[src]);
  ++reg.raw16[src];
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::LDDreg$reg() {
  simulateSpriteBug(reg.raw16[src]);
  reg.raw8[dest] = mem_->ClockedRead8(reg.raw16[src]);
  --reg.raw16[src];
}

template<uint8_t dest,uint8_t src,int mode>
void CpuInterpreter::LD() {
  if (mode == 10) { //dest,d8
    reg.raw8[dest] = mem_->ClockedRead8(reg.PC++);
  }else if (mode == 11) { //(dest),d8
    auto d8 = mem_->ClockedRead8(reg.PC++);
    mem_->ClockedWrite8(reg.raw16[dest],d8);
  } else if (mode == 12) { //0xFF00+d8 src
    uint8_t a8 = mem_->ClockedRead8(reg.PC++);
    mem_->ClockedWrite8(0xFF00+a8,reg.raw8[src]);
  } else if (mode == 13) { //dest,0xFF00+d8 
    uint8_t a8 = mem_->ClockedRead8(reg.PC++);
    reg.raw8[dest] = mem_->ClockedRead8(0xFF00+a8);
  } else if (mode == 14) { //(d16),src
    uint16_t d16 = mem_->ClockedRead8(reg.PC++);
    d16 |= (mem_->ClockedRead8(reg.PC++))<<8;
    mem_->ClockedWrite8(d16,reg.raw8[src]);
  } else if (mode == 15) { //src,(d16)
    uint16_t d16 = mem_->ClockedRead8(reg.PC++);
    d16 |= (mem_->ClockedRead8(reg.PC++))<<8;
    reg.raw8[dest] = mem_->ClockedRead8(d16);
  }
}

void CpuInterpreter::LDSPHL() {
  reg.SP = reg.HL;
  emu_->MachineTick();
}

void CpuInterpreter::LDHLSPr8() {



  reg.F.N  = reg.F.Z = 0;
  uint16_t a = reg.SP;
  int8_t r8 = mem_->ClockedRead8(reg.PC++);
  reg.HL = (reg.SP + r8);
  updateCpuFlagC(a&0xFF,r8,0);
  updateCpuFlagH(a&0xFF,r8,0);
  /*uint16_t r1 = (a&0xFFF) + (r8&0xFFF);
  reg.F.H = r1>0xFFF?1:0;
  uint32_t r2 = (a&0xFFFF) + (r8&0xFFFF);
  reg.F.C = r2>0xFFFF?1:0;*/
  emu_->MachineTick();
}

void CpuInterpreter::LDa16SP() {
  uint16_t a16 = mem_->ClockedRead8(reg.PC++);
  a16 |= (mem_->ClockedRead8(reg.PC++))<<8;
  mem_->ClockedWrite8(a16,reg.SP&0xFF);
  mem_->ClockedWrite8(a16+1,reg.SP>>8);
}

template<uint8_t dest,uint8_t src,int mode>
void CpuInterpreter::ADD() {
  reg.F.N = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a + b;
  updateCpuFlagC(a,b,0);
  updateCpuFlagH(a,b,0);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src>
void CpuInterpreter::ADD_16bit() {
  reg.F.N = 0;
  uint16_t a=0,b=0;
  a = reg.raw16[dest];
  b = reg.raw16[src];
  reg.raw16[dest] = a + b;
  uint16_t r1 = (a&0xFFF) + (b&0xFFF);
  reg.F.H = r1>0xFFF?1:0;
  uint32_t r2 = (a&0xFFFF) + (b&0xFFFF);
  reg.F.C = r2>0xFFFF?1:0;
  emu_->MachineTick();
}

void CpuInterpreter::ADD_SPr8() {
  reg.F.N  = reg.F.Z = 0;
  uint16_t a = reg.SP;
  int8_t r8 = mem_->ClockedRead8(reg.PC++);
  reg.SP += r8;

  updateCpuFlagC(a&0xFF,r8,0);
  updateCpuFlagH(a&0xFF,r8,0);
  emu_->MachineTick();
  emu_->MachineTick();
}

template<uint8_t dest,uint8_t src,int mode>
void CpuInterpreter::ADC() {
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
    int a = 1;
          //Tick();Tick();Tick();Tick();
          //Tick();Tick();Tick();Tick();
  }
  
}



template<uint8_t dest,uint8_t src,int mode>
void CpuInterpreter::SUB() {
  reg.F.N = 1;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a - b;
  updateCpuFlagC(a,b,1);
  updateCpuFlagH(a,b,1);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void CpuInterpreter::SBC() {
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
void CpuInterpreter::AND() {


  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a & b;
  reg.F.H = 1;
  updateCpuFlagZ(reg.raw8[dest]);


}


template<uint8_t dest,uint8_t src,int mode>
void CpuInterpreter::XOR() {
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a ^ b;
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void CpuInterpreter::OR() {
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a | b;
  updateCpuFlagZ(reg.raw8[dest]);
}

void CpuInterpreter::SCF() {
  reg.F.H = reg.F.N = 0;
  reg.F.C = 1;
}

void CpuInterpreter::CCF() {
  reg.F.H = reg.F.N = 0;
  reg.F.C = ~reg.F.C;
}

void CpuInterpreter::HALT() {
  cpumode_ = CpuModeHalt;
}

void CpuInterpreter::STOP() {
  cpumode_ = CpuModeStop;
  if (mem_->ioports()[0x4D] & 0x1) {
    emu_->speed = ((mem_->ioports()[0x4D])>>7)+1;
    if (emu_->speed == 2)
      emu_->set_base_freq_hz(default_gb_hz*2);
    else
      emu_->set_base_freq_hz(default_gb_hz);
#ifdef _DEBUG    
    {
      WCHAR str[25];
      wprintf_s(str,"CPU Speed Change:%d\n",emu_->speed);
      OutputDebugString(str);
    }
#endif
    cpumode_ = CpuModeNormal;
  }
}

void CpuInterpreter::CPL() {
  reg.F.H = 1;
  reg.F.N = 1;
  reg.A = ~reg.A;
}

const CpuRegisterNames8 reg_index[8] = {
    RegB,RegC,RegD,RegE,RegH,RegL,RegH,RegA,
};

void CpuInterpreter::PREFIX_CB() {
  uint8_t code = mem_->ClockedRead8(reg.PC++);

  auto getr = [=]() {
    if ((code&0x7) != 6) {
      return reg.raw8[reg_index[code&0x7]];
    } else {

      return mem_->ClockedRead8(reg.HL);
    }
  };

  auto setr = [=](uint8_t r) {
    if ((code&0x7) != 6) {
      reg.raw8[reg_index[code&0x7]] = r;
    } else {
      mem_->ClockedWrite8(reg.HL,r);
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
  //Tick();Tick();Tick();Tick();
}

void CpuInterpreter::JR() {
   int8_t disp8 = mem_->ClockedRead8(reg.PC++);
   reg.PC += disp8;
   emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuInterpreter::JR_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    JR();
  } else {
    int8_t disp8 = mem_->ClockedRead8(reg.PC++);
  }

}

template<uint8_t dest,int mode>
void CpuInterpreter::INC_8bit() {
  reg.F.N = 0;

  if (mode==0) {
    updateCpuFlagH(reg.raw8[dest],1,0);
    ++reg.raw8[dest];
    updateCpuFlagZ(reg.raw8[dest]);
  } else {
    uint8_t data = mem_->ClockedRead8(reg.HL);
    updateCpuFlagH(data,1,0);
    ++data;
    mem_->ClockedWrite8(reg.HL,data);
     updateCpuFlagZ(data);
  }
    
 
}

template<uint8_t dest>
void CpuInterpreter::INC_16bit() {
  if (dest != RegAF)
    simulateSpriteBug(reg.raw16[dest]);
  ++reg.raw16[dest];
  emu_->MachineTick();
}

template<uint8_t dest,int mode>
void CpuInterpreter::DEC_8bit() {
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
    uint8_t data = mem_->ClockedRead8(reg.HL);

    //updateCpuFlagH(data,1,1);
    --data;
    if ((data&0xF)==0xF)
      reg.F.H = 1;
    else
      reg.F.H = 0;
    updateCpuFlagZ(data);
    mem_->ClockedWrite8(reg.HL,data);

  }
  
}

template<uint8_t dest>
void CpuInterpreter::DEC_16bit() {
  if (dest != RegAF)
    simulateSpriteBug(reg.raw16[dest]);
  --reg.raw16[dest];
  emu_->MachineTick();
}

void CpuInterpreter::JP() {
  uint16_t nn;
  nn = mem_->ClockedRead8(reg.PC++);
  nn |= (mem_->ClockedRead8(reg.PC++))<<8;
  reg.PC = nn;

  emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuInterpreter::JP_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    JP();
  } else {
    uint16_t nn;
    nn = mem_->ClockedRead8(reg.PC++);
    nn |= (mem_->ClockedRead8(reg.PC++))<<8;
  }
}

void CpuInterpreter::JP_HL() {
  reg.PC = reg.HL;
}


void CpuInterpreter::CALL() {
  uint16_t nn;
  nn = mem_->ClockedRead8(reg.PC++);
  nn |= (mem_->ClockedRead8(reg.PC++))<<8;
  pushPC();
  reg.PC = nn;

  emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuInterpreter::CALL_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    CALL();
  } else {
    uint16_t nn;
    nn = mem_->ClockedRead8(reg.PC++);
    nn |= (mem_->ClockedRead8(reg.PC++))<<8;
  }
}

void CpuInterpreter::RET() {
  reg.PC = pop();
  reg.PC |= pop() << 8;
  emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuInterpreter::RET_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    RET();
  } else {
   
  }
  emu_->MachineTick();
}


template<uint8_t src>
void CpuInterpreter::PUSH() {

  simulateSpriteBug(reg.SP+1);
  push(reg.raw16[src]>>8);
  push(reg.raw16[src]&0xFF);
  emu_->MachineTick();
}

template<uint8_t dest>
void CpuInterpreter::POP() {

  simulateSpriteBug(reg.SP+1);
  reg.raw16[dest] = pop();
  reg.raw16[dest] |= pop() << 8;
  if (dest == RegAF)
    reg.F._unused = 0;


}

void CpuInterpreter::RLCA() {
  uint8_t& r = reg.A;
  reg.F.C = (r&0x80)>>7;
  r = r << 1;
  r |= reg.F.C;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
 // updateCpuFlagZ(r);
}

void CpuInterpreter::RRCA() {
  uint8_t& r = reg.A;
  reg.F.C = r&1;
  r = r >> 1;
  r |= reg.F.C<<7;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void CpuInterpreter::RLA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = (r&0x80)>>7;
  r = r << 1;
  r |= oldC;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void CpuInterpreter::RRA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = r & 1;
  r = r >> 1;
  r |= oldC << 7;
  reg.F.H = reg.F.N = 0;
  reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void CpuInterpreter::CP(uint8_t a, uint8_t b) {
  reg.F.N = 1;
  updateCpuFlagC(a,b,1);
  updateCpuFlagH(a,b,1);
  updateCpuFlagZ(a-b);
}

template<CpuRegisterNames8 r>
void CpuInterpreter::CP_reg() {
  CP(reg.A,reg.raw8[r]);
}

void CpuInterpreter::CP_d8() {
  CP(reg.A,mem_->ClockedRead8(reg.PC++));
}

void CpuInterpreter::CP_HL() {
  CP(reg.A,mem_->ClockedRead8(reg.HL));
}

void CpuInterpreter::DI() {
  ime = false;
}

void CpuInterpreter::EI() {
  ime = true;
}


void CpuInterpreter::RETI() {
  EI();
  RET();
}

void CpuInterpreter::DAA() {
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

void CpuInterpreter::simulateSpriteBug(uint16_t value) {
  auto driver = emu_->lcd_driver();
  auto lcdc = driver->lcdc();
  auto mode = driver->stat().mode;
  if ((value>=0xFE00&&value<=0xFEFF) && lcdc.lcd_enable == 1 && mode != 1 && driver->scanline_counter() <= 72) {

    driver->sprite_bug_counter = 80;
    //auto oam = mem_->oam();
    //for (int i=8;i<0xA0;++i)
    //  oam[i] = rand()&0xFF;
    sprite_bug = 0;



  }
  
}

}
}