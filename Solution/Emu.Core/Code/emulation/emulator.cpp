#include "emulator.h"

namespace emulation {

int Emulator::Start() {
  //if (cartridge_.cart_loaded() == false) return;
  if ( state_ != State::Stopped) return 0;

 // frequency_mhz_ = 0;
 // cycles_per_second_ = 0;
 // cpu_cycles_ = 0;
 // last_cpu_cycles_ = 0;

  state_ = State::Running;
  thread_ = std::thread(Emulator::thread_function, this);
  return 1;
}

int Emulator::Stop() {
  if ( state_ != State::Running) return 0;
 
  {
    //std::unique_lock<std::mutex> lk(m);
    state_ = State::Stopped;
    //OutputDebugString("set  thread state = 0\n");
    //
    //cv.wait(lk, [this] {return state==0; });
  }

 
  if (thread_.joinable()) {
    thread_.join();
  } else {
    return 0;
  }

  return 1;
}

int Emulator::Reset() {

  return 1;
}


void Emulator::thread_function(Emulator* self) {
 

    //memset(&emu->timing, 0, sizeof(emu->timing));
    self->timer_.Calibrate();
    //emu->timing.prev_cycles = emu->utimer.GetCurrentCycles();
    //emu->set_base_freq_hz(emu->base_freq_hz_);
    //printThreadId();

    while (self->state_ == State::Running) {
      self->Step();
    }


}

}