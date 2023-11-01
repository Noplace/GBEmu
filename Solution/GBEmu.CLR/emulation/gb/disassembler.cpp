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


namespace emulation {
namespace gb {

const char* reg_names[] = { "F", "A", "C", "B", "E", "D", "L", "H", "SPl", "SPh", "PCl", "PCh" };
const char* reg16_names[] = { "AF", "BC", "DE", "HL", "SP", "PC" };
Disassembler::Disassembler() :
      instructions{
        &Disassembler::NOP,               &Disassembler::LDrd16<RegBC>,         &Disassembler::LD$rr<RegBC,RegA>, &Disassembler::INC_16bit<RegBC>,
        &Disassembler::INC_8bit<RegB,0>,  &Disassembler::DEC_8bit<RegB,0>,      &Disassembler::LD<RegB,0,10>,     &Disassembler::RLCA,
        &Disassembler::LDa16SP,           &Disassembler::ADD_16bit<RegHL,RegBC>,&Disassembler::LDr$r<RegA,RegBC>, &Disassembler::DEC_16bit<RegBC>,
        &Disassembler::INC_8bit<RegC,0>,  &Disassembler::DEC_8bit<RegC,0>,      &Disassembler::LD<RegC,0,10>,     &Disassembler::RRCA,

    &Disassembler::STOP,
    &Disassembler::LDrd16<RegDE>,
    &Disassembler::LD$rr<RegDE,RegA>,
    &Disassembler::INC_16bit<RegDE>,
    &Disassembler::INC_8bit<RegD,0>,
    &Disassembler::DEC_8bit<RegD,0>,
    &Disassembler::LD<RegD,0,10>,
    &Disassembler::RLA,
    &Disassembler::JR,
    &Disassembler::ADD_16bit<RegHL,RegDE>,
    &Disassembler::LDr$r<RegA,RegDE>,
    &Disassembler::DEC_16bit<RegDE>,
    &Disassembler::INC_8bit<RegE,0>,
    &Disassembler::DEC_8bit<RegE,0>,
    &Disassembler::LD<RegE,0,10>,
    &Disassembler::RRA,

    &Disassembler::JR_cc<CpuFlagsZ,1>,
    &Disassembler::LDrd16<RegHL>,
    &Disassembler::LDI$regreg<RegHL,RegA>,
    &Disassembler::INC_16bit<RegHL>,
    &Disassembler::INC_8bit<RegH,0>,
    &Disassembler::DEC_8bit<RegH,0>,
    &Disassembler::LD<RegH,0,10>,
    &Disassembler::DAA,
    &Disassembler::JR_cc<CpuFlagsZ,0>,
    &Disassembler::ADD_16bit<RegHL,RegHL>,
    &Disassembler::LDIreg$reg<RegA,RegHL>,
    &Disassembler::DEC_16bit<RegHL>,
    &Disassembler::INC_8bit<RegL,0>,
    &Disassembler::DEC_8bit<RegL,0>,
    &Disassembler::LD<RegL,0,10>,
    &Disassembler::CPL,

    &Disassembler::JR_cc<CpuFlagsC,1>,
    &Disassembler::LDrd16<RegSP>,
    &Disassembler::LDD$regreg<RegHL,RegA>,
    &Disassembler::INC_16bit<RegSP>,
    &Disassembler::INC_8bit<RegHL,1>,
    &Disassembler::DEC_8bit<RegHL,1>,
    &Disassembler::LD<RegHL,0,11>,
    &Disassembler::SCF,
    &Disassembler::JR_cc<CpuFlagsC,0>,
    &Disassembler::ADD_16bit<RegHL,RegSP>,
    &Disassembler::LDDreg$reg<RegA,RegHL>,
    &Disassembler::DEC_16bit<RegSP>,
    &Disassembler::INC_8bit<RegA,0>,
    &Disassembler::DEC_8bit<RegA,0>,
    &Disassembler::LD<RegA,0,10>,
    &Disassembler::CCF,

    &Disassembler::LDrr<RegB,RegB>,
    &Disassembler::LDrr<RegB,RegC>,
    &Disassembler::LDrr<RegB,RegD>,
    &Disassembler::LDrr<RegB,RegE>,
    &Disassembler::LDrr<RegB,RegH>,
    &Disassembler::LDrr<RegB,RegL>,
    &Disassembler::LDr$r<RegB,RegHL>,
    &Disassembler::LDrr<RegB,RegA>,
    &Disassembler::LDrr<RegC,RegB>,
    &Disassembler::LDrr<RegC,RegC>,
    &Disassembler::LDrr<RegC,RegD>,
    &Disassembler::LDrr<RegC,RegE>,
    &Disassembler::LDrr<RegC,RegH>,
    &Disassembler::LDrr<RegC,RegL>,
    &Disassembler::LDr$r<RegC,RegHL>,
    &Disassembler::LDrr<RegC,RegA>,

    &Disassembler::LDrr<RegD,RegB>,
    &Disassembler::LDrr<RegD,RegC>,
    &Disassembler::LDrr<RegD,RegD>,
    &Disassembler::LDrr<RegD,RegE>,
    &Disassembler::LDrr<RegD,RegH>,
    &Disassembler::LDrr<RegD,RegL>,
    &Disassembler::LDr$r<RegD,RegHL>,
    &Disassembler::LDrr<RegD,RegA>,
    &Disassembler::LDrr<RegE,RegB>,
    &Disassembler::LDrr<RegE,RegC>,
    &Disassembler::LDrr<RegE,RegD>,
    &Disassembler::LDrr<RegE,RegE>,
    &Disassembler::LDrr<RegE,RegH>,
    &Disassembler::LDrr<RegE,RegL>,
    &Disassembler::LDr$r<RegE,RegHL>,
    &Disassembler::LDrr<RegE,RegA>,

    &Disassembler::LDrr<RegH,RegB>,
    &Disassembler::LDrr<RegH,RegC>,
    &Disassembler::LDrr<RegH,RegD>,
    &Disassembler::LDrr<RegH,RegE>,
    &Disassembler::LDrr<RegH,RegH>,
    &Disassembler::LDrr<RegH,RegL>,
    &Disassembler::LDr$r<RegH,RegHL>,
    &Disassembler::LDrr<RegH,RegA>,
    &Disassembler::LDrr<RegL,RegB>,
    &Disassembler::LDrr<RegL,RegC>,
    &Disassembler::LDrr<RegL,RegD>,
    &Disassembler::LDrr<RegL,RegE>,
    &Disassembler::LDrr<RegL,RegH>,
    &Disassembler::LDrr<RegL,RegL>,
    &Disassembler::LDr$r<RegL,RegHL>,
    &Disassembler::LDrr<RegL,RegA>,

    &Disassembler::LD$rr<RegHL,RegB>,
    &Disassembler::LD$rr<RegHL,RegC>,
    &Disassembler::LD$rr<RegHL,RegD>,
    &Disassembler::LD$rr<RegHL,RegE>,
    &Disassembler::LD$rr<RegHL,RegH>,
    &Disassembler::LD$rr<RegHL,RegL>,
    &Disassembler::HALT,
    &Disassembler::LD$rr<RegHL,RegA>,
    &Disassembler::LDrr<RegA,RegB>,
    &Disassembler::LDrr<RegA,RegC>,
    &Disassembler::LDrr<RegA,RegD>,
    &Disassembler::LDrr<RegA,RegE>,
    &Disassembler::LDrr<RegA,RegH>,
    &Disassembler::LDrr<RegA,RegL>,
    &Disassembler::LDr$r<RegA,RegHL>,
    &Disassembler::LDrr<RegA,RegA>,

    &Disassembler::ADD<RegA,RegB,0>,
    &Disassembler::ADD<RegA,RegC,0>,
    &Disassembler::ADD<RegA,RegD,0>,
    &Disassembler::ADD<RegA,RegE,0>,
    &Disassembler::ADD<RegA,RegH,0>,
    &Disassembler::ADD<RegA,RegL,0>,
    &Disassembler::ADD<RegA,RegHL,1>,
    &Disassembler::ADD<RegA,RegA,0>,
    &Disassembler::ADC<RegA,RegB,0>,
    &Disassembler::ADC<RegA,RegC,0>,
    &Disassembler::ADC<RegA,RegD,0>,
    &Disassembler::ADC<RegA,RegE,0>,
    &Disassembler::ADC<RegA,RegH,0>,
    &Disassembler::ADC<RegA,RegL,0>,
    &Disassembler::ADC<RegA,RegHL,1>,
    &Disassembler::ADC<RegA,RegA,0>,

    &Disassembler::SUB<RegA,RegB,0>,
    &Disassembler::SUB<RegA,RegC,0>,
    &Disassembler::SUB<RegA,RegD,0>,
    &Disassembler::SUB<RegA,RegE,0>,
    &Disassembler::SUB<RegA,RegH,0>,
    &Disassembler::SUB<RegA,RegL,0>,
    &Disassembler::SUB<RegA,RegHL,1>,
    &Disassembler::SUB<RegA,RegA,0>,
    &Disassembler::SBC<RegA,RegB,0>,
    &Disassembler::SBC<RegA,RegC,0>,
    &Disassembler::SBC<RegA,RegD,0>,
    &Disassembler::SBC<RegA,RegE,0>,
    &Disassembler::SBC<RegA,RegH,0>,
    &Disassembler::SBC<RegA,RegL,0>,
    &Disassembler::SBC<RegA,RegHL,1>,
    &Disassembler::SBC<RegA,RegA,0>,

    &Disassembler::AND<RegA,RegB,0>,
    &Disassembler::AND<RegA,RegC,0>,
& Disassembler::AND<RegA, RegD, 0>,
& Disassembler::AND<RegA, RegE, 0>,
& Disassembler::AND<RegA, RegH, 0>,
& Disassembler::AND<RegA, RegL, 0>,
& Disassembler::AND<RegA, RegHL, 1>,
& Disassembler::AND<RegA, RegA, 0>,
& Disassembler::XOR<RegA, RegB, 0>,
& Disassembler::XOR<RegA, RegC, 0>,
& Disassembler::XOR<RegA, RegD, 0>,
& Disassembler::XOR<RegA, RegE, 0>,
& Disassembler::XOR<RegA, RegH, 0>,
& Disassembler::XOR<RegA, RegL, 0>,
& Disassembler::XOR<RegA, RegHL, 1>,
& Disassembler::XOR<RegA, RegA, 0>,

& Disassembler::OR<RegA, RegB, 0>,
& Disassembler::OR<RegA, RegC, 0>,
& Disassembler::OR<RegA, RegD, 0>,
& Disassembler::OR<RegA, RegE, 0>,
& Disassembler::OR<RegA, RegH, 0>,
& Disassembler::OR<RegA, RegL, 0>,
& Disassembler::OR<RegA, RegHL, 1>,
& Disassembler::OR<RegA, RegA, 0>,
& Disassembler::CP_reg<RegB>,
& Disassembler::CP_reg<RegC>,
& Disassembler::CP_reg<RegD>,
& Disassembler::CP_reg<RegE>,
& Disassembler::CP_reg<RegH>,
& Disassembler::CP_reg<RegL>,
& Disassembler::CP_HL,
& Disassembler::CP_reg<RegA>,

& Disassembler::RET_cc<CpuFlagsZ, 1>,
& Disassembler::POP<RegBC>,
& Disassembler::JP_cc<CpuFlagsZ, 1>,
& Disassembler::JP,
& Disassembler::CALL_cc<CpuFlagsZ, 1>,
& Disassembler::PUSH<RegBC>,
& Disassembler::ADD<RegA, 0, 2>,
& Disassembler::RST,
& Disassembler::RET_cc<CpuFlagsZ, 0>,
& Disassembler::RET,
& Disassembler::JP_cc<CpuFlagsZ, 0>,
& Disassembler::PREFIX_CB,
& Disassembler::CALL_cc<CpuFlagsZ, 0>,
& Disassembler::CALL,
& Disassembler::ADC<RegA, 0, 2>,
& Disassembler::RST,

& Disassembler::RET_cc<CpuFlagsC, 1>,
& Disassembler::POP<RegDE>,
& Disassembler::JP_cc<CpuFlagsC, 1>,
& Disassembler::ILLEGAL,
& Disassembler::CALL_cc<CpuFlagsC, 1>,
& Disassembler::PUSH<RegDE>,
& Disassembler::SUB<RegA, 0, 2>,
& Disassembler::RST,
& Disassembler::RET_cc<CpuFlagsC, 0>,
& Disassembler::RETI,
& Disassembler::JP_cc<CpuFlagsC, 0>,
& Disassembler::ILLEGAL,
& Disassembler::CALL_cc<CpuFlagsC, 0>,
& Disassembler::ILLEGAL,
& Disassembler::SBC<RegA, 0, 2>,
& Disassembler::RST,

& Disassembler::LD<0, RegA, 12>,
& Disassembler::POP<RegHL>,
& Disassembler::LD$FF00rr<RegC, RegA>,
& Disassembler::ILLEGAL,
& Disassembler::ILLEGAL,
& Disassembler::PUSH<RegHL>,
& Disassembler::AND<RegA, 0, 2>,
& Disassembler::RST,
& Disassembler::ADD_SPr8,
& Disassembler::JP_HL,
& Disassembler::LD<0, RegA, 14>,
& Disassembler::ILLEGAL,
& Disassembler::ILLEGAL,
& Disassembler::ILLEGAL,
& Disassembler::XOR<RegA, 0, 2>,
& Disassembler::RST,

& Disassembler::LD<RegA, 0, 13>,
& Disassembler::POP<RegAF>,
& Disassembler::LDr$FF00r<RegA, RegC>,
& Disassembler::DI,
& Disassembler::ILLEGAL,
& Disassembler::PUSH<RegAF>,
& Disassembler::OR<RegA, 0, 2>,
& Disassembler::RST,
& Disassembler::LDHLSPr8,
& Disassembler::LDSPHL,
& Disassembler::LD<RegA, 0, 15>,
& Disassembler::EI,
& Disassembler::ILLEGAL,
& Disassembler::ILLEGAL,
& Disassembler::CP_d8,
& Disassembler::RST } {

}

Disassembler::~Disassembler() {

}

void Disassembler::Initialize(Emu* emu) {
  Component::Initialize(emu);
  mem_ = emu_->memory();
  Reset();
  memset(&reg, 0, sizeof(reg));
}

void Disassembler::Deinitialize() {

}

void Disassembler::FormatInstruction() {
  opcode_pc = reg.PC;
  opcode = mem_->Read8(reg.PC++);
  (this->*(instructions[opcode]))();
}


void Disassembler::NOP() {
  sprintf_s(current_str_, "%sNOP", prefix());
}

