#include "quadrangular_wave.h"

namespace audio {
namespace synth {
 
uint32_t noiseseed;

SquareOscillator::SquareOscillator() : Wavetable() {
  table = new real_t[kIndexRange+1];
  int div2 = kIndexRange >> 1;
  int i;
  for(i=0;i<div2;++i) {
	  table[i]     =  1.0f;
	  table[i+div2] = -1.0f;
  }
  table[kIndexRange] = 1.0f;
}

SquareOscillator::~SquareOscillator() {
  delete [] table;
  table = nullptr;
}


  
NoiseSynth::NoiseSynth() : Wavetable() {
  table = new real_t[kIndexRange+1];
  int i;
  for(i=0;i<=kIndexRange;++i) {
    table[i] = HardClip(WhiteNoise);
  }
}

NoiseSynth::~NoiseSynth() {
  delete [] table;
  table = nullptr;
}

}
}