#include "gb.h"


namespace emulation {
namespace gb {


void Timer::Initialize(Emu* emu) {
  Component::Initialize(emu);
	ioports = emu_->memory()->ioports();
}

void Timer::Deinitialize() {
 
}

void Timer::Tick() {

}

uint8_t Timer::Read(uint16_t address) {

  switch (address) {
    case 0xFF40:
      return 0;
 
  }
	return 0;
}

void Timer::Write(uint16_t address, uint8_t data) {
  switch (address) {
    case 0xFF10:
     
      break;
 
  }
}


}
}