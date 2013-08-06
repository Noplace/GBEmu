#pragma once

namespace emulation {
namespace gb {

	
class SoundController : public Component {
 public:

  SoundController():ioports(nullptr) {}
  ~SoundController() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Step(double dt);
  uint8_t Read(uint16_t address);
  void  Write(uint16_t address, uint8_t data);
 private:
	uint8_t* ioports;
};

}
}