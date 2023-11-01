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

CpuInterpreter::CpuInterpreter() : 
  instructions{ 
    &CpuInterpreter::NOP,               &CpuInterpreter::LDrd16<RegBC>,         &CpuInterpreter::LD$rr<RegBC,RegA>, &CpuInterpreter::INC_16bit<RegBC>,
    &CpuInterpreter::INC_8bit<RegB,0>,  &CpuInterpreter::DEC_8bit<RegB,0>,      &CpuInterpreter::LD<RegB,0,10>,     &CpuInterpreter::RLCA,
    &CpuInterpreter::LDa16SP,           &CpuInterpreter::ADD_16bit<RegHL,RegBC>,&CpuInterpreter::LDr$r<RegA,RegBC>, &CpuInterpreter::DEC_16bit<RegBC>,
    &CpuInterpreter::INC_8bit<RegC,0>,  &CpuInterpreter::DEC_8bit<RegC,0>,      &CpuInterpreter::LD<RegC,0,10>,     &CpuInterpreter::RRCA,

&CpuInterpreter::STOP,
&CpuInterpreter::LDrd16<RegDE>,
&CpuInterpreter::LD$rr<RegDE,RegA>,
&CpuInterpreter::INC_16bit<RegDE>,
&CpuInterpreter::INC_8bit<RegD,0>,
&CpuInterpreter::DEC_8bit<RegD,0>,
&CpuInterpreter::LD<RegD,0,10>,
&CpuInterpreter::RLA,
&CpuInterpreter::JR,
&CpuInterpreter::ADD_16bit<RegHL,RegDE>,
&CpuInterpreter::LDr$r<RegA,RegDE>,
&CpuInterpreter::DEC_16bit<RegDE>,
&CpuInterpreter::INC_8bit<RegE,0>,
&CpuInterpreter::DEC_8bit<RegE,0>,
&CpuInterpreter::LD<RegE,0,10>,
&CpuInterpreter::RRA,

&CpuInterpreter::JR_cc<CpuFlagsZ,1>,
&CpuInterpreter::LDrd16<RegHL>,
&CpuInterpreter::LDI$regreg<RegHL,RegA>,
&CpuInterpreter::INC_16bit<RegHL>,
&CpuInterpreter::INC_8bit<RegH,0>,
&CpuInterpreter::DEC_8bit<RegH,0>,
&CpuInterpreter::LD<RegH,0,10>,
&CpuInterpreter::DAA,
&CpuInterpreter::JR_cc<CpuFlagsZ,0>,
&CpuInterpreter::ADD_16bit<RegHL,RegHL>,
&CpuInterpreter::LDIreg$reg<RegA,RegHL>,
&CpuInterpreter::DEC_16bit<RegHL>,
&CpuInterpreter::INC_8bit<RegL,0>,
&CpuInterpreter::DEC_8bit<RegL,0>,
&CpuInterpreter::LD<RegL,0,10>,
&CpuInterpreter::CPL,

&CpuInterpreter::JR_cc<CpuFlagsC,1>,
&CpuInterpreter::LDrd16<RegSP>,
&CpuInterpreter::LDD$regreg<RegHL,RegA>,
&CpuInterpreter::INC_16bit<RegSP>,
&CpuInterpreter::INC_8bit<RegHL,1>,
&CpuInterpreter::DEC_8bit<RegHL,1>,
&CpuInterpreter::LD<RegHL,0,11>,
&CpuInterpreter::SCF,
&CpuInterpreter::JR_cc<CpuFlagsC,0>,
&CpuInterpreter::ADD_16bit<RegHL,RegSP>,
&CpuInterpreter::LDDreg$reg<RegA,RegHL>,
&CpuInterpreter::DEC_16bit<RegSP>,
&CpuInterpreter::INC_8bit<RegA,0>,
&CpuInterpreter::DEC_8bit<RegA,0>,
&CpuInterpreter::LD<RegA,0,10>,
&CpuInterpreter::CCF,

&CpuInterpreter::LDrr<RegB,RegB>,
&CpuInterpreter::LDrr<RegB,RegC>,
&CpuInterpreter::LDrr<RegB,RegD>,
&CpuInterpreter::LDrr<RegB,RegE>,
&CpuInterpreter::LDrr<RegB,RegH>,
&CpuInterpreter::LDrr<RegB,RegL>,
&CpuInterpreter::LDr$r<RegB,RegHL>,
&CpuInterpreter::LDrr<RegB,RegA>,
&CpuInterpreter::LDrr<RegC,RegB>,
&CpuInterpreter::LDrr<RegC,RegC>,
&CpuInterpreter::LDrr<RegC,RegD>,
&CpuInterpreter::LDrr<RegC,RegE>,
&CpuInterpreter::LDrr<RegC,RegH>,
&CpuInterpreter::LDrr<RegC,RegL>,
&CpuInterpreter::LDr$r<RegC,RegHL>,
&CpuInterpreter::LDrr<RegC,RegA>,

&CpuInterpreter::LDrr<RegD,RegB>,
&CpuInterpreter::LDrr<RegD,RegC>,
&CpuInterpreter::LDrr<RegD,RegD>,
&CpuInterpreter::LDrr<RegD,RegE>,
&CpuInterpreter::LDrr<RegD,RegH>,
&CpuInterpreter::LDrr<RegD,RegL>,
&CpuInterpreter::LDr$r<RegD,RegHL>,
&CpuInterpreter::LDrr<RegD,RegA>,
&CpuInterpreter::LDrr<RegE,RegB>,
&CpuInterpreter::LDrr<RegE,RegC>,
&CpuInterpreter::LDrr<RegE,RegD>,
&CpuInterpreter::LDrr<RegE,RegE>,
&CpuInterpreter::LDrr<RegE,RegH>,
&CpuInterpreter::LDrr<RegE,RegL>,
&CpuInterpreter::LDr$r<RegE,RegHL>,
&CpuInterpreter::LDrr<RegE,RegA>,

&CpuInterpreter::LDrr<RegH,RegB>,
&CpuInterpreter::LDrr<RegH,RegC>,
&CpuInterpreter::LDrr<RegH,RegD>,
&CpuInterpreter::LDrr<RegH,RegE>,
&CpuInterpreter::LDrr<RegH,RegH>,
&CpuInterpreter::LDrr<RegH,RegL>,
&CpuInterpreter::LDr$r<RegH,RegHL>,
&CpuInterpreter::LDrr<RegH,RegA>,
&CpuInterpreter::LDrr<RegL,RegB>,
&CpuInterpreter::LDrr<RegL,RegC>,
&CpuInterpreter::LDrr<RegL,RegD>,
&CpuInterpreter::LDrr<RegL,RegE>,
&CpuInterpreter::LDrr<RegL,RegH>,
&CpuInterpreter::LDrr<RegL,RegL>,
&CpuInterpreter::LDr$r<RegL,RegHL>,
&CpuInterpreter::LDrr<RegL,RegA>,

&CpuInterpreter::LD$rr<RegHL,RegB>,
&CpuInterpreter::LD$rr<RegHL,RegC>,
&CpuInterpreter::LD$rr<RegHL,RegD>,
&CpuInterpreter::LD$rr<RegHL,RegE>,
&CpuInterpreter::LD$rr<RegHL,RegH>,
&CpuInterpreter::LD$rr<RegHL,RegL>,
&CpuInterpreter::HALT,
&CpuInterpreter::LD$rr<RegHL,RegA>,
&CpuInterpreter::LDrr<RegA,RegB>,
&CpuInterpreter::LDrr<RegA,RegC>,
&CpuInterpreter::LDrr<RegA,RegD>,
&CpuInterpreter::LDrr<RegA,RegE>,
&CpuInterpreter::LDrr<RegA,RegH>,
&CpuInterpreter::LDrr<RegA,RegL>,
&CpuInterpreter::LDr$r<RegA,RegHL>,
&CpuInterpreter::LDrr<RegA,RegA>,

&CpuInterpreter::ADD<RegA,RegB,0>,
&CpuInterpreter::ADD<RegA,RegC,0>,
&CpuInterpreter::ADD<RegA,RegD,0>,
&CpuInterpreter::ADD<RegA,RegE,0>,
&CpuInterpreter::ADD<RegA,RegH,0>,
&CpuInterpreter::ADD<RegA,RegL,0>,
&CpuInterpreter::ADD<RegA,RegHL,1>,
&CpuInterpreter::ADD<RegA,RegA,0>,
&CpuInterpreter::ADC<RegA,RegB,0>,
&CpuInterpreter::ADC<RegA,RegC,0>,
&CpuInterpreter::ADC<RegA,RegD,0>,
&CpuInterpreter::ADC<RegA,RegE,0>,
&CpuInterpreter::ADC<RegA,RegH,0>,
&CpuInterpreter::ADC<RegA,RegL,0>,
&CpuInterpreter::ADC<RegA,RegHL,1>,
&CpuInterpreter::ADC<RegA,RegA,0>,

&CpuInterpreter::SUB<RegA,RegB,0>,
&CpuInterpreter::SUB<RegA,RegC,0>,
&CpuInterpreter::SUB<RegA,RegD,0>,
&CpuInterpreter::SUB<RegA,RegE,0>,
&CpuInterpreter::SUB<RegA,RegH,0>,
&CpuInterpreter::SUB<RegA,RegL,0>,
&CpuInterpreter::SUB<RegA,RegHL,1>,
&CpuInterpreter::SUB<RegA,RegA,0>,
&CpuInterpreter::SBC<RegA,RegB,0>,
&CpuInterpreter::SBC<RegA,RegC,0>,
&CpuInterpreter::SBC<RegA,RegD,0>,
&CpuInterpreter::SBC<RegA,RegE,0>,
&CpuInterpreter::SBC<RegA,RegH,0>,
&CpuInterpreter::SBC<RegA,RegL,0>,
&CpuInterpreter::SBC<RegA,RegHL,1>,
&CpuInterpreter::SBC<RegA,RegA,0>,

&CpuInterpreter::AND<RegA,RegB,0>,
&CpuInterpreter::AND<RegA,RegC,0>,
&CpuInterpreter::AND<RegA,RegD,0>,
&CpuInterpreter::AND<RegA,RegE,0>,
&CpuInterpreter::AND<RegA,RegH,0>,
&CpuInterpreter::AND<RegA,RegL,0>,
&CpuInterpreter::AND<RegA,RegHL,1>,
&CpuInterpreter::AND<RegA,RegA,0>,
&CpuInterpreter::XOR<RegA,RegB,0>,
&CpuInterpreter::XOR<RegA,RegC,0>,
&CpuInterpreter::XOR<RegA,RegD,0>,
&CpuInterpreter::XOR<RegA,RegE,0>,
&CpuInterpreter::XOR<RegA,RegH,0>,
&CpuInterpreter::XOR<RegA,RegL,0>,
&CpuInterpreter::XOR<RegA,RegHL,1>,
&CpuInterpreter::XOR<RegA,RegA,0>,

&CpuInterpreter::OR<RegA,RegB,0>,
&CpuInterpreter::OR<RegA,RegC,0>,
&CpuInterpreter::OR<RegA,RegD,0>,
&CpuInterpreter::OR<RegA,RegE,0>,
&CpuInterpreter::OR<RegA,RegH,0>,
&CpuInterpreter::OR<RegA,RegL,0>,
&CpuInterpreter::OR<RegA,RegHL,1>,
&CpuInterpreter::OR<RegA,RegA,0>,
&CpuInterpreter::CP_reg<RegB>,
&CpuInterpreter::CP_reg<RegC>,
&CpuInterpreter::CP_reg<RegD>,
&CpuInterpreter::CP_reg<RegE>,
&CpuInterpreter::CP_reg<RegH>,
&CpuInterpreter::CP_reg<RegL>,
&CpuInterpreter::CP_HL,
&CpuInterpreter::CP_reg<RegA>,

&CpuInterpreter::RET_cc<CpuFlagsZ,1>,
&CpuInterpreter::POP<RegBC>,
&CpuInterpreter::JP_cc<CpuFlagsZ,1>,
&CpuInterpreter::JP,
&CpuInterpreter::CALL_cc<CpuFlagsZ,1>,
&CpuInterpreter::PUSH<RegBC>,
&CpuInterpreter::ADD<RegA,0,2>,
&CpuInterpreter::RST,
&CpuInterpreter::RET_cc<CpuFlagsZ,0>,
&CpuInterpreter::RET,
&CpuInterpreter::JP_cc<CpuFlagsZ,0>,
&CpuInterpreter::PREFIX_CB,
&CpuInterpreter::CALL_cc<CpuFlagsZ,0>,
&CpuInterpreter::CALL,
&CpuInterpreter::ADC<RegA,0,2>,
&CpuInterpreter::RST,

&CpuInterpreter::RET_cc<CpuFlagsC,1>,
&CpuInterpreter::POP<RegDE>,
&CpuInterpreter::JP_cc<CpuFlagsC,1>,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::CALL_cc<CpuFlagsC,1>,
&CpuInterpreter::PUSH<RegDE>,
&CpuInterpreter::SUB<RegA,0,2>,
&CpuInterpreter::RST,
&CpuInterpreter::RET_cc<CpuFlagsC,0>,
&CpuInterpreter::RETI,
&CpuInterpreter::JP_cc<CpuFlagsC,0>,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::CALL_cc<CpuFlagsC,0>,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::SBC<RegA,0,2>,
&CpuInterpreter::RST,

&CpuInterpreter::LD<0,RegA,12>,
&CpuInterpreter::POP<RegHL>,
&CpuInterpreter::LD$FF00rr<RegC,RegA>,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::PUSH<RegHL>,
&CpuInterpreter::AND<RegA,0,2>,
&CpuInterpreter::RST,
&CpuInterpreter::ADD_SPr8,
&CpuInterpreter::JP_HL,
&CpuInterpreter::LD<0,RegA,14>,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::XOR<RegA,0,2>,
&CpuInterpreter::RST,

&CpuInterpreter::LD<RegA,0,13>,
&CpuInterpreter::POP<RegAF>,
&CpuInterpreter::LDr$FF00r<RegA,RegC>,
&CpuInterpreter::DI,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::PUSH<RegAF>,
&CpuInterpreter::OR<RegA,0,2>,
&CpuInterpreter::RST,
&CpuInterpreter::LDHLSPr8,
&CpuInterpreter::LDSPHL,
&CpuInterpreter::LD<RegA,0,15>,
&CpuInterpreter::EI,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::ILLEGAL,
&CpuInterpreter::CP_d8,
&CpuInterpreter::RST } {
  
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
  //sprite_bug = 0;
  ticks_to_switchspeed = 0;
}

