#pragma once

#include <atomic>
#include <thread>
#include "../utilities/timer.h"

namespace emulation {
 
enum class State { Running = 1, Stopped = 0 };

class Emulator {
public:
  Emulator():state_(State::Stopped){

  }
  virtual ~Emulator() {

  }
  virtual int Start();
  virtual int Stop();
  virtual int Reset();
  virtual int Step() = 0;
protected:
  std::atomic<State> state_;
  std::thread thread_;
  utilities::Timer timer_;
  static void thread_function(Emulator*);
};


}