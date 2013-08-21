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

void Emu::Initialize() {
  cartridge_.Initialize(this);
  timer_.Initialize(this);
  lcd_driver_.Initialize(this);
  memory_.Initialize(this);
  cpu_.Initialize(this);
  apu_.Initialize(this);
  thread = null;
  state = 0;
}

void Emu::Deinitialize() {
  Stop();
  apu_.Deinitialize();
  cpu_.Deinitialize();
  memory_.Deinitialize();
  lcd_driver_.Deinitialize();
  timer_.Deinitialize();
  cartridge_.Deinitialize();
}

double Emu::Step() {
  const double dt =  1000.0 / emulation::gb::clockspeed;//options.cpu_freq(); 0.00058f;//16.667f;
  timing.current_cycles = utimer.GetCurrentCycles();
  double time_span =  (timing.current_cycles - timing.prev_cycles) * utimer.resolution();
  if (time_span > 500.0) //clamping time
    time_span = 500.0;

  timing.span_accumulator += time_span;
  while (timing.span_accumulator >= dt) {
    //emu_->cartridge()->mbc->Tick();
    cpu_.Step(dt);
    timing.span_accumulator -= dt*cpu_.cycles;
  }

  timing.total_cycles += timing.current_cycles-timing.prev_cycles;
  timing.prev_cycles = timing.current_cycles;
  timing.fps_time_span += time_span;
  return timing.span_accumulator;
}

void Emu::Run() {
  if (thread!=nullptr && state == 1) return;
  state = 1;
  thread = new std::thread(Emu::thread_func,this);
}

void Emu::Stop() {
  if (thread==nullptr && state == 0) return;
  apu_.output()->Stop();
  state = 0;
  thread->join();
  OutputDebugString("killed thread\n");
  SafeDelete(&thread);
}

void Emu::Pause() {
  Stop();
}

void Emu::Reset() {
  Stop();
  timer_.Reset();
  lcd_driver_.Reset();
  memory_.Reset();
  cpu_.Reset();
  apu_.Reset();
  //Run();
}

void Emu::Render() {
  timing.render_time_span = 0;
  ++timing.fps_counter;
  on_render();
  if (timing.fps_time_span >= 1000.0) {
    timing.fps = timing.fps_counter * (1000.0/timing.fps_time_span);
    timing.fps_counter = 0;
    timing.fps_time_span = 0;
  }
}

void Emu::thread_func(Emu* emu) {
  memset(&emu->timing,0,sizeof(emu->timing));
  emu->utimer.Calibrate();
  emu->timing.prev_cycles = emu->utimer.GetCurrentCycles();
  while (emu->state != 0) {
    emu->Step();
  }
  OutputDebugString("end of thread\n");
}



}
}
   