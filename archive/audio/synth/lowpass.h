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

#define _USE_MATH_DEFINES
#include <math.h>

namespace audio {
namespace synth {
namespace filters {

class Filter {
 public:
  virtual int Initialize() = 0;
  virtual int Update() = 0;
  virtual float Tick(float) = 0;
};


class LowPassFilter : public Filter {
 public:  
  LowPassFilter():y1(0),a0(0),b1(0),cutoff_freq_(0) {

  }

  int Initialize() {
    y1=0;
    Update();
    return 0;
  }

  int Update() {
    float fc = (cutoff_freq_)/(sample_rate_);
    float x = expf(float(-2*M_PI*fc));
    a0 = 1 - x;
    b1 = x;
    return 0;
  }

  float Tick(float sample) {
    float y = a0*sample+b1*y1;
    y1 = y;
    return y;
  }

  void set_cutoff_freq(float cutoff_freq) { cutoff_freq_ = cutoff_freq; }
  void set_sample_rate(int sample_rate) { sample_rate_ = sample_rate; }
 protected:
  float a0,b1,cutoff_freq_;
  float y1;
  int sample_rate_;

};


}
}
}
