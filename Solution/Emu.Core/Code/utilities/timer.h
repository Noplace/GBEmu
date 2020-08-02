/*****************************************************************************************************************
* Copyright (c) 2012 Khalid Ali Al-Kooheji                                                                       *
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

#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace utilities {

template<typename T>
inline T clamp(T x, T a, T b)   {   
  return x < a ? a : (x > b ? b : x);
}

class Timer {
 public:
  Timer():time_slice_(0) {
    Calibrate();
  }
  void Calibrate() {
    start_cycles_ = GetCurrentCycles();
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency_);
    resolution_ = 1000.0 / frequency_;
    resolution_ns_ = 1000000 / frequency_;
  }

  double time_slice() { return time_slice_; }
  uint64_t elapsed_ticks() { return elapsed_ticks_; }
  __forceinline double resolution() { return resolution_; }
  void Tick() {
    uint64_t elapsed_cycles = GetCurrentCycles() - start_cycles_;
    start_cycles_ = GetCurrentCycles();
    elapsed_ticks_ = elapsed_cycles;
    time_slice_ = (1.0 / 1000.0) * (double)elapsed_ticks_;
  }

  bool isTimeForUpdate(int framerate)
  {
      uint64_t actual_ticks = GetCurrentCycles() - start_cycles_;
      actual_ticks = clamp<uint64_t>(actual_ticks,(uint64_t)0,(uint64_t)(frequency_ / 20.0));
      if (actual_ticks >= (uint64_t)(frequency_ / framerate   )) /// (1000.0 / (T)framerate)
      {
        Tick();
        return true;
      }
      else
      {
        return false;
      }
  }

  __forceinline uint64_t GetCurrentCycles() {
    QueryPerformanceCounter((LARGE_INTEGER*)&current_cycles_);
    return current_cycles_; 
  }

 private:
  uint64_t elapsed_ticks_;
  uint64_t frequency_;
  uint64_t current_cycles_;
  uint64_t start_cycles_;
  uint64_t resolution_ns_;
  double time_slice_;
  double resolution_;
};

}

