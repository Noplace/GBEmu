#include "gb.h"


namespace emulation {
namespace gb {


void Timer::Initialize(Emu* emu) {
  Component::Initialize(emu);
	ioports = emu_->memory()->ioports();
  div = tima = tma = tac = 0;
  counter1 = 0;
  counter2 = 0;
  tima_max = 0;
}

void Timer::Deinitialize() {
 
}

void Timer::Tick() {
   
  if (++counter1 == (4194304/16384)) {
    counter1 = 0;
    ++div;
  }
  if ((tac&0x4)==0)return;
  if (++counter2 == tima_max) {
    if (tima == 0xFF) {
      emu_->memory()->interrupt_flag() |= 0x4;
      emu_->cpu()->Wake();
      tima = tma;
    }
    else
      ++tima;
    counter2 = 0;
  }



}

uint8_t Timer::Read(uint16_t address) {

  switch (address) {
    case 0xFF04:
      return div;
    case 0xFF05:
      return tima;
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
        case 1: tima_max = 4194304/262144 ; break;
          case 2: tima_max = 4194304/65536 ; break;
            case 3: tima_max = 4194304/16384 ; break;
      }

      break;
 
  }
}


}
}