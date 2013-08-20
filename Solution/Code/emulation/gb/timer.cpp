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


void Timer::Initialize(Emu* emu) {
  Component::Initialize(emu);
  Reset();
}

void Timer::Deinitialize() {
 
}

void Timer::Reset() {
  div = tma = tac = 0;
  tima = 0;
  counter1 = 0;
  counter2 = 0;
  tima_max = 0;
}

void Timer::Tick() {
   
  if (++counter1 == (4194304/16384)) {
    counter1 = 0;
    ++div;
  }
  if ((tac&0x4)==0)return;
  if (++counter2 == tima_max) {
    if (++tima > 0xFF) {
      emu_->memory()->interrupt_flag() |= 0x4;
      emu_->cpu()->Wake();
      tima = tma;
    }

    counter2 = 0;
  }
}

void Timer::Check() {


}


uint8_t Timer::Read(uint16_t address) {

  switch (address) {
    case 0xFF04:
      return div;
    case 0xFF05:
      return tima&0xFF;
    case 0xFF06:
      return tma;
    case 0xFF07:
      return tac;
 
  }
  return 0;
}

void Timer::Write(uint16_t address, uint8_t data) {
  switch (address) {
    case 0xFF04:
      div = 0;
      break;
    case 0xFF05:
      tima = data;
      break;
    case 0xFF06:
      tma = data;
      break;
    case 0xFF07:
      tac = data;
      switch(tac&0x3) {
      case 0: tima_max = 4194304/4096 ; break;
        case 1: tima_max = 4194304/262144; break;
          case 2: tima_max = 4194304/65536 ; break;
            case 3: tima_max = 4194304/16384 ; break;
      }

      break;
 
  }
}


}
}