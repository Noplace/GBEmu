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

#include "../../audio/output/wasapi.h"
#include "../../audio/output/directsound.h"
#include "../../audio/synth/quadrangular_wave.h"

namespace emulation {
namespace gb {

const uint8_t dutycycletable[32] = {
  0,0,0,0,1,0,0,0,
  0,0,0,0,1,1,0,0,
  0,0,1,1,1,1,0,0,
  1,1,1,1,0,0,1,1,
};

union SweepRegister {
 struct {
   uint8_t sweep_shift:3;
   uint8_t incdec:1;
   uint8_t sweep_time:3;
   uint8_t unused:1;
 };
 uint8_t raw;
};
  

union SoundLengthWaveDutyRegister {
 struct {
   uint8_t soundlength:6;
   uint8_t waveduty:2;
 };
 uint8_t raw;

 real_t soundlength_ms() {
   return 1000.0f*(64-soundlength)*(1.0f/256.0f) ;
 }
};

union VolumeEnvelope {
 struct {
   uint8_t env_sweep:3;
   uint8_t direction:1;
   uint8_t vol:4;
 };
 uint8_t raw;
};	

union SoundChannelControlVolumeRegister {
 struct {
   uint8_t so1vol:3;
   uint8_t so1vinen:1;
   uint8_t so2vol:3;
   uint8_t so2vinen:1;
 };
 uint8_t raw;
};
  
union SoundOutputTerminalRegister {
 struct {
   uint8_t ch1so1:1;
   uint8_t ch2so1:1;
   uint8_t ch3so1:1;
   uint8_t ch4so1:1;
   uint8_t ch1so2:1;
   uint8_t ch2so2:1;
   uint8_t ch3so2:1;
   uint8_t ch4so2:1;
 };
 uint8_t raw;
};

class WaveramSynth : public audio::synth::Wavetable<5> {
 public:
  WaveramSynth(): Wavetable() {
    table = new real_t[kIndexRange+1];
  }

  ~WaveramSynth() {
    delete [] table;
  }
  float* waveram() { return table; }
 private:
   
};

class Apu : public Component {
 public:

  Apu():ioports(nullptr) {}
  ~Apu() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  void Step(double dt);
  uint8_t Read(uint16_t address);
  void  Write(uint16_t address, uint8_t data);
  audio::output::Interface* output() {
    return output_;
  }
 private:
  uint8_t* ioports;
  audio::output::Interface* output_;
  uint32_t sample_counter;
  uint32_t sample_ratio;
  audio::synth::NoiseSynth noise;
  SweepRegister nr10_;
  SoundLengthWaveDutyRegister nr11_;
  VolumeEnvelope nr12_;
  uint8_t nr13_,nr14_;

  SoundLengthWaveDutyRegister nr21_;
  VolumeEnvelope nr22_;
  uint8_t nr23_,nr24_;

  uint8_t nr30_,nr31_,nr32_,nr33_,nr34_;
  uint8_t wavram[32];
  SoundLengthWaveDutyRegister nr41_;
  VolumeEnvelope nr42_;
  uint8_t nr43_,nr44_;
  uint32_t maincounter;
  uint32_t ulencounterclock;

  struct {
    Apu* apu_;
    uint16_t freqcounterload,freqcounter,sweepfreqcounter;
    VolumeEnvelope envelope;
    uint8_t wavepatternduty,wavepatterncounter;
    uint8_t lengthcounterload,lengthcounter;
    uint8_t sweepcounter;
    uint32_t envcounter,envcounterload; 

    void Initialize(Apu* sc) {
      apu_ = sc;
      wavepatterncounter = 0;
      wavepatternduty = 0;
      freqcounter = 0;
      sample = 0;
      envelope.raw = 0;
      sweepcounter = 0;
      envcounter = envcounterload = 0;
    }

    void EnvelopeTick() {
      if (envelope.env_sweep != 0 && --envcounter == 0) {
        if (envelope.direction && envelope.vol<0xF)
          ++envelope.vol;
        if (!envelope.direction && envelope.vol>0)
          --envelope.vol;
        envcounter = envcounterload;
      }
    }

    void SweepTick() {
      if (apu_->nr10_.sweep_time != 0 && --sweepcounter == 0) {
        sweepcounter = apu_->nr10_.sweep_time;
        if (apu_->nr10_.incdec) //could be reversed but doubt it
          freqcounterload += sweepfreqcounter;
        else
          freqcounterload -= sweepfreqcounter;
      }
    }

    void LengthTick() {
      if (lengthcounter!=0)
        if ((apu_->nr14_ & 0x40) && --lengthcounter == 0) {
          //apu_->nr14_ &= ~0x80;
        }
    }

