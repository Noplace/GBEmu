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
  0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,1,1,1,
  0,1,1,1,1,1,1,0,
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

struct ApuChannel {
  Apu* apu_;
  uint32_t acc_sample_left,acc_sample_right;
  SweepRegister reg0;
  uint8_t reg1,reg2,reg3,reg4;
  uint32_t freqcounterload,freqcounter,sweepfreqcounter;
  VolumeEnvelope envelope;
  uint8_t wavepatternduty,wavepatterncounter;
  uint8_t sweepcounter;
  uint16_t lengthcounter;
  uint32_t envcounter,envcounterload; 
  uint8_t sample;
  int channel;
  bool dac_enable;
  bool enable_length_clock;
  void Initialize(Apu* sc,int channel) {
    apu_ = sc;
    wavepatterncounter = 0;
    wavepatternduty = 0;
    freqcounter = 0;
    sample = 0;
    envelope.raw = 0;
    sweepcounter = 0;
    envcounter = envcounterload = 0;
    this->channel = channel;
    reg0.raw = 0;
    reg1 = reg2 = reg3 = reg4 = 0;
    dac_enable = false;
    lengthcounter = 0;
    acc_sample_left = acc_sample_right = 0;
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

  void LengthTick(uint8_t& nr52) {
    const int flags[4] = {0x01,0x02,0x04,0x08};
    if (lengthcounter!=0)
      if ((reg4 & 0x40) && --lengthcounter == 0) {
        //apu_->nr14_ &= ~0x80;
        nr52 &= ~flags[channel-1];
      }
  }

};

class Apu : public Component {
  friend ApuChannel;
 public:

  Apu() {}
  ~Apu() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  void Tick();
  uint8_t Read(uint16_t address);
  void  Write(uint16_t address, uint8_t data);
  audio::output::Interface* output() {
    return output_;
  }
  void set_output(audio::output::Interface* output) { output_ = output; }
 private:
  short* output_buffer_;
  audio::output::Interface* output_;
  uint32_t sample_counter_;
  uint32_t sample_ratio_;

  /*SweepRegister nr10_;
  SoundLengthWaveDutyRegister nr11_;
  VolumeEnvelope nr12_;
  uint8_t nr13_,nr14_;*/
  uint8_t io_0xff15_;
  /*SoundLengthWaveDutyRegister nr21_;
  VolumeEnvelope nr22_;
  uint8_t nr23_,nr24_;*/

  //uint8_t nr30_,nr31_,nr32_,nr33_,nr34_;
  uint8_t io_0xff1f_;
  /*SoundLengthWaveDutyRegister nr41_;
  VolumeEnvelope nr42_;
  uint8_t nr43_,nr44_;*/
  uint8_t io_0xff27_;
  uint8_t io_0xff28_;
  uint8_t io_0xff29_;
  uint8_t io_0xff2a_;
  uint8_t io_0xff2b_;
  uint8_t io_0xff2c_;
  uint8_t io_0xff2d_;
  uint8_t io_0xff2e_;
  uint8_t io_0xff2f_;
  uint8_t waveram[16];

  uint32_t maincounter;
  uint32_t ulencounterclock;
  uint32_t frame_seq_step,frame_seq_clock;
  uint32_t acc_sample_left,acc_sample_right;
  struct : ApuChannel {

    void SweepTick() {
      if (reg0.sweep_time != 0 && --sweepcounter == 0) {
        if (reg0.sweep_shift == 0) {
          apu_->nr52_ &= ~0x01;
          return;
        }
        sweepcounter = reg0.sweep_time;
        if (reg0.incdec) //could be reversed but doubt it
          freqcounterload += sweepfreqcounter;
        else
          freqcounterload -= sweepfreqcounter;
      }
    }

    uint8_t SampleTick() {
      if (--freqcounter == 0) {
        sample = dutycycletable[wavepatternduty|wavepatterncounter];
        wavepatterncounter = (wavepatterncounter +1 ) % 8;
        freqcounter = freqcounterload;
      }
      if (dac_enable) {
        return sample;
      } else {
        return 0;
      }
      
    }
  }channel1;

  struct : ApuChannel {
    uint8_t sample;
    uint8_t SampleTick() {
      if (--freqcounter == 0) {
        sample = dutycycletable[wavepatternduty|wavepatterncounter];
        wavepatterncounter = (wavepatterncounter +1 ) % 8;
        freqcounter = freqcounterload;
      }
      if (dac_enable) {
        return sample;
      } else {
        return 0;
      }
    }
  }channel2;

  struct : ApuChannel {   
    uint8_t volshift;
    real_t vol;
    uint8_t playback_counter;
    real_t wavsample;
    uint8_t wavedata[32];
    //bool enabled;
    void Initialize(Apu* sc) {
      ApuChannel::Initialize(sc,3);
      playback_counter = 0;
      vol=0;
      wavsample = 0;
      volshift = 0;
    }

    uint8_t SampleTick() {
      if (--freqcounter == 0) {
        sample = wavedata[playback_counter];
        playback_counter = (playback_counter + 1) & 0x1F;
        freqcounter = freqcounterload;
      }
      if (dac_enable) {
        return sample;
      } else {
        return 0;
      }
    }

  } channel3;

  struct :ApuChannel {
    uint16_t shiftreg;
    void Initialize(Apu* sc) {
      ApuChannel::Initialize(sc,4);
      shiftreg = 1;
    }


    uint8_t SampleTick() {

      if (--freqcounter == 0) {
        uint8_t tap = 14 - (reg3 & 8);
        const unsigned mask = ~(1u << tap);
        unsigned feedback = shiftreg;
        shiftreg >>= 1;
        feedback = 1 & (feedback ^ shiftreg);
        shiftreg = (feedback << tap) | (shiftreg & mask);

        sample = shiftreg & 1;
        freqcounter = freqcounterload;
      } 
      if (dac_enable) {
        return sample;
      } else {
        return 0;
      }
      /*
      real_t noisetimems = 1000.0f/apu_->channel4freq;
      if (apu_->channel4polycounterms >= noisetimems) {
        uint8_t tap = 14 - (reg3 & 8);
        const unsigned mask = ~(1u << tap);
        unsigned feedback = shiftreg;
        shiftreg >>= 1;
        feedback = 1 & (feedback ^ shiftreg);
        shiftreg = (feedback << tap) | (shiftreg & mask);

        sample = shiftreg & 1;
        apu_->channel4polycounterms = 0;
      }
      if (!dac_enable) return 0;
      return sample;*/
    }
  } channel4;
  
  void LengthTick() {
    channel1.LengthTick(nr52_);
    channel2.LengthTick(nr52_);
    channel3.LengthTick(nr52_);
    channel4.LengthTick(nr52_);
  }
  
  void EnvelopeTick() {
    channel1.EnvelopeTick();
    channel2.EnvelopeTick();
    channel4.EnvelopeTick();
  }

  //real_t channel4freq,channel4polycounterms;
  SoundChannelControlVolumeRegister nr50_;
  SoundOutputTerminalRegister nr51_;
  uint8_t nr52_; 
};

}
}