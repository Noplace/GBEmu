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




std::mutex m;
std::condition_variable cv;
inline void printThreadId() {
  {
    char str[50];
    std::hash<std::thread::id> hasher;
    sprintf_s(str, "thread id:%zx\n", hasher(std::this_thread::get_id()));
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
  cpu_ = new CpuInterpreter();
  cpu_->Initialize(this);
  apu_.Initialize(this);
  disassembler_.Initialize(this);
  thread = null;
  state = EmuState::EmuStateStopped;
  cpu_cycles_ = 0;
  last_cpu_cycles_ = 0;
  mode_ = EmuMode::EmuModeGB;
  speed = EmuSpeed::EmuSpeedNormal;


}

void Emu::Deinitialize() {
  Stop();
  disassembler_.Deinitialize();
  apu_.Deinitialize();
  cpu_->Deinitialize();
  SafeDelete(&cpu_);
  memory_.Deinitialize();
  lcd_driver_.Deinitialize();
  timer_.Deinitialize();
  cartridge_.Deinitialize();
  DeleteCriticalSection(&cs);
}


void Emu::Run() {
  if (cartridge_.cart_loaded() == false) return;
  if (thread!=nullptr && state != EmuState::EmuStateStopped) return;
  //state = 1;
  frequency_mhz_ = 0;
  cycles_per_second_ = 0;
  cpu_cycles_ = 0;
  last_cpu_cycles_ = 0;

  state = EmuState::EmuStateRunning;
  //OutputDebugString("set  thread state = 1\n");
  thread = new std::thread(Emu::thread_func,this);
  //SetThreadAffinityMask(thread->native_handle(),1);


  // send data to the worker thread
  {
    //std::lock_guard<std::mutex> lk(m);
    
    //OutputDebugString("emu thread ready state = 1\n");
  }
  //cv.notify_one();


}

void Emu::Stop() {
  if (thread == nullptr && state != EmuState::EmuStateRunning) return;
  //EnterCriticalSection(&cs);

  //LeaveCriticalSection(&cs);
  //printThreadId();
  //std::this_thread::sleep_for(std::chrono::milliseconds(1000));


  // wait for the worker
  {
    //std::unique_lock<std::mutex> lk(m);
    state = EmuState::EmuStateStopped;
    //OutputDebugString("set  thread state = 0\n");
    //
    //cv.wait(lk, [this] {return state==0; });
  }

  //wait for join signal
  //OutputDebugString("wait for join signal\n");

  //while (state != 2) {
  //  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ///}
  
  if (thread->joinable())   {
    thread->join();
  }
  else  {
    OutputDebugString("cant join thread, what to do\n");
  }
  //OutputDebugString("killed thread\n");
  SafeDelete(&thread);
}

void Emu::Pause() {
  Stop();
}


void Emu::SkipBootROM() {
  //cpu_->cpumode
  //AF = $01B0
    //BC = $0013
    //DE = $00D8
    //HL = $014D
    //Stack Pointer = $FFFE

  
  
    memory_.Write8(0xFF50, 0x11);//observed value GBC
    memory_.Write8(0xFF05, 0x00); //TIMA
    memory_.Write8(0xFF06, 0x00); //TMA
    memory_.Write8(0xFF07, 0x00); //TAC
    memory_.Write8(0xFF10, 0x80); //NR10
    memory_.Write8(0xFF11, 0xBF); //NR11
    memory_.Write8(0xFF12, 0xF3); //NR12
    memory_.Write8(0xFF14, 0xBF); //NR14
    memory_.Write8(0xFF16, 0x3F); //NR21
    memory_.Write8(0xFF17, 0x00); //NR22
    memory_.Write8(0xFF19, 0xBF); //NR24
    memory_.Write8(0xFF1A, 0x7F); //NR30
    memory_.Write8(0xFF1B, 0xFF); //NR31
    memory_.Write8(0xFF1C, 0x9F); //NR32
    memory_.Write8(0xFF1E, 0xBF); //NR33
    memory_.Write8(0xFF20, 0xFF); //NR41
    memory_.Write8(0xFF21, 0x00); //NR42
    memory_.Write8(0xFF22, 0x00); //NR43
    memory_.Write8(0xFF23, 0xBF); //NR30
    memory_.Write8(0xFF24, 0x77); //NR50
    memory_.Write8(0xFF25, 0xF3); //NR51
    memory_.Write8(0xFF26, 0xF1);// -GB, $F0 - SGB; NR52
    memory_.Write8(0xFF40,0x91);//LCDC
    memory_.Write8(0xFF42,0x00);//SCY
    memory_.Write8(0xFF43,0x00);//SCX
    memory_.Write8(0xFF45,0x00);//LYC
    memory_.Write8(0xFF47,0xFC);//BGP
    memory_.Write8(0xFF48,0xFF);//OBP0
    memory_.Write8(0xFF49,0xFF);//OBP1
    memory_.Write8(0xFF4A,0x00);//WY
    memory_.Write8(0xFF4B,0x00);//WX
    memory_.Write8(0xFFFF,0x00);//IE

    cpu_->SkipBootROM();
}

void Emu::Reset() {
  Stop();
  timer_.Reset();
  lcd_driver_.Reset();
  memory_.Reset();
  cpu_->Reset();
  apu_.Reset();
  cpu_cycles_ = 0;
  last_cpu_cycles_ = 0;
  speed = EmuSpeed::EmuSpeedNormal;
  set_base_freq_hz(default_gb_hz * 1.0);


  SkipBootROM();
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


double Emu::Step() {
  //const double dt =  1000.0 / base_freq_hz_;//options.cpu_freq(); 0.00058f;//16.667f;
  timing.current_cycles = utimer.GetCurrentCycles();
  timing.time_span = (timing.current_cycles - timing.prev_cycles) * utimer.resolution();
  if (timing.time_span > 500.0) //clamping time
    timing.time_span = 500.0;

  timing.span_accumulator += timing.time_span;
  cartridge()->MBCStep(timing.time_span);
  while (timing.span_accumulator >= timing.step_dt) {
  
    cpu_cycles_per_step_ = 0;
    cpu_->Step();
    timing.span_accumulator -= timing.step_dt * cpu_cycles_per_step_;
    cpu_cycles_ += cpu_cycles_per_step_;
  }

  timing.misc_time_span += timing.time_span;
  if (timing.misc_time_span >= 1000.0) {
    cycles_per_second_ = uint64_t((cpu_cycles_ - last_cpu_cycles_) * (timing.misc_time_span / 1000.0));
    frequency_mhz_ = double(cycles_per_second_) * 0.000001f;
    last_cpu_cycles_ = cpu_cycles_;
    timing.misc_time_span -= 1000.0;
  }

  timing.total_cycles += timing.current_cycles - timing.prev_cycles;
  timing.prev_cycles = timing.current_cycles;
  timing.fps_time_span += timing.time_span;
  return timing.span_accumulator;
}

void Emu::thread_func(Emu* emu) {
  //auto output_ = new audio::output::WASAPI();
  //output_->set_window_handle(app::Application::Current()->display_window().handle());
  //output_->set_buffer_size(4*44100*2);
  //output_->Initialize(44100,2,16);
  //output_->Play();
  //WASAPI_Initialize(44100,2,16);
  //emu->apu_.set_output(output_);
  
  //  std::unique_lock<std::mutex> lk(m);
  //  cv.wait(lk, [&emu] {return emu->state == 1; });
   // lk.unlock();
   // cv.notify_one();
  

  memset(&emu->timing,0,sizeof(emu->timing));
  emu->utimer.Calibrate();
  emu->timing.prev_cycles = emu->utimer.GetCurrentCycles();
  emu->set_base_freq_hz(emu->base_freq_hz_);
  //printThreadId();
  
  while (emu->state == EmuState::EmuStateRunning) {
    emu->Step();
  }
  

  //output_->Stop();
  //output_->Deinitialize(); 
  //SafeDelete(&output_);
  //OutputDebugString("end of thread\n");
  //emu->state = 2; //signal to join
  //OutputDebugString("set  thread state = 2\n");
}



}
}
   