    uint8_t sample;
    uint8_t SampleTick() {
      if ((apu_->nr14_ & 0x80)&&--freqcounter == 0) {
        sample = dutycycletable[wavepatternduty|wavepatterncounter];
        wavepatterncounter = (wavepatterncounter +1 ) % 8;
        freqcounter = freqcounterload;
      }
      return sample;

    }
  }channel1;

  struct {
    VolumeEnvelope envelope;
    uint8_t wavepatternduty,wavepatterncounter;
    Apu* apu_;
    uint16_t freqcounterload,freqcounter;
    uint8_t lengthcounterload,lengthcounter;
    uint32_t envcounter,envcounterload; 
    void Initialize(Apu* sc) {
      apu_ = sc;
      wavepatterncounter = 0;
      wavepatternduty = 0;
      freqcounter = 0;
      sample = 0;
      envelope.raw = 0;
      envcounter = envcounterload = 0;
    }

    void EnvelopeTick() {
      if (envelope.env_sweep != 0 && --envcounter == 0) {
        if (envelope.direction && envelope.vol<0xF)
          ++envelope.vol;
        if (!envelope.direction && envelope.vol>0)
          --envelope.vol;
        envcounter = envcounterload;
      }
    }

    void LengthTick() {
      if (lengthcounter!=0)
        if ((apu_->nr24_ & 0x40) && --lengthcounter == 0) {
          //apu_->nr24_ &= ~0x80;
        }
    }

    uint8_t sample;
    uint8_t SampleTick() {
      if ((apu_->nr24_ & 0x80)&&--freqcounter == 0) {
        sample = dutycycletable[wavepatternduty|wavepatterncounter];
        wavepatterncounter = (wavepatterncounter +1 ) % 8;
        freqcounter = freqcounterload;
      }
      return sample;
    }
  }channel2;

  struct {    
    Apu* apu_;
    real_t vol;
    uint8_t playback_counter;
    real_t wavsample;

    uint16_t freqcounter,freqcounterload;
    uint8_t lengthcounterload,lengthcounter;
    uint8_t sample;
    bool enabled;
    void Initialize(Apu* sc) {
      playback_counter = 0;
      freqcounter=0;
      freqcounterload =0;

      vol=0;
      lengthcounter=0;
      apu_ = sc;
      wavsample = 0;
      enabled = false;
      freqcounter = 0;
      sample = 0;
    }

    void LengthTick() {
      if (lengthcounter!=0)
        if ((apu_->nr34_ & 0x40) && --lengthcounter == 0) {
          //apu_->nr34_ &= ~0x80;
          enabled = false;
        }
    }

    uint8_t SampleTick() {
      if (!enabled) return 0;
      if ((apu_->nr24_ & 0x80)&&--freqcounter == 0) {
        sample = apu_->wavram[playback_counter];
        playback_counter = (playback_counter + 1) & 0x1F;
        freqcounter = freqcounterload;
      }
      return sample;
    }

  } channel3;

  struct {
    Apu* apu_;
    VolumeEnvelope envelope;
    uint32_t envcounter,envcounterload; 
    uint8_t lengthcounterload,lengthcounter;
    uint8_t sample;
    uint16_t shiftreg;
    void Initialize(Apu* sc) {
      apu_ = sc;
      envelope.raw = 0;
      envcounter = envcounterload = 0;
      lengthcounter = lengthcounterload = 0;
      shiftreg = 1;
    }

    void EnvelopeTick() {
      if (envelope.env_sweep != 0 && --envcounter == 0) {
        if (envelope.direction && envelope.vol<0xF)
          ++envelope.vol;
        if (!envelope.direction && envelope.vol>0)
          --envelope.vol;
        envcounter = envcounterload;
      }
    }
    
    void LengthTick() {
      if (lengthcounter!=0)
        if ((apu_->nr44_ & 0x40) && --lengthcounter == 0) {
          //apu_->nr44_ &= ~0x80;
        }
    }

    uint8_t SampleTick() {
      

      real_t noisetimems = 1000.0f/apu_->channel4freq;
      if (apu_->channel4polycounterms >= noisetimems) {
        uint8_t tap = 14 - (apu_->nr43_ & 8);
        const unsigned mask = ~(1u << tap);
        unsigned feedback = shiftreg;
        shiftreg >>= 1;
        feedback = 1 & (feedback ^ shiftreg);
        shiftreg = (feedback << tap) | (shiftreg & mask);

        sample = shiftreg & 1;
        apu_->channel4polycounterms = 0;
      }
      return sample;
    }
  } channel4;
  
  
  real_t channel4freq,channel4polycounterms;
  SoundChannelControlVolumeRegister nr50_;
  SoundOutputTerminalRegister nr51_;
  uint8_t nr52_; 
};

}
}