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
#include <map>
#include <vector>

/*


*/

#define StopAt(x) { if (x == opcode_pc) DebugBreak(); } 



namespace emulation {
namespace gb {


#define MachineCycle { Tick();Tick();Tick();Tick(); }

#define CMKey(a,b,c,d) (((a&0xFF)<<24)|((b&0xFF)<<16)|((c&0xFF)<<8)|(d&0xFF))

std::map<uint32_t,void*> compiledMap;




CpuRecompiler::CpuRecompiler() {
  instructions[0x00] = &CpuRecompiler::NOP;
  instructions[0x01] = &CpuRecompiler::LDrd16<RegBC>;
  instructions[0x02] = &CpuRecompiler::LD$rr<RegBC,RegA>;
  instructions[0x03] = &CpuRecompiler::INC_16bit<RegBC>;
  instructions[0x04] = &CpuRecompiler::INC_8bit<RegB,0>;
  instructions[0x05] = &CpuRecompiler::DEC_8bit<RegB,0>;
  instructions[0x06] = &CpuRecompiler::LD<RegB,0,10>;
  instructions[0x07] = &CpuRecompiler::RLCA;
  instructions[0x08] = &CpuRecompiler::LDa16SP;
  instructions[0x09] = &CpuRecompiler::ADD_16bit<RegHL,RegBC>;
  instructions[0x0A] = &CpuRecompiler::LDr$r<RegA,RegBC>;
  instructions[0x0B] = &CpuRecompiler::DEC_16bit<RegBC>;
  instructions[0x0C] = &CpuRecompiler::INC_8bit<RegC,0>;
  instructions[0x0D] = &CpuRecompiler::DEC_8bit<RegC,0>;
  instructions[0x0E] = &CpuRecompiler::LD<RegC,0,10>;
  instructions[0x0F] = &CpuRecompiler::RRCA;

  instructions[0x10] = &CpuRecompiler::STOP;
  instructions[0x11] = &CpuRecompiler::LDrd16<RegDE>;
  instructions[0x12] = &CpuRecompiler::LD$rr<RegDE,RegA>;
  instructions[0x13] = &CpuRecompiler::INC_16bit<RegDE>;
  instructions[0x14] = &CpuRecompiler::INC_8bit<RegD,0>;
  instructions[0x15] = &CpuRecompiler::DEC_8bit<RegD,0>;
  instructions[0x16] = &CpuRecompiler::LD<RegD,0,10>;
  instructions[0x17] = &CpuRecompiler::RLA;
  instructions[0x18] = &CpuRecompiler::JR;
  instructions[0x19] = &CpuRecompiler::ADD_16bit<RegHL,RegDE>;
  instructions[0x1A] = &CpuRecompiler::LDr$r<RegA,RegDE>;
  instructions[0x1B] = &CpuRecompiler::DEC_16bit<RegDE>;
  instructions[0x1C] = &CpuRecompiler::INC_8bit<RegE,0>;
  instructions[0x1D] = &CpuRecompiler::DEC_8bit<RegE,0>;
  instructions[0x1E] = &CpuRecompiler::LD<RegE,0,10>;
  instructions[0x1F] = &CpuRecompiler::RRA;
  
  instructions[0x20] = &CpuRecompiler::JR_cc<CpuFlagsZ,1>;
  instructions[0x21] = &CpuRecompiler::LDrd16<RegHL>;
  instructions[0x22] = &CpuRecompiler::LDI$regreg<RegHL,RegA>;
  instructions[0x23] = &CpuRecompiler::INC_16bit<RegHL>;
  instructions[0x24] = &CpuRecompiler::INC_8bit<RegH,0>;
  instructions[0x25] = &CpuRecompiler::DEC_8bit<RegH,0>;
  instructions[0x26] = &CpuRecompiler::LD<RegH,0,10>;
  instructions[0x27] = &CpuRecompiler::DAA;
  instructions[0x28] = &CpuRecompiler::JR_cc<CpuFlagsZ,0>;
  instructions[0x29] = &CpuRecompiler::ADD_16bit<RegHL,RegHL>;
  instructions[0x2A] = &CpuRecompiler::LDIreg$reg<RegA,RegHL>;
  instructions[0x2B] = &CpuRecompiler::DEC_16bit<RegHL>;
  instructions[0x2C] = &CpuRecompiler::INC_8bit<RegL,0>;
  instructions[0x2D] = &CpuRecompiler::DEC_8bit<RegL,0>;
  instructions[0x2E] = &CpuRecompiler::LD<RegL,0,10>;
  instructions[0x2F] = &CpuRecompiler::CPL;

  instructions[0x30] = &CpuRecompiler::JR_cc<CpuFlagsC,1>;
  instructions[0x31] = &CpuRecompiler::LDrd16<RegSP>;
  instructions[0x32] = &CpuRecompiler::LDD$regreg<RegHL,RegA>;
  instructions[0x33] = &CpuRecompiler::INC_16bit<RegSP>;
  instructions[0x34] = &CpuRecompiler::INC_8bit<RegHL,1>;
  instructions[0x35] = &CpuRecompiler::DEC_8bit<RegHL,1>;
  instructions[0x36] = &CpuRecompiler::LD<RegHL,0,11>;
  instructions[0x37] = &CpuRecompiler::SCF;
  instructions[0x38] = &CpuRecompiler::JR_cc<CpuFlagsC,0>;
  instructions[0x39] = &CpuRecompiler::ADD_16bit<RegHL,RegSP>;
  instructions[0x3A] = &CpuRecompiler::LDDreg$reg<RegA,RegHL>;
  instructions[0x3B] = &CpuRecompiler::DEC_16bit<RegSP>;
  instructions[0x3C] = &CpuRecompiler::INC_8bit<RegA,0>;
  instructions[0x3D] = &CpuRecompiler::DEC_8bit<RegA,0>;
  instructions[0x3E] = &CpuRecompiler::LD<RegA,0,10>;
  instructions[0x3F] = &CpuRecompiler::CCF;

  instructions[0x40] = &CpuRecompiler::LDrr<RegB,RegB>;
  instructions[0x41] = &CpuRecompiler::LDrr<RegB,RegC>;
  instructions[0x42] = &CpuRecompiler::LDrr<RegB,RegD>;
  instructions[0x43] = &CpuRecompiler::LDrr<RegB,RegE>;
  instructions[0x44] = &CpuRecompiler::LDrr<RegB,RegH>;
  instructions[0x45] = &CpuRecompiler::LDrr<RegB,RegL>;
  instructions[0x46] = &CpuRecompiler::LDr$r<RegB,RegHL>;
  instructions[0x47] = &CpuRecompiler::LDrr<RegB,RegA>;
  instructions[0x48] = &CpuRecompiler::LDrr<RegC,RegB>;
  instructions[0x49] = &CpuRecompiler::LDrr<RegC,RegC>;
  instructions[0x4A] = &CpuRecompiler::LDrr<RegC,RegD>;
  instructions[0x4B] = &CpuRecompiler::LDrr<RegC,RegE>;
  instructions[0x4C] = &CpuRecompiler::LDrr<RegC,RegH>;
  instructions[0x4D] = &CpuRecompiler::LDrr<RegC,RegL>;
  instructions[0x4E] = &CpuRecompiler::LDr$r<RegC,RegHL>;
  instructions[0x4F] = &CpuRecompiler::LDrr<RegC,RegA>;

  instructions[0x50] = &CpuRecompiler::LDrr<RegD,RegB>;
  instructions[0x51] = &CpuRecompiler::LDrr<RegD,RegC>;
  instructions[0x52] = &CpuRecompiler::LDrr<RegD,RegD>;
  instructions[0x53] = &CpuRecompiler::LDrr<RegD,RegE>;
  instructions[0x54] = &CpuRecompiler::LDrr<RegD,RegH>;
  instructions[0x55] = &CpuRecompiler::LDrr<RegD,RegL>;
  instructions[0x56] = &CpuRecompiler::LDr$r<RegD,RegHL>;
  instructions[0x57] = &CpuRecompiler::LDrr<RegD,RegA>;
  instructions[0x58] = &CpuRecompiler::LDrr<RegE,RegB>;
  instructions[0x59] = &CpuRecompiler::LDrr<RegE,RegC>;
  instructions[0x5A] = &CpuRecompiler::LDrr<RegE,RegD>;
  instructions[0x5B] = &CpuRecompiler::LDrr<RegE,RegE>;
  instructions[0x5C] = &CpuRecompiler::LDrr<RegE,RegH>;
  instructions[0x5D] = &CpuRecompiler::LDrr<RegE,RegL>;
  instructions[0x5E] = &CpuRecompiler::LDr$r<RegE,RegHL>;
  instructions[0x5F] = &CpuRecompiler::LDrr<RegE,RegA>;

  instructions[0x60] = &CpuRecompiler::LDrr<RegH,RegB>;
  instructions[0x61] = &CpuRecompiler::LDrr<RegH,RegC>;
  instructions[0x62] = &CpuRecompiler::LDrr<RegH,RegD>;
  instructions[0x63] = &CpuRecompiler::LDrr<RegH,RegE>;
  instructions[0x64] = &CpuRecompiler::LDrr<RegH,RegH>;
  instructions[0x65] = &CpuRecompiler::LDrr<RegH,RegL>;
  instructions[0x66] = &CpuRecompiler::LDr$r<RegH,RegHL>;
  instructions[0x67] = &CpuRecompiler::LDrr<RegH,RegA>;
  instructions[0x68] = &CpuRecompiler::LDrr<RegL,RegB>;
  instructions[0x69] = &CpuRecompiler::LDrr<RegL,RegC>;
  instructions[0x6A] = &CpuRecompiler::LDrr<RegL,RegD>;
  instructions[0x6B] = &CpuRecompiler::LDrr<RegL,RegE>;
  instructions[0x6C] = &CpuRecompiler::LDrr<RegL,RegH>;
  instructions[0x6D] = &CpuRecompiler::LDrr<RegL,RegL>;
  instructions[0x6E] = &CpuRecompiler::LDr$r<RegL,RegHL>;
  instructions[0x6F] = &CpuRecompiler::LDrr<RegL,RegA>;

  instructions[0x70] = &CpuRecompiler::LD$rr<RegHL,RegB>;
  instructions[0x71] = &CpuRecompiler::LD$rr<RegHL,RegC>;
  instructions[0x72] = &CpuRecompiler::LD$rr<RegHL,RegD>;
  instructions[0x73] = &CpuRecompiler::LD$rr<RegHL,RegE>;
  instructions[0x74] = &CpuRecompiler::LD$rr<RegHL,RegH>;
  instructions[0x75] = &CpuRecompiler::LD$rr<RegHL,RegL>;
  instructions[0x76] = &CpuRecompiler::HALT;
  instructions[0x77] = &CpuRecompiler::LD$rr<RegHL,RegA>;
  instructions[0x78] = &CpuRecompiler::LDrr<RegA,RegB>;
  instructions[0x79] = &CpuRecompiler::LDrr<RegA,RegC>;
  instructions[0x7A] = &CpuRecompiler::LDrr<RegA,RegD>;
  instructions[0x7B] = &CpuRecompiler::LDrr<RegA,RegE>;
  instructions[0x7C] = &CpuRecompiler::LDrr<RegA,RegH>;
  instructions[0x7D] = &CpuRecompiler::LDrr<RegA,RegL>;
  instructions[0x7E] = &CpuRecompiler::LDr$r<RegA,RegHL>;
  instructions[0x7F] = &CpuRecompiler::LDrr<RegA,RegA>;

  instructions[0x80] = &CpuRecompiler::ADD<RegA,RegB,0>;
  instructions[0x81] = &CpuRecompiler::ADD<RegA,RegC,0>;
  instructions[0x82] = &CpuRecompiler::ADD<RegA,RegD,0>;
  instructions[0x83] = &CpuRecompiler::ADD<RegA,RegE,0>;
  instructions[0x84] = &CpuRecompiler::ADD<RegA,RegH,0>;
  instructions[0x85] = &CpuRecompiler::ADD<RegA,RegL,0>;
  instructions[0x86] = &CpuRecompiler::ADD<RegA,RegHL,1>;
  instructions[0x87] = &CpuRecompiler::ADD<RegA,RegA,0>;
  instructions[0x88] = &CpuRecompiler::ADC<RegA,RegB,0>;
  instructions[0x89] = &CpuRecompiler::ADC<RegA,RegC,0>;
  instructions[0x8A] = &CpuRecompiler::ADC<RegA,RegD,0>;
  instructions[0x8B] = &CpuRecompiler::ADC<RegA,RegE,0>;
  instructions[0x8C] = &CpuRecompiler::ADC<RegA,RegH,0>;
  instructions[0x8D] = &CpuRecompiler::ADC<RegA,RegL,0>;
  instructions[0x8E] = &CpuRecompiler::ADC<RegA,RegHL,1>;
  instructions[0x8F] = &CpuRecompiler::ADC<RegA,RegA,0>;

  instructions[0x90] = &CpuRecompiler::SUB<RegA,RegB,0>;
  instructions[0x91] = &CpuRecompiler::SUB<RegA,RegC,0>;
  instructions[0x92] = &CpuRecompiler::SUB<RegA,RegD,0>;
  instructions[0x93] = &CpuRecompiler::SUB<RegA,RegE,0>;
  instructions[0x94] = &CpuRecompiler::SUB<RegA,RegH,0>;
  instructions[0x95] = &CpuRecompiler::SUB<RegA,RegL,0>;
  instructions[0x96] = &CpuRecompiler::SUB<RegA,RegHL,1>;
  instructions[0x97] = &CpuRecompiler::SUB<RegA,RegA,0>;
  instructions[0x98] = &CpuRecompiler::SBC<RegA,RegB,0>;
  instructions[0x99] = &CpuRecompiler::SBC<RegA,RegC,0>;
  instructions[0x9A] = &CpuRecompiler::SBC<RegA,RegD,0>;
  instructions[0x9B] = &CpuRecompiler::SBC<RegA,RegE,0>;
  instructions[0x9C] = &CpuRecompiler::SBC<RegA,RegH,0>;
  instructions[0x9D] = &CpuRecompiler::SBC<RegA,RegL,0>;
  instructions[0x9E] = &CpuRecompiler::SBC<RegA,RegHL,1>;
  instructions[0x9F] = &CpuRecompiler::SBC<RegA,RegA,0>;

  instructions[0xA0] = &CpuRecompiler::AND<RegA,RegB,0>;
  instructions[0xA1] = &CpuRecompiler::AND<RegA,RegC,0>;
  instructions[0xA2] = &CpuRecompiler::AND<RegA,RegD,0>;
  instructions[0xA3] = &CpuRecompiler::AND<RegA,RegE,0>;
  instructions[0xA4] = &CpuRecompiler::AND<RegA,RegH,0>;
  instructions[0xA5] = &CpuRecompiler::AND<RegA,RegL,0>;
  instructions[0xA6] = &CpuRecompiler::AND<RegA,RegHL,1>;
  instructions[0xA7] = &CpuRecompiler::AND<RegA,RegA,0>;
  instructions[0xA8] = &CpuRecompiler::XOR<RegA,RegB,0>;
  instructions[0xA9] = &CpuRecompiler::XOR<RegA,RegC,0>;
  instructions[0xAA] = &CpuRecompiler::XOR<RegA,RegD,0>;
  instructions[0xAB] = &CpuRecompiler::XOR<RegA,RegE,0>;
  instructions[0xAC] = &CpuRecompiler::XOR<RegA,RegH,0>;
  instructions[0xAD] = &CpuRecompiler::XOR<RegA,RegL,0>;
  instructions[0xAE] = &CpuRecompiler::XOR<RegA,RegHL,1>;
  instructions[0xAF] = &CpuRecompiler::XOR<RegA,RegA,0>;

  instructions[0xB0] = &CpuRecompiler::OR<RegA,RegB,0>;
  instructions[0xB1] = &CpuRecompiler::OR<RegA,RegC,0>;
  instructions[0xB2] = &CpuRecompiler::OR<RegA,RegD,0>;
  instructions[0xB3] = &CpuRecompiler::OR<RegA,RegE,0>;
  instructions[0xB4] = &CpuRecompiler::OR<RegA,RegH,0>;
  instructions[0xB5] = &CpuRecompiler::OR<RegA,RegL,0>;
  instructions[0xB6] = &CpuRecompiler::OR<RegA,RegHL,1>;
  instructions[0xB7] = &CpuRecompiler::OR<RegA,RegA,0>;
  instructions[0xB8] = &CpuRecompiler::CP_reg<RegB>;
  instructions[0xB9] = &CpuRecompiler::CP_reg<RegC>;
  instructions[0xBA] = &CpuRecompiler::CP_reg<RegD>;
  instructions[0xBB] = &CpuRecompiler::CP_reg<RegE>;
  instructions[0xBC] = &CpuRecompiler::CP_reg<RegH>;
  instructions[0xBD] = &CpuRecompiler::CP_reg<RegL>;
  instructions[0xBE] = &CpuRecompiler::CP_HL;
  instructions[0xBF] = &CpuRecompiler::CP_reg<RegA>;
  
  instructions[0xC0] = &CpuRecompiler::RET_cc<CpuFlagsZ,1>;
  instructions[0xC1] = &CpuRecompiler::POP<RegBC>;
  instructions[0xC2] = &CpuRecompiler::JP_cc<CpuFlagsZ,1>;
  instructions[0xC3] = &CpuRecompiler::JP;
  instructions[0xC4] = &CpuRecompiler::CALL_cc<CpuFlagsZ,1>;
  instructions[0xC5] = &CpuRecompiler::PUSH<RegBC>;
  instructions[0xC6] = &CpuRecompiler::ADD<RegA,0,2>;
  instructions[0xC7] = &CpuRecompiler::RST;
  instructions[0xC8] = &CpuRecompiler::RET_cc<CpuFlagsZ,0>;
  instructions[0xC9] = &CpuRecompiler::RET;
  instructions[0xCA] = &CpuRecompiler::JP_cc<CpuFlagsZ,0>;
  instructions[0xCB] = &CpuRecompiler::PREFIX_CB;
  instructions[0xCC] = &CpuRecompiler::CALL_cc<CpuFlagsZ,0>;
  instructions[0xCD] = &CpuRecompiler::CALL;
  instructions[0xCE] = &CpuRecompiler::ADC<RegA,0,2>;
  instructions[0xCF] = &CpuRecompiler::RST;

  instructions[0xD0] = &CpuRecompiler::RET_cc<CpuFlagsC,1>;
  instructions[0xD1] = &CpuRecompiler::POP<RegDE>;
  instructions[0xD2] = &CpuRecompiler::JP_cc<CpuFlagsC,1>;
  instructions[0xD3] = &CpuRecompiler::ILLEGAL;
  instructions[0xD4] = &CpuRecompiler::CALL_cc<CpuFlagsC,1>;
  instructions[0xD5] = &CpuRecompiler::PUSH<RegDE>;
  instructions[0xD6] = &CpuRecompiler::SUB<RegA,0,2>;
  instructions[0xD7] = &CpuRecompiler::RST;
  instructions[0xD8] = &CpuRecompiler::RET_cc<CpuFlagsC,0>;
  instructions[0xD9] = &CpuRecompiler::RETI;
  instructions[0xDA] = &CpuRecompiler::JP_cc<CpuFlagsC,0>;
  instructions[0xDB] = &CpuRecompiler::ILLEGAL;
  instructions[0xDC] = &CpuRecompiler::CALL_cc<CpuFlagsC,0>;
  instructions[0xDD] = &CpuRecompiler::ILLEGAL;
  instructions[0xDE] = &CpuRecompiler::SBC<RegA,0,2>;
  instructions[0xDF] = &CpuRecompiler::RST;
  
  instructions[0xE0] = &CpuRecompiler::LD<0,RegA,12>;
  instructions[0xE1] = &CpuRecompiler::POP<RegHL>;
  instructions[0xE2] = &CpuRecompiler::LD$FF00rr<RegC,RegA>;
  instructions[0xE3] = &CpuRecompiler::ILLEGAL;
  instructions[0xE4] = &CpuRecompiler::ILLEGAL;
  instructions[0xE5] = &CpuRecompiler::PUSH<RegHL>;
  instructions[0xE6] = &CpuRecompiler::AND<RegA,0,2>;
  instructions[0xE7] = &CpuRecompiler::RST;
  instructions[0xE8] = &CpuRecompiler::ADD_SPr8;
  instructions[0xE9] = &CpuRecompiler::JP_HL;
  instructions[0xEA] = &CpuRecompiler::LD<0,RegA,14>;
  instructions[0xEB] = &CpuRecompiler::ILLEGAL;
  instructions[0xEC] = &CpuRecompiler::ILLEGAL;
  instructions[0xED] = &CpuRecompiler::ILLEGAL;
  instructions[0xEE] = &CpuRecompiler::XOR<RegA,0,2>;
  instructions[0xEF] = &CpuRecompiler::RST;

  instructions[0xF0] = &CpuRecompiler::LD<RegA,0,13>;
  instructions[0xF1] = &CpuRecompiler::POP<RegAF>;
  instructions[0xF2] = &CpuRecompiler::LDr$FF00r<RegA,RegC>;
  instructions[0xF3] = &CpuRecompiler::DI;
  instructions[0xF4] = &CpuRecompiler::ILLEGAL;
  instructions[0xF5] = &CpuRecompiler::PUSH<RegAF>;
  instructions[0xF6] = &CpuRecompiler::OR<RegA,0,2>;
  instructions[0xF7] = &CpuRecompiler::RST;
  instructions[0xF8] = &CpuRecompiler::LDHLSPr8;
  instructions[0xF9] = &CpuRecompiler::LDSPHL;
  instructions[0xFA] = &CpuRecompiler::LD<RegA,0,15>;
  instructions[0xFB] = &CpuRecompiler::EI;
  instructions[0xFC] = &CpuRecompiler::ILLEGAL;
  instructions[0xFD] = &CpuRecompiler::ILLEGAL;
  instructions[0xFE] = &CpuRecompiler::CP_d8;
  instructions[0xFF] = &CpuRecompiler::RST;
  //-checked above
}

CpuRecompiler::~CpuRecompiler() {

}

void CpuRecompiler::Initialize(Emu* emu) {
  Component::Initialize(emu);
  block = e.create_block(1024*1024*8);
  e.set_block(block);
  mem_ = emu_->memory();
  Reset();
}

void CpuRecompiler::Deinitialize() {
  e.destroy_block(block);
}

void CpuRecompiler::Reset() {
  memset(&reg,0,sizeof(reg));
  reg.PC = 0;
  ime = false;
  cpumode_ = CpuModeNormal;
  sprite_bug = 0;
  block->cursor = 0;
  compiledMap.clear();
}



void CpuRecompiler::Step() {
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

void CpuRecompiler::NOP() { 
  //Tick();
  //Tick();
  //Tick();
  //Tick();
}

void CpuRecompiler::ILLEGAL() {
  int a = 1;
}

void CpuRecompiler::RST() {
  pushPC();
  uint8_t t = (opcode&0x38)>>3;
  reg.PC = t*8;
  emu_->MachineTick();
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LDrr() {
  reg.raw8[dest] = reg.raw8[src];
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LD$rr() { //(dest), src
  mem_->ClockedWrite8(reg.raw16[dest],reg.raw8[src]);
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LDr$r() {
  reg.raw8[dest] = mem_->ClockedRead8(reg.raw16[src]);
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LD$FF00rr() {
  mem_->ClockedWrite8(0xFF00+reg.raw8[dest],reg.raw8[src]);
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LDr$FF00r() {
  reg.raw8[dest] = mem_->ClockedRead8(0xFF00+reg.raw8[src]);
}

template<uint8_t dest>
void CpuRecompiler::LDrd16() {
  reg.raw16[dest] = mem_->ClockedRead8(reg.PC++);
  reg.raw16[dest] |= (mem_->ClockedRead8(reg.PC++))<<8;
  //if (dest != RegAF && (reg.raw16[dest]>=0xFDFF&&reg.raw16[dest]<=0xFEFF) && emu_->lcd_driver()->lcdc().lcd_enable == 1)
  //  sprite_bug = 2;
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LDI$regreg() {
  mem_->ClockedWrite8(reg.raw16[dest],reg.raw8[src]);
  ++reg.raw16[dest];
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LDD$regreg() {
  mem_->ClockedWrite8(reg.raw16[dest],reg.raw8[src]);
  --reg.raw16[dest];
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LDIreg$reg() {
  simulateSpriteBug(reg.raw16[src]);
  reg.raw8[dest] = mem_->ClockedRead8(reg.raw16[src]);
  ++reg.raw16[src];
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::LDDreg$reg() {
  simulateSpriteBug(reg.raw16[src]);
  reg.raw8[dest] = mem_->ClockedRead8(reg.raw16[src]);
  --reg.raw16[src];
}

template<uint8_t dest,uint8_t src,int mode>
void CpuRecompiler::LD() {
  if (mode == 10) { //dest,d8


    //reg.raw8[dest] = mem_->ClockedRead8(reg.PC++);


    if (compiledMap[CMKey(0x40,dest,src,mode)]==nullptr)  {
        compiledMap[CMKey(0x40,dest,src,mode)] = (block->ptr8bit+block->cursor);
        using namespace reccore::intel;
        IA32 ia32(&e);

        ia32.PUSH(EBP);
        ia32.MOV(EBP,EA(ESP));
        
        anycast<uint8_t (Memory::*)(uint16_t),void*> cast1;
        cast1.a =  &Memory::ClockedRead8;
        ia32.LEA(ECX,mem_);
        ia32.MOV(AX,EA("[disp32]",int32_t(&reg.PC)));
        ia32.PUSH(AX);
        ia32.INC(AX);
        ia32.MOV(EA("[disp32]",int32_t(&reg.PC)),AX);
        ia32.CALL(cast1.b);
        ia32.MOV(EA("[disp32]",int32_t(&reg.raw8[dest])),AL);

        ia32.MOV(ESP,EA(EBP));
        ia32.POP(EBP);
        ia32.RET();
    
 
      }
      void* ptr = compiledMap[CMKey(0x40,dest,src,mode)];
      ((void (*)())ptr)();

  } else if (mode == 11) { //(dest),d8


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

void CpuRecompiler::LDSPHL() {
  reg.SP = reg.HL;
  emu_->MachineTick();
}

void CpuRecompiler::LDHLSPr8() {



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

void CpuRecompiler::LDa16SP() {
  uint16_t a16 = mem_->ClockedRead8(reg.PC++);
  a16 |= (mem_->ClockedRead8(reg.PC++))<<8;
  mem_->ClockedWrite8(a16,reg.SP&0xFF);
  mem_->ClockedWrite8(a16+1,reg.SP>>8);
}

template<uint8_t dest,uint8_t src,int mode>
void CpuRecompiler::ADD() {
  reg.F.N = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a + b;
  updateCpuFlagC(a,b,0);
  updateCpuFlagH(a,b,0);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src>
void CpuRecompiler::ADD_16bit() {
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

void CpuRecompiler::ADD_SPr8() {
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
void CpuRecompiler::ADC() {
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
void CpuRecompiler::SUB() {
  reg.F.N = 1;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a - b;
  updateCpuFlagC(a,b,1);
  updateCpuFlagH(a,b,1);
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void CpuRecompiler::SBC() {
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
void CpuRecompiler::AND() {


  uint32_t key = (0x70<<24|(dest<<16)|(src<<8)|(mode&0xFF));

  if (compiledMap[CMKey(0x70,dest,src,mode)]==nullptr)
  {
    compiledMap[CMKey(0x70,dest,src,mode)] = (block->ptr8bit+block->cursor);

    using namespace reccore::intel;
    IA32 ia32(&e);

    ia32.PUSH(EBP);
    ia32.MOV(EBP,EA(ESP));

    ia32.MOV(AL,0);
    ia32.MOV(EA("[disp32]",int32_t(&reg.F.raw)),AL);
    
    arithmeticModeREC<dest,src,mode>();

    ia32.AND(AL,EA(DL));
    ia32.MOV(EA("[disp32]",int32_t(&reg.raw8[dest])),AL);

    ia32.MOV(AH,0x20);
    ia32.MOV(EA("[disp32]",int32_t(&reg.F.raw)),AH);

    updateCpuFlagZREC(&reg.raw8[dest]);

    ia32.MOV(ESP,EA(EBP));
    ia32.POP(EBP);
    ia32.RET();
    //e.ExecuteBlock(block);
    
 
  }
  void* ptr = compiledMap[CMKey(0x70,dest,src,mode)];
  ((void (*)())ptr)();
/*
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a & b;
  reg.F.H = 1;
  updateCpuFlagZ(reg.raw8[dest]);
*/

}


template<uint8_t dest,uint8_t src,int mode>
void CpuRecompiler::XOR() {
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a ^ b;
  updateCpuFlagZ(reg.raw8[dest]);
}

template<uint8_t dest,uint8_t src,int mode>
void CpuRecompiler::OR() {
  reg.F.raw = 0;
  uint8_t a=0,b=0;
  arithmeticMode<dest,src,mode>(a,b);
  reg.raw8[dest] = a | b;
  updateCpuFlagZ(reg.raw8[dest]);
}

void CpuRecompiler::SCF() {
  reg.F.H = reg.F.N = 0;
  reg.F.C = 1;
}

void CpuRecompiler::CCF() {
  reg.F.H = reg.F.N = 0;
  reg.F.C = ~reg.F.C;
}

void CpuRecompiler::HALT() {
  cpumode_ = CpuModeHalt;
}

void CpuRecompiler::STOP() {
  cpumode_ = CpuModeStop;
  if (mem_->ioports()[0x4D] & 0x1) {
    emu_->speed = ((mem_->ioports()[0x4D])>>7)+1;
    if (emu_->speed == 2)
      emu_->set_base_freq_hz(default_gb_hz*2);
    else
      emu_->set_base_freq_hz(default_gb_hz);
#ifdef _DEBUG    
    {
      char str[25];
      sprintf(str,"CPU Speed Change:%d\n",emu_->speed);
      OutputDebugString(str);
    }
#endif
    cpumode_ = CpuModeNormal;
  }
}

void CpuRecompiler::CPL() {
  reg.F.H = 1;
  reg.F.N = 1;
  reg.A = ~reg.A;
}

const CpuRegisterNames8 reg_index[8] = {
    RegB,RegC,RegD,RegE,RegH,RegL,RegH,RegA,
};

void CpuRecompiler::PREFIX_CB() {
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

void CpuRecompiler::JR() {
   int8_t disp8 = mem_->ClockedRead8(reg.PC++);
   reg.PC += disp8;
   emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuRecompiler::JR_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    JR();
  } else {
    int8_t disp8 = mem_->ClockedRead8(reg.PC++);
  }

}

template<uint8_t dest,int mode>
void CpuRecompiler::INC_8bit() {
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
void CpuRecompiler::INC_16bit() {
  if (dest != RegAF)
    simulateSpriteBug(reg.raw16[dest]);
  ++reg.raw16[dest];
  emu_->MachineTick();
}

template<uint8_t dest,int mode>
void CpuRecompiler::DEC_8bit() {
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
void CpuRecompiler::DEC_16bit() {
  if (dest != RegAF)
    simulateSpriteBug(reg.raw16[dest]);
  --reg.raw16[dest];
  emu_->MachineTick();
}

void CpuRecompiler::JP() {
  uint16_t nn;
  nn = mem_->ClockedRead8(reg.PC++);
  nn |= (mem_->ClockedRead8(reg.PC++))<<8;
  reg.PC = nn;

  emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuRecompiler::JP_cc() {
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

void CpuRecompiler::JP_HL() {
  reg.PC = reg.HL;
}


void CpuRecompiler::CALL() {
  uint16_t nn;
  nn = mem_->ClockedRead8(reg.PC++);
  nn |= (mem_->ClockedRead8(reg.PC++))<<8;
  pushPC();
  reg.PC = nn;

  emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuRecompiler::CALL_cc() {
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

void CpuRecompiler::RET() {
  reg.PC = pop();
  reg.PC |= pop() << 8;
  emu_->MachineTick();
}

template<CpuFlags condbit,bool inv>
void CpuRecompiler::RET_cc() {
  int table[2] = {((reg.F.raw & (1<<condbit))>>condbit),
    ~((reg.F.raw & (1<<condbit))>>condbit)};

  if (table[inv]&1) {
    RET();
  } else {
   
  }
  emu_->MachineTick();
}


template<uint8_t src>
void CpuRecompiler::PUSH() {

  simulateSpriteBug(reg.SP+1);
  push(reg.raw16[src]>>8);
  push(reg.raw16[src]&0xFF);
  emu_->MachineTick();
}

template<uint8_t dest>
void CpuRecompiler::POP() {

  simulateSpriteBug(reg.SP+1);
  reg.raw16[dest] = pop();
  reg.raw16[dest] |= pop() << 8;
  if (dest == RegAF)
    reg.F._unused = 0;


}

void CpuRecompiler::RLCA() {
  uint8_t& r = reg.A;
  reg.F.C = (r&0x80)>>7;
  r = r << 1;
  r |= reg.F.C;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
 // updateCpuFlagZ(r);
}

void CpuRecompiler::RRCA() {
  uint8_t& r = reg.A;
  reg.F.C = r&1;
  r = r >> 1;
  r |= reg.F.C<<7;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void CpuRecompiler::RLA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = (r&0x80)>>7;
  r = r << 1;
  r |= oldC;
  reg.F.H = reg.F.N = 0;
    reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void CpuRecompiler::RRA() {
  uint8_t& r = reg.A;;
  uint8_t oldC = reg.F.C;
  reg.F.C = r & 1;
  r = r >> 1;
  r |= oldC << 7;
  reg.F.H = reg.F.N = 0;
  reg.F.Z = 0;
  //updateCpuFlagZ(r);
}

void CpuRecompiler::CP(uint8_t a, uint8_t b) {
  reg.F.N = 1;
  updateCpuFlagC(a,b,1);
  updateCpuFlagH(a,b,1);
  updateCpuFlagZ(a-b);
}

template<CpuRegisterNames8 r>
void CpuRecompiler::CP_reg() {
  CP(reg.A,reg.raw8[r]);
}

void CpuRecompiler::CP_d8() {
  CP(reg.A,mem_->ClockedRead8(reg.PC++));
}

void CpuRecompiler::CP_HL() {
  CP(reg.A,mem_->ClockedRead8(reg.HL));
}

void CpuRecompiler::DI() {
  ime = false;
}

void CpuRecompiler::EI() {
  ime = true;
}


void CpuRecompiler::RETI() {
  EI();
  RET();
}

void CpuRecompiler::DAA() {
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

void CpuRecompiler::simulateSpriteBug(uint16_t value) {
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