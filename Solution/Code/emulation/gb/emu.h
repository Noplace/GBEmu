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
#pragma once

namespace emulation {
namespace gb {

const double default_gb_hz = 4194304.0; //(4.295454MHz for SGB, max. 8.4MHz for CGB)
//const uint32_t hsync_hz = 9198; // Hz (9420 KHz for SGB)
//const uint32_t vsync_hz = 59.730; // Hz (61.17 Hz for SGB)

enum EmuMode { EmuModeGB,EmuModeGBC };

class Emu {
 public:
  uint64_t cpu_cycles_,last_cpu_cycles_,cpu_cycles_per_step_;
  uint8_t speed;
  Emu():cpu_cycles_(0),last_cpu_cycles_(0),cpu_cycles_per_step_(0) {}
  ~Emu() {}
  void Initialize(double base_freq_hz);
  void Deinitialize();
  double Step();
  void Run();
  void Stop();
  void Pause();
  void Reset();
  void Render();
  EmuMode mode() { return mode_; }
  void set_mode(EmuMode mode) { mode_= mode; }
  Cartridge* cartridge() { return &cartridge_; }
  Cpu* cpu() { return &cpu_; }
  Memory* memory() { return &memory_; }
  LCDDriver* lcd_driver() { return &lcd_driver_; }
  Apu* apu() { return &apu_; }
  Timer* timer() { return &timer_; }
  std::function <void ()> on_render;
  void set_on_render(const  std::function <void ()>& on_render) {
    if (on_render != nullptr)
      this->on_render = on_render;
  } 
  std::atomic<int> state;
  const double fps() { return timing.fps; }
  const double base_freq_hz() { return base_freq_hz_; }
  void set_base_freq_hz(double base_freq_hz) { base_freq_hz_ = base_freq_hz; timing.step_dt =  1000.0 / base_freq_hz_; }
  double frequency_mhz() { return frequency_mhz_; }
  uint64_t cycles_per_second() { return cycles_per_second_; }

  void ClockTick() {
    ++cpu_cycles_per_step_;
    timer_.Tick();
    memory_.Tick();
    if (speed == 1) {
      lcd_driver_.Tick();
      
    } else {
      static bool s = false;
      if (s == false) {
        lcd_driver_.Tick();
        
      }
      s = !s;
    }
    apu_.Tick();
  }
  void MachineTick() {
    ClockTick();
    ClockTick();
    ClockTick();
    ClockTick();
  }

 private:
  uint64_t cycles_per_second_;
  utilities::Timer utimer;
  Cartridge cartridge_;
  Cpu cpu_;
  Memory memory_;
  LCDDriver lcd_driver_;
  Apu apu_;
  Timer timer_;
  std::thread* thread;
  double base_freq_hz_,frequency_mhz_;
  EmuMode mode_;
  struct {
    uint64_t extra_cycles;
    uint64_t current_cycles;
    uint64_t prev_cycles;
    uint64_t total_cycles;
    uint32_t fps_counter;
    double fps;
    double misc_time_span;
    double fps_time_span;
    double span_accumulator;
    double time_span;
    double step_dt;
  } timing;
  CRITICAL_SECTION cs;
  static void thread_func(Emu* emu);

};

}
}