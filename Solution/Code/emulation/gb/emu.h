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


  Cartridge* cartridge() { return &cartridge_; }
  Cpu* cpu() { return &cpu_; }
  Memory* memory() { return &memory_; }
  LCDDriver* lcd_driver() { return &lcd_driver_; }
	SoundController* sc() { return &sc_; }
	Timer* timer() { return &timer_; }
  std::function <void ()> on_render;
  void set_on_render(const  std::function <void ()>& on_render) {
    if (on_render != nullptr)
      this->on_render = on_render;
  } 

  std::function <void ()> on_vertical_blank;
  void set_on_vertical_blank(const  std::function <void ()>& on_vertical_blank) {
    if (on_vertical_blank != nullptr)
      this->on_vertical_blank = on_vertical_blank;
  } 
 private:
  Cartridge cartridge_;
  Cpu cpu_;
  Memory memory_;
  LCDDriver lcd_driver_;
	SoundController sc_;
	Timer timer_;

};

}
}