    void Disassembler::ILLEGAL() {
      sprintf_s(current_str_, "%sILLEGAL", prefix());
    }

    void Disassembler::RST() {
      uint8_t t = (opcode & 0x38) >> 3;
      auto target = t * 8;
      sprintf_s(current_str_, "%sRST %04x", prefix(), target);
    }


    template<uint8_t dest, uint8_t src>
    void Disassembler::LDrr() {
      
      sprintf_s(current_str_, "%sLD %s,%s", prefix(),reg_names[dest],reg_names[src] );
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LD$rr() { //(dest), src
      sprintf_s(current_str_, "%sLD %s,%s", prefix(), reg16_names[dest], reg_names[src]);
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LDr$r() {
      sprintf_s(current_str_, "%sLD %s,%s", prefix(), reg_names[dest], reg16_names[src]);
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LD$FF00rr() {
      sprintf_s(current_str_, "%sLD (FF00+%s),%s", prefix(), reg_names[dest], reg_names[src]);
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LDr$FF00r() {
      sprintf_s(current_str_, "%sLD %s,(FF00+%s)", prefix(), reg_names[dest], reg_names[src]);
    }

    template<uint8_t dest>
    void Disassembler::LDrd16() {
     auto d = mem_->Read8(reg.PC++);
      d |= (mem_->Read8(reg.PC++)) << 8;

      sprintf_s(current_str_, "%sLD %s,%04x", prefix(), reg16_names[dest], d);
  
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LDI$regreg() {
      sprintf_s(current_str_, "%sLDI %s,%s", prefix(), reg16_names[dest], reg_names[src]);

    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LDD$regreg() {
      sprintf_s(current_str_, "%sLDD %s,%s", prefix(), reg16_names[dest], reg_names[src]);
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LDIreg$reg() {
      sprintf_s(current_str_, "%sLDI %s,%s", prefix(), reg_names[dest], reg16_names[src]);
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::LDDreg$reg() {
      sprintf_s(current_str_, "%sLDD %s,%s", prefix(), reg_names[dest], reg16_names[src]);
    }

    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::LD() {
      if (mode == 10) { //dest,d8
        auto d8 = mem_->Read8(reg.PC++);
        sprintf_s(current_str_, "%sLD %s,%02X", prefix(), reg_names[dest],d8);
      } else if (mode == 11) { //(dest),d8
        auto d8 = mem_->Read8(reg.PC++);
        sprintf_s(current_str_, "%sLD (%s),%02X", prefix(), reg16_names[dest], d8);
        
        //mem_->ClockedWrite8(reg.raw16[dest], d8);
      } else if (mode == 12) { //0xFF00+d8 src

        uint8_t a8 = mem_->Read8(reg.PC++);
        //mem_->ClockedWrite8(0xFF00 + a8, reg.raw8[src]);
        sprintf_s(current_str_, "%sLD (FF00+%02X),%s", prefix(), a8, reg_names[src]);
      } else if (mode == 13) { //dest,0xFF00+d8 
        uint8_t a8 = mem_->Read8(reg.PC++);
        sprintf_s(current_str_, "%sLD %s,(FF00+%02X)", prefix(), reg_names[src], a8);

        //uint8_t a8 = mem_->ClockedRead8(reg.PC++);
        //reg.raw8[dest] = mem_->ClockedRead8(0xFF00 + a8);
      } else if (mode == 14) { //(d16),src

        uint16_t d16 = mem_->Read8(reg.PC++);
        d16 |= (mem_->Read8(reg.PC++)) << 8;

        sprintf_s(current_str_, "%sLD (%04X),%s", prefix(), d16, reg_names[src]);
        
        //mem_->ClockedWrite8(d16, reg.raw8[src]);

      } else if (mode == 15) { //src,(d16)
        uint16_t d16 = mem_->Read8(reg.PC++);
        d16 |= (mem_->Read8(reg.PC++)) << 8;
        sprintf_s(current_str_, "%sLD %s,(%04X)", prefix(), reg_names[dest], d16);

        //uint16_t d16 = mem_->ClockedRead8(reg.PC++);
        //d16 |= (mem_->ClockedRead8(reg.PC++)) << 8;
        //reg.raw8[dest] = mem_->ClockedRead8(d16);
      }
    }

    void Disassembler::LDSPHL() {
      sprintf_s(current_str_, "%sLDSPHL", prefix());
    }

    void Disassembler::LDHLSPr8() {


      /*
      reg.F.N = reg.F.Z = 0;
      uint16_t a = reg.SP;
      int8_t r8 = mem_->ClockedRead8(reg.PC++);
      reg.HL = (reg.SP + r8);
      updateCpuFlagC(a & 0xFF, r8, 0);
      updateCpuFlagH(a & 0xFF, r8, 0);
      emu_->MachineTick();
      */

      int8_t r8 = mem_->Read8(reg.PC++);
      sprintf_s(current_str_, "%sLDHLSPr8 %02X", prefix(), r8);

    }

    void Disassembler::LDa16SP() {
      uint16_t a16 = mem_->Read8(reg.PC++);
      a16 |= (mem_->Read8(reg.PC++)) << 8;
     /* mem_->ClockedWrite8(a16, reg.SP & 0xFF);
      mem_->ClockedWrite8(a16 + 1, reg.SP >> 8);*/

      int8_t r8 = mem_->Read8(reg.PC++);
      sprintf_s(current_str_, "%sLDa16SP (%04X),SP", prefix(), a16);
    }

    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::ADD() {
      


      char a[25], b[25];
      arithmeticMode<dest, src, mode>(a, b);
      sprintf_s(current_str_, "%sADD %s,%s",prefix(), a,b);
    }

    template<uint8_t dest, uint8_t src>
    void Disassembler::ADD_16bit() {
      /*reg.F.N = 0;
      uint16_t a = 0, b = 0;
      a = reg.raw16[dest];
      b = reg.raw16[src];
      reg.raw16[dest] = a + b;
      uint16_t r1 = (a & 0xFFF) + (b & 0xFFF);
      reg.F.H = r1 > 0xFFF ? 1 : 0;
      uint32_t r2 = (a & 0xFFFF) + (b & 0xFFFF);
      reg.F.C = r2 > 0xFFFF ? 1 : 0;
      emu_->MachineTick();*/
    }

    void Disassembler::ADD_SPr8() {
      /*reg.F.N = reg.F.Z = 0;
      uint16_t a = reg.SP;
      int8_t r8 = mem_->ClockedRead8(reg.PC++);
      reg.SP += r8;

      updateCpuFlagC(a & 0xFF, r8, 0);
      updateCpuFlagH(a & 0xFF, r8, 0);
      emu_->MachineTick();
      emu_->MachineTick();*/

      int8_t r8 = mem_->Read8(reg.PC++);
      sprintf_s(current_str_, "%sADD_SPr8 %02X", prefix(), r8);
    }

    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::ADC() {

      char a[25], b[25];
      arithmeticMode<dest, src, mode>(a, b);
      sprintf_s(current_str_, "%sADC %s,%s", prefix(), a, b);

    }



    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::SUB() {

      char a[25], b[25];
      arithmeticMode<dest, src, mode>(a, b);
      sprintf_s(current_str_, "%sSUB %s,%s", prefix(), a, b);
    }

    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::SBC() {

      char a[25], b[25];
      arithmeticMode<dest, src, mode>(a, b);
      sprintf_s(current_str_, "%sSBC %s,%s", prefix(), a, b);
    }

    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::AND() {



      char a[25], b[25];
      arithmeticMode<dest, src, mode>(a, b);
      sprintf_s(current_str_, "%sAND %s,%s", prefix(), a, b);


    }


    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::XOR() {

      char a[25], b[25];
      arithmeticMode<dest, src, mode>(a, b);
      sprintf_s(current_str_, "%sXOR %s,%s", prefix(), a, b);
    }

    template<uint8_t dest, uint8_t src, int mode>
    void Disassembler::OR() {

      char a[25], b[25];
      arithmeticMode<dest, src, mode>(a, b);
      sprintf_s(current_str_, "%sOR %s,%s", prefix(), a, b);
    }

    void Disassembler::SCF() {
      //reg.F.H = reg.F.N = 0;
      //reg.F.C = 1;
      sprintf_s(current_str_, "%sSCF", prefix());
    }

    void Disassembler::CCF() {
     // reg.F.H = reg.F.N = 0;
      //reg.F.C = ~reg.F.C;
      sprintf_s(current_str_, "%sCCF", prefix());
    }

    void Disassembler::HALT() {
      sprintf_s(current_str_, "%sHALT", prefix());
    }




    void Disassembler::STOP() {
      sprintf_s(current_str_, "%sSTOP", prefix());
    }

    void Disassembler::CPL() {
      sprintf_s(current_str_, "%sCPL", prefix());
    }

    const CpuRegisterNames8 reg_index[8] = {
        RegB,RegC,RegD,RegE,RegH,RegL,RegH,RegA,
    };

    void Disassembler::PREFIX_CB() {
     
      uint8_t code = mem_->Read8(reg.PC++);

      sprintf_s(current_str_, "%sPREFIX_CB %02x", prefix(), code);

      /*
      auto getr = [=]() {
        if ((code & 0x7) != 6) {
          return reg.raw8[reg_index[code & 0x7]];
        } else {

          return mem_->ClockedRead8(reg.HL);
        }
      };

      auto setr = [=](uint8_t r) {
        if ((code & 0x7) != 6) {
          reg.raw8[reg_index[code & 0x7]] = r;
        } else {
          mem_->ClockedWrite8(reg.HL, r);
        }
      };

      if ((code & 0xC0) == 0x40) { //bit
        uint8_t test = getr();
        uint8_t bitshift = (code & 0x38) >> 3;
        reg.F.Z = (~(((test & (1 << bitshift)) >> bitshift)) & 0x1);
        reg.F.H = 1;
        reg.F.N = 0;

      } else if ((code & 0xC0) == 0x80) { //res
        uint8_t test = getr();
        uint8_t bitshift = (code & 0x38) >> 3;
        test &= ~(1 << bitshift);
        setr(test);
      } else if ((code & 0xC0) == 0xC0) { //set
        uint8_t test = getr();
        uint8_t bitshift = (code & 0x38) >> 3;
        test |= (1 << bitshift);
        setr(test);
      } else  if ((code & 0xF8) == 0x30) { //swap
        uint8_t r = getr();
        uint8_t n0 = r & 0xF;
        uint8_t n1 = (r & 0xF0) >> 4;
        r = n1 | (n0 << 4);
        setr(r);
        reg.F.H = reg.F.N = reg.F.C = 0;
        updateCpuFlagZ(r);
      } else if ((code & 0xF8) == 0) { //RLC r
        uint8_t r = getr();
        reg.F.C = (r & 0x80) != 0 ? 1 : 0;
        r = r << 1;
        r |= reg.F.C;
        setr(r);
        reg.F.H = reg.F.N = 0;
        updateCpuFlagZ(r);
      } else if ((code & 0xF8) == 0x08) { //RRC r
        uint8_t r = getr();
        reg.F.C = r & 1;
        r = r >> 1;
        r |= reg.F.C << 7;
        setr(r);
        reg.F.H = reg.F.N = 0;
        updateCpuFlagZ(r);
      } else if ((code & 0xF8) == 0x10) { //RL r
        uint8_t r = getr();
        uint8_t oldC = reg.F.C;
        reg.F.C = ((r & 0x80) >> 7);
        r = r << 1;
        r |= oldC;
        setr(r);
        reg.F.H = reg.F.N = 0;
        updateCpuFlagZ(r);
      } else if ((code & 0xF8) == 0x18) { //RR r
        uint8_t r = getr();
        uint8_t oldC = reg.F.C;
        reg.F.C = r & 1;
        r = r >> 1;
        r |= oldC << 7;
        setr(r);
        reg.F.H = reg.F.N = 0;
        updateCpuFlagZ(r);

      } else if ((code & 0xF8) == 0x20) { //SLA r
        uint8_t r = getr();
        reg.F.C = ((r & 0x80) >> 7);
        r = r << 1;
        r &= ~0x01;
        setr(r);
        reg.F.H = reg.F.N = 0;
        updateCpuFlagZ(r);
      } else if ((code & 0xF8) == 0x28) { //SRA r
        uint8_t r = getr();
        reg.F.C = r & 1;
        r = (r & 0x80) + (r >> 1);
        setr(r);
        reg.F.H = reg.F.N = 0;
        updateCpuFlagZ(r);
      } else if ((code & 0xF8) == 0x38) { //SRL n
        auto r = getr();
        reg.F.C = r & 1;
        r >>= 1;
        setr(r);
        reg.F.H = reg.F.N = 0;
        updateCpuFlagZ(r);
      }
      //Tick();Tick();Tick();Tick();*/
    }

    void Disassembler::JR() {


      int8_t disp8 = mem_->Read8(reg.PC++);
      auto target = reg.PC + disp8;
      sprintf_s(current_str_, "%sJR %04X", prefix(), target);

    }

    template<CpuFlags condbit, bool inv>
    void Disassembler::JR_cc() {
      /*int table[2] = { ((reg.F.raw & (1 << condbit)) >> condbit),
        ~((reg.F.raw & (1 << condbit)) >> condbit) };

      if (table[inv] & 1) {
        JR();
      } else {
        int8_t disp8 = mem_->ClockedRead8(reg.PC++);
      }*/

      int8_t disp8 = mem_->Read8(reg.PC++);
      auto target = reg.PC + disp8;
      sprintf_s(current_str_, "%sJR CC%1d%1d %04X", prefix(), condbit, inv, target);


    }

    template<uint8_t dest, int mode>
    void Disassembler::INC_8bit() {
      sprintf_s(current_str_, "%sINC_8bit", prefix());
      /*reg.F.N = 0;

      if (mode == 0) {
        updateCpuFlagH(reg.raw8[dest], 1, 0);
        ++reg.raw8[dest];
        updateCpuFlagZ(reg.raw8[dest]);
      } else {
        uint8_t data = mem_->ClockedRead8(reg.HL);
        updateCpuFlagH(data, 1, 0);
        ++data;
        mem_->ClockedWrite8(reg.HL, data);
        updateCpuFlagZ(data);
      }
      */

    }

    template<uint8_t dest>
    void Disassembler::INC_16bit() {
     /* if (dest != RegAF)
        simulateSpriteBug(reg.raw16[dest]);
      ++reg.raw16[dest];
      emu_->MachineTick();*/
      sprintf_s(current_str_, "%sINC_16bit", prefix());
    }

    template<uint8_t dest, int mode>
    void Disassembler::DEC_8bit() {
      sprintf_s(current_str_, "%sDEC_8bit", prefix());
      /*reg.F.N = 1;

      if (mode == 0) {
        //updateCpuFlagH(reg.raw8[dest],1,1);
        --reg.raw8[dest];
        if ((reg.raw8[dest] & 0xF) == 0xF)
          reg.F.H = 1;
        else
          reg.F.H = 0;
        updateCpuFlagZ(reg.raw8[dest]);
      } else {
        uint8_t data = mem_->ClockedRead8(reg.HL);

        //updateCpuFlagH(data,1,1);
        --data;
        if ((data & 0xF) == 0xF)
          reg.F.H = 1;
        else
          reg.F.H = 0;
        updateCpuFlagZ(data);
        mem_->ClockedWrite8(reg.HL, data);

      }*/

    }

    template<uint8_t dest>
    void Disassembler::DEC_16bit() {
     /* if (dest != RegAF)
        simulateSpriteBug(reg.raw16[dest]);
      --reg.raw16[dest];
      emu_->MachineTick();*/
      sprintf_s(current_str_, "%sDEC_16bit", prefix());
    }

    void Disassembler::JP() {
      auto nn = mem_->Read8(reg.PC++);
      nn |= (mem_->Read8(reg.PC++)) << 8;
      
      sprintf_s(current_str_, "%sJP %04X", prefix(), nn);


    }

    template<CpuFlags condbit, bool inv>
    void Disassembler::JP_cc() {

      auto nn = mem_->Read8(reg.PC++);
      nn |= (mem_->Read8(reg.PC++)) << 8;

      sprintf_s(current_str_, "%sJP CC%1d%1d %04X", prefix(), condbit, inv, nn);

      /*int table[2] = { ((reg.F.raw & (1 << condbit)) >> condbit),
        ~((reg.F.raw & (1 << condbit)) >> condbit) };




      if (table[inv] & 1) {
        JP();
      } else {
        uint16_t nn;
        nn = mem_->ClockedRead8(reg.PC++);
        nn |= (mem_->ClockedRead8(reg.PC++)) << 8;
      }*/
    }

    void Disassembler::JP_HL() {
      //reg.PC = reg.HL;
      sprintf_s(current_str_, "%sJP_HL", prefix());

    }


    void Disassembler::CALL() {
      uint16_t nn;
      nn = mem_->Read8(reg.PC++);
      nn |= (mem_->Read8(reg.PC++)) << 8;
      sprintf_s(current_str_, "%sCALL %04X", prefix(),nn);

     /* pushPC();
      reg.PC = nn;

      emu_->MachineTick();*/
    }

    template<CpuFlags condbit, bool inv>
    void Disassembler::CALL_cc() {
      uint16_t nn;
      nn = mem_->Read8(reg.PC++);
      nn |= (mem_->Read8(reg.PC++)) << 8;
      sprintf_s(current_str_, "%sCALL CC%1d%1d %04X", prefix(), condbit, inv, nn);


     /* int table[2] = { ((reg.F.raw & (1 << condbit)) >> condbit),
        ~((reg.F.raw & (1 << condbit)) >> condbit) };

      if (table[inv] & 1) {
        CALL();
      } else {
        uint16_t nn;
        nn = mem_->ClockedRead8(reg.PC++);
        nn |= (mem_->ClockedRead8(reg.PC++)) << 8;
      }*/
    }

    void Disassembler::RET() {
      sprintf_s(current_str_, "%sRET", prefix());
     // reg.PC = pop();
     // reg.PC |= pop() << 8;
      //emu_->MachineTick();
    }

    template<CpuFlags condbit, bool inv>
    void Disassembler::RET_cc() {
      sprintf_s(current_str_, "%sRET CC%1d%1d", prefix(), condbit, inv);
      /*int table[2] = { ((reg.F.raw & (1 << condbit)) >> condbit),
        ~((reg.F.raw & (1 << condbit)) >> condbit) };

      if (table[inv] & 1) {
        RET();
      } else {

      }
      emu_->MachineTick();*/
    }


    template<uint8_t src>
    void Disassembler::PUSH() {
      sprintf_s(current_str_, "%sPUSH %s", prefix(), reg_names[src]);
      /*simulateSpriteBug(reg.SP + 1);
      push(reg.raw16[src] >> 8);
      push(reg.raw16[src] & 0xFF);
      emu_->MachineTick();*/
    }

    template<uint8_t dest>
    void Disassembler::POP() {
      sprintf_s(current_str_, "%sPOP %s", prefix(), reg_names[dest]);
     /* simulateSpriteBug(reg.SP + 1);
      reg.raw16[dest] = pop();
      reg.raw16[dest] |= pop() << 8;
      if (dest == RegAF)
        reg.F._unused = 0;*/


    }

    void Disassembler::RLCA() {
      sprintf_s(current_str_, "%sRLCA", prefix());
     /* uint8_t& r = reg.A;
      reg.F.C = (r & 0x80) >> 7;
      r = r << 1;
      r |= reg.F.C;
      reg.F.H = reg.F.N = 0;
      reg.F.Z = 0;
      // updateCpuFlagZ(r);*/
    }

    void Disassembler::RRCA() {
      sprintf_s(current_str_, "%sRRCA", prefix());
     /* uint8_t& r = reg.A;
      reg.F.C = r & 1;
      r = r >> 1;
      r |= reg.F.C << 7;
      reg.F.H = reg.F.N = 0;
      reg.F.Z = 0;
      //updateCpuFlagZ(r);*/
    }

    void Disassembler::RLA() {
      sprintf_s(current_str_, "%sRLA", prefix());
     /* uint8_t& r = reg.A;;
      uint8_t oldC = reg.F.C;
      reg.F.C = (r & 0x80) >> 7;
      r = r << 1;
      r |= oldC;
      reg.F.H = reg.F.N = 0;
      reg.F.Z = 0;
      //updateCpuFlagZ(r);*/
    }

    void Disassembler::RRA() {
      sprintf_s(current_str_, "%sRRA", prefix());
      /*uint8_t& r = reg.A;;
      uint8_t oldC = reg.F.C;
      reg.F.C = r & 1;
      r = r >> 1;
      r |= oldC << 7;
      reg.F.H = reg.F.N = 0;
      reg.F.Z = 0;
      //updateCpuFlagZ(r);*/
    }

    void Disassembler::CP(uint8_t a, uint8_t b) {
     
      /*reg.F.N = 1;
      updateCpuFlagC(a, b, 1);
      updateCpuFlagH(a, b, 1);
      updateCpuFlagZ(a - b);*/
    }

    template<CpuRegisterNames8 r>
    void Disassembler::CP_reg() {
      //CP(reg.A, reg.raw8[r]);
      sprintf_s(current_str_, "%sCP A,%s", prefix(), reg_names[r]);
    }

    void Disassembler::CP_d8() {
      auto d = mem_->Read8(reg.PC++);
     // CP(reg.A, mem_->ClockedRead8(reg.PC++));
      sprintf_s(current_str_, "%sCP A,%02X", prefix(), d);
    }

    void Disassembler::CP_HL() {
      //CP(reg.A, mem_->ClockedRead8(reg.HL));
      // CP(reg.A, mem_->ClockedRead8(reg.PC++));
      sprintf_s(current_str_, "%sCP A,(HL)", prefix());
    }

    void Disassembler::DI() {
     // ime = false;
      sprintf_s(current_str_, "%sDI", prefix());
    }

    void Disassembler::EI() {
     // ime = true;
      sprintf_s(current_str_, "%sEI", prefix());
    }


    void Disassembler::RETI() {
      sprintf_s(current_str_, "%sRETI", prefix());
      //  EI();
       // RET();
    }

    void Disassembler::DAA() {
      sprintf_s(current_str_, "%sDAA", prefix());
      /*int a = reg.A;

      if (!reg.F.N) {
        if (reg.F.H || (a & 0xF) > 9)
          a += 0x06;
        if (reg.F.C || a > 0x9F)
          a += 0x60;
      } else {
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
      reg.A = (uint8_t)a;*/
    }



}
}