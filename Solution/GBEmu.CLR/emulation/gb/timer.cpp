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

  if (enable_timer_irq == true) {
    if (--timer_irq_delay == 0) {
      enable_timer_irq = false;
      emu_->memory()->interrupt_flag() |= 0x4;
      emu_->cpu()->Wake();
      tima = tma;
    }
  }

  ++counter1;
  div = ((counter1) >> 8) & 0xFF;



  bool do_count = ((counter1 >> tac_div_shift) & 1) & ((tac >> 2) & 1);
  bool inv_do_count = !do_count;
  if (inv_do_count & prev_do_count) {
    ++tima;
    //if tima == rapid works, but other test fail
    if (tima > 0xFF && enable_timer_irq == false) {
      timer_irq_delay = 4;
      enable_timer_irq = true;
      tima = 0;

    }
  }

  prev_do_count = do_count;

    /*if (++counter2 >= tima_max) {

      if ((tac & 0x4) != 0) {
        if (++tima > 0xFF) {
          emu_->memory()->interrupt_flag() |= 0x4;
          emu_->cpu()->Wake();
          tima = tma;

        }
      }
      counter2 -= tima_max;
    
    
  }*/
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
      return 0xF8|tac;
 
  }
  return 0;
}

void Timer::Write(uint16_t address, uint8_t data) {
  switch (address) {
    case 0xFF04:
      div = 0;
      counter1 = 0;
      
      break;
    case 0xFF05:
      tima = data;
      counter2 = 0;
      break;
    case 0xFF06:
      tma = data;
      break;
    case 0xFF07:
      tac = data;
      switch (tac & 0x3) {
      case 0: tac_div_shift = 9; tima_max = emu_->base_freq_hz() / 4096; break;
      case 1: tac_div_shift = 3; tima_max = emu_->base_freq_hz() / 262144; break;
      case 2: tac_div_shift = 5; tima_max = emu_->base_freq_hz() / 65536; break;
      case 3: tac_div_shift = 7; tima_max = emu_->base_freq_hz() / 16384; break;
      }

      break;
 
  }
}


}
}