void CpuInterpreter::HandleInterrupts() {

  uint8_t test = mem_->interrupt_enable() & mem_->interrupt_flag() & 0x1F;
  if (test) {
    emu_->MachineTick();
    emu_->MachineTick();
    emu_->MachineTick();
    ime = false;
    cpumode_ = CpuModeNormal;
    pushPC();

    if (test & 0x1) { //vblank
      reg.PC = 0x0040;
      mem_->interrupt_flag() &= ~0x1;
    }
    else if (test & 0x2) {
      reg.PC = 0x0048; //lcdc status
      mem_->interrupt_flag() &= ~0x2;
      emu_->lcd_driver()->int48signal = 0; //is this right?
    }
    else if (test & 0x4) {
      reg.PC = 0x0050; //timer overflow
      mem_->interrupt_flag() &= ~0x4;
      //mem_->interrupt_enable() &= ~0x4;
    }
    else if (test & 0x8) {
      reg.PC = 0x0058; //serial transfer
      mem_->interrupt_flag() &= ~0x8;
    }
    else if (test & 0x10) {
      reg.PC = 0x0060; //hi-lo p10-p13
      mem_->interrupt_flag() &= ~0x10;
    }
  }
}


void CpuInterpreter::ExecuteInstruction() {
  opcode_pc = reg.PC;
  //if (opcode_pc == 0x100) {
    //if (emu_->mode() == EmuModeGBC) //hack
    //  reg.A = 0x11;
  //}
  opcode = mem_->ClockedRead8(reg.PC++);

  //print_instruction(opcode_pc, opcode);
  (this->*(instructions[opcode]))();



}

