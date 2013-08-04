#pragma once

namespace emulation {
namespace gb {

const double clockspeed = 4194304.0; //(4.295454MHz for SGB, max. 8.4MHz for CGB)
const uint32_t hsync_hz = 9198; // Hz (9420 KHz for SGB)
//const uint32_t vsync_hz = 59.730; // Hz (61.17 Hz for SGB)

class Emu {
 public:
  Emu() {}
  ~Emu() {}
  void Initialize();
  void Deinitialize();
  double Step(double dt);
  uint32_t* frame_buffer;

  Cartridge* cartridge() { return &cartridge_; }
  Cpu* cpu() { return &cpu_; }
  Memory* memory() { return &memory_; }
  LCDDriver* lcd_driver() { return &lcd_driver_; }
 private:
  Cartridge cartridge_;
  Cpu cpu_;
  Memory memory_;
  LCDDriver lcd_driver_;

};

}
}