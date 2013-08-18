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

#include <WinCore/types.h>
#include <WinCore/windows/windows.h>

typedef float real_t;
#define PI  3.14159265358979323846f

namespace audio {
namespace synth {


template<typename T1,typename T2>
union AnyCast{
  T1 in;
  T2 out;
};

typedef AnyCast<uint32_t,real_t> UIntToFloatCast;


inline real_t bits2float(uint32_t u) {
  UIntToFloatCast x;
  x.in = u;
  return x.out;
}

inline uint32_t RandomInt(uint32_t* seed) {
  *seed = *seed * 196314165 + 907633515;
  return *seed;
}

// uniform random float in [-1,1)
inline real_t RandomFloat(uint32_t* seed) {
  uint32_t bits = RandomInt(seed); // random 32-bit value
  real_t f = bits2float((bits >> 9) | 0x40000000); // random float in [2,4)
  return f - 3.0f; // uniform random float in [-1,1)
}

inline real_t Noise(uint32_t* seed) {
  float r1 = (1+RandomFloat(seed))*0.5f;
  float r2 = (1+RandomFloat(seed))*0.5f;
  return (float) sqrtf( -2.0f * logf(r1)) * cosf( 2.0f * PI *r2);//white noise
}

extern uint32_t noiseseed;

#define WhiteNoise Noise(&noiseseed)

static real_t HardClip(real_t x) {
  if (x < -1.0f)
    return -1.0f;
  else if (x > 1.0f)
    return 1.0f;
  else
    return x;
}

static real_t SoftClip(real_t x) {
  static const real_t lim = 0.6666666666f;
  if (x < -lim)
    return -lim;
  else if (x > lim)
    return lim;
  else
    return x-((x*x*x)/3.0f);
}

class Component {
 public:
  Component() : sample_rate_(0),sample_time_ms_(0) {}
  virtual ~Component() {}
  void set_sample_rate(uint32_t sample_rate) { 
    sample_rate_ = sample_rate; 
    sample_time_ms_ = (1000.0f/sample_rate_);
  }
  real_t sample_time_ms_;
  uint32_t sample_rate_;
};

template<int kIndex=8,int kPrecision=24>
class Wavetable : public Component {
 public:
  const int kPrecisionMask;
  const int kIndexRange;
  const real_t kPrecisionRange;
  const real_t kPrecisionRangeInv;

  Wavetable() : Component(),
    kPrecisionMask((1<<kPrecision)-1),
    kIndexRange(1<<kIndex),
    kPrecisionRange((real_t)(1<<kPrecision)),
    kPrecisionRangeInv(1.0f/(kPrecisionRange)){
   //kPrecisionMask = ((1<<kPrecision)-1);
   //kIndexRange = (1<<kIndex);
   //kPrecisionRange = (real_t)(1<<kPrecision);
      phase = 0;
  }

  virtual ~Wavetable() {

  }

  virtual real_t Tick(uint32_t inc) {
    // the 8 MSB are the index in the table in the range 0-255 
    int i = phase >> kPrecision; 
    // and the kPrecision LSB are the fractionnal part
    real_t frac = (phase & kPrecisionMask) * kPrecisionRangeInv;
    // increment the phase for the next tick
    phase += inc; // the phase overflow itself
    return (table[i]*(1.0f-frac) + table[i+1]*frac); // linear interpolation
  }
  virtual uint32_t get_increment(real_t frequency) {
    return (uint32_t)((kIndexRange * frequency / sample_rate_) * kPrecisionRange);
  }
 protected:
  real_t* table;
  uint32_t phase;
};


class SquareOscillator : public Wavetable<> {
 public:
  SquareOscillator();
  ~SquareOscillator();
 private:
   
};

class NoiseSynth : public Wavetable<> {
 public:
  NoiseSynth();
  ~NoiseSynth();
 private:
   
};



}
}
