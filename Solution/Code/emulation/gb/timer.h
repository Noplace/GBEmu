#pragma once

namespace emulation {
namespace gb {

	
class Timer : public Component {
 public:

  Timer():ioports(nullptr) {}
  ~Timer() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Tick();
  uint8_t Read(uint16_t address);
  void  Write(uint16_t address, uint8_t data);
 private:
	uint8_t* ioports;
};

}
}