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
#include "../../gbemu.h"


inline void printThreadId() {
  {
    char str[50];
    sprintf_s(str,"thread id:%llu\n",std::this_thread::get_id().hash());
    OutputDebugString(str);
  }
}

namespace emulation {
namespace gb {

void Emu::Initialize(double base_freq_hz) {
  InitializeCriticalSection(&cs);
  memset(&timing,0,sizeof(timing));
  set_base_freq_hz(base_freq_hz);
  cartridge_.Initialize(this);
  timer_.Initialize(this);
  lcd_driver_.Initialize(this);
  memory_.Initialize(this);
  cpu_ = new CpuRecompiler();
  cpu_->Initialize(this);
  apu_.Initialize(this);
  thread = null;
  state = 0;
  cpu_cycles_ = 0;
  last_cpu_cycles_ = 0;
  mode_ = EmuModeGB;
  speed = 1;
}

void Emu::Deinitialize() {
  Stop();
  apu_.Deinitialize();
  cpu_->Deinitialize();
  SafeDelete(&cpu_);
  memory_.Deinitialize();
  lcd_driver_.Deinitialize();
  timer_.Deinitialize();
  cartridge_.Deinitialize();
  DeleteCriticalSection(&cs);
}

double Emu::Step() {
  //const double dt =  1000.0 / base_freq_hz_;//options.cpu_freq(); 0.00058f;//16.667f;
  timing.current_cycles = utimer.GetCurrentCycles();
  timing.time_span =  (timing.current_cycles - timing.prev_cycles) * utimer.resolution();
  if (timing.time_span > 500.0) //clamping time
    timing.time_span = 500.0;

  timing.span_accumulator += timing.time_span;
  cartridge()->MBCStep(timing.time_span);
  while (timing.span_accumulator >= timing.step_dt) {
    
    cpu_cycles_per_step_ = 0;
    cpu_->Step();
    timing.span_accumulator -= timing.step_dt*cpu_cycles_per_step_;
    cpu_cycles_ += cpu_cycles_per_step_;
  }

  timing.misc_time_span += timing.time_span;
  if (timing.misc_time_span >= 1000.0) {
    cycles_per_second_ = uint64_t((cpu_cycles_ - last_cpu_cycles_)*(timing.misc_time_span/1000.0));
    frequency_mhz_ = double( cycles_per_second_ ) * 0.000001f;
    last_cpu_cycles_ = cpu_cycles_;
    timing.misc_time_span -= 1000.0;
  }

  timing.total_cycles += timing.current_cycles-timing.prev_cycles;
  timing.prev_cycles = timing.current_cycles;
  timing.fps_time_span += timing.time_span;
  return timing.span_accumulator;
}

void Emu::Run() {
  if (thread!=nullptr && state == 1) return;
  state = 1;
  frequency_mhz_ = 0;
  cycles_per_second_ = 0;
  cpu_cycles_ = 0;
  last_cpu_cycles_ = 0;
  thread = new std::thread(Emu::thread_func,this);
  SetThreadAffinityMask(thread->native_handle(),1);
}

void Emu::Stop() {
  if (thread==nullptr && state == 0) return;
  EnterCriticalSection(&cs);
  state = 0;
  LeaveCriticalSection(&cs);
  printThreadId();
  //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
  cpu_->Reset();
  apu_.Reset();
  //Run();
}

void Emu::Render() {
  ++timing.fps_counter;
  on_render();
  if (timing.fps_time_span >= 1000.0) {
    timing.fps = timing.fps_counter * (1000.0/timing.fps_time_span);
    timing.fps_counter = 0;
    timing.fps_time_span = 0;
  }
}



void Emu::thread_func(Emu* emu) {
  //auto output_ = new audio::output::WASAPI();
  //output_->set_window_handle(app::Application::Current()->display_window().handle());
  //output_->set_buffer_size(4*44100*2);
  //output_->Initialize(44100,2,16);
  //output_->Play();
  //WASAPI_Initialize(44100,2,16);
  //emu->apu_.set_output(output_);
  memset(&emu->timing,0,sizeof(emu->timing));
  emu->utimer.Calibrate();
  emu->timing.prev_cycles = emu->utimer.GetCurrentCycles();
  emu->set_base_freq_hz(emu->base_freq_hz_);
  printThreadId();
  
  while (emu->state==1) {
    emu->Step();
  }
  //output_->Stop();
  //output_->Deinitialize(); 
  //SafeDelete(&output_);
  OutputDebugString("end of thread\n");
}



}
}
   