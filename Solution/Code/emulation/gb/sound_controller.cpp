#include "gb.h"


namespace emulation {
namespace gb {

const uint32_t colormap[4] = {0xffffffff,0xffAAAAAA,0xff545454,0xff000000};

void SoundController::Initialize(Emu* emu) {
  Component::Initialize(emu);
	ioports = emu_->memory()->ioports();
}

void SoundController::Deinitialize() {
 
}

void SoundController::Step(double dt) {

}

uint8_t SoundController::Read(uint16_t address) {

  switch (address) {
    case 0xFF40:
      return 0;
 
  }
	return 0;
}

void SoundController::Write(uint16_t address, uint8_t data) {
  switch (address) {
    case 0xFF10:
     
      break;
 
  }
}


}
}