void CpuInterpreter::Step() {
  this->dt = dt;
  //reg.F._unused = 0;//always 0 according to docs
  //StopAt(0x0100);
  //StopAt(0x0073);
  if (cpumode_ == CpuModeStop) {
    
    //(mem_->ioports()[0x4D] & 0x1) &&
    if (ticks_to_switchspeed != 0) {
      --ticks_to_switchspeed;

      //document specified ticks before switch, hwoever test rom doesnt reflect, will have to recheck , leave commented for now
      //uncommented again to check, demotronic requires ticks to countdown
      if (ticks_to_switchspeed == 0)
      {
        mem_->ioports()[0x4D] = 0;

        if (emu_->speed == EmuSpeed::EmuSpeedDouble) {
          emu_->speed = EmuSpeed::EmuSpeedNormal;
          emu_->set_base_freq_hz(default_gb_hz * 1);

        } else if (emu_->speed == EmuSpeed::EmuSpeedNormal) {
          emu_->speed = EmuSpeed::EmuSpeedDouble;
          emu_->set_base_freq_hz(default_gb_hz * 2);
        }
        Wake();

#ifdef _DEBUG    
        {
          char str[25];
          sprintf_s(str, "CPU Speed Change:%d\n", emu_->speed);
          OutputDebugString(str);
        }
#endif
        char str[25];
        sprintf_s(str, "CPU Speed Change:%d\n", emu_->speed);
        emu_->log_output(str);

      }
    }
    
    emu_->cpu_cycles_per_step_ = 0;
    return;
  } else  if (cpumode_ == CpuModeNormal) {
    //if (sprite_bug != 0) --sprite_bug;
    ExecuteInstruction();



  } 
  
  
  if (cpumode_ == CpuModeNormal) {
    if (ime) {
      HandleInterrupts();
    }
  } else if (cpumode_ == CpuModeHalt) {
    
    //cpu instr test fails halt without this, update: no need for this anymore
    //emu_->ClockTick();
    //if (ime) {
    //  HandleInterrupts();
    //}

    
    //demotronic works with this
    //
    
    if (ime) {
      //switch to clocktick if this not working
      emu_->MachineTick(); 
      HandleInterrupts();
    }
    else {
      uint8_t test = mem_->interrupt_enable() & mem_->interrupt_flag() & 0x1F;
      if (test == 0) {
        //demotronic and halt bug work because of this, but the intro of demotronic is uneven, i guess not done with halt yet.
        cpumode_ = CpuModeNormal;

        //fixes uneven demotronic but is not right
        //emu_->ClockTick(); //clock can be here ot above


        //ime = false;
        //cpumode_ = CpuModeNormal;
        //ExecuteInstruction();
        //Wake();
      
      } else {
        //strange check, how to fix for both proper halt bug and demotronic
        //char str[55];
        //sprintf_s(str, "PC %04x , IE %x IF %x\n", reg.PC, mem_->interrupt_enable(), mem_->interrupt_flag());
        //OutputDebugString(str);
        //ExecuteInstruction();
        opcode_pc = reg.PC;
        opcode = mem_->ClockedRead8(reg.PC);
        (this->*(instructions[opcode]))();
        //reg.PC = opcode_pc;
        //ExecuteInstruction();
       // reg.PC = opcode_pc;
        //emu_->MachineTick();
        cpumode_ = CpuModeNormal;
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

    //queue_speedswitch after 
    ticks_to_switchspeed = 130996;

    /*emu_->speed = ((mem_->ioports()[0x4D])>>7)+1;

    if (emu_->speed == EmuSpeedDouble)
      emu_->set_base_freq_hz(default_gb_hz*2);
    else
      emu_->set_base_freq_hz(default_gb_hz);
#ifdef _DEBUG    
    {
      char str[25];
      sprintf_s(str,"CPU Speed Change:%d\n",emu_->speed);
      OutputDebugString(str);
    }
#endif

*/

    //cpumode_ = CpuModeNormal;
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
  //check if need to uncomment the rest of the ocnditions
  if ((value>=0xFE00&&value<=0xFEFF) && lcdc.lcd_enable == 1 && mode != 1 && driver->scanline_counter() <= 72) {
    //if (driver->sprite_bug_counter == 0)
      driver->sprite_bug_counter = 80;
    //auto oam = mem_->oam();
    //for (int i=8;i<0xA0;++i)
    //  oam[i] = rand()&0xFF;
    //sprite_bug = 0;



  }
  
}

}
}