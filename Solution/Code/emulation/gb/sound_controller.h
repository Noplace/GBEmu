#pragma once

#include "../../audio/directsound.h"
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

class SoundController : public Component {
 public:

  SoundController():ioports(nullptr) {}
  ~SoundController() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  void Step(double dt);
  uint8_t Read(uint16_t address);
  void  Write(uint16_t address, uint8_t data);
 private:
	uint8_t* ioports;
  audio::output::Interface* output;
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
  uint8_t maincounter,ulencounterclock;

  struct {
    SoundController* sc_;
    uint16_t freqcounterload,freqcounter,sweepfreqcounter;
		VolumeEnvelope envelope;
    uint8_t wavepatternduty,wavepatterncounter;
    uint8_t lengthcounterload,lengthcounter;
		uint8_t sweepcounter;
    void Initialize(SoundController* sc) {
      sc_ = sc;
      wavepatterncounter = 0;
      wavepatternduty = 0;
      freqcounter = 0;
      sample = 0;
			envelope.raw = 0;
			sweepcounter = 0;
    }

		void SweepTick() {
			if (sc_->nr10_.sweep_time != 0 && sweepcounter-- == 0) {
				sweepfreqcounter >>= sc_->nr10_.sweep_shift;
			  sweepcounter = sc_->nr10_.sweep_time;
				if (sc_->nr10_.incdec)
					freqcounter += sweepfreqcounter;
				else
					freqcounter -= sweepfreqcounter;
			}
		}

		void LengthTick() {
			if ((sc_->nr14_ & 0x40) && lengthcounter-- == 0) {
				sc_->nr14_ &= ~0x80;
			}
		}

    uint8_t sample;
    uint8_t SampleTick() {
			if ((sc_->nr14_ & 0x80)&&freqcounter-- == 0) {
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
    SoundController* sc_;
    uint16_t freqcounterload,freqcounter;
    uint8_t lengthcounterload,lengthcounter;
    void Initialize(SoundController* sc) {
      sc_ = sc;
      wavepatterncounter = 0;
      wavepatternduty = 0;
      freqcounter = 0;
      sample = 0;
			envelope.raw = 0;
    }

		void LengthTick() {
			if ((sc_->nr24_ & 0x40) && lengthcounter-- == 0) {
				sc_->nr24_ &= ~0x80;
			}
		}

    uint8_t sample;
    uint8_t SampleTick() {
			if ((sc_->nr24_ & 0x80)&&freqcounter-- == 0) {
				sample = dutycycletable[wavepatternduty|wavepatterncounter];
				wavepatterncounter = (wavepatterncounter +1 ) % 8;
				freqcounter = freqcounterload;
			}
      return sample;
    }
  }channel2;

  struct {
		VolumeEnvelope envelope;
    SoundController* sc_;
    real_t vol;
    uint8_t playback_counter;
    real_t wavsample;

    uint16_t freqcounter,freqcounterload;
    uint8_t lengthcounterload,lengthcounter;
		uint8_t sample;
    bool enabled;
    void Initialize(SoundController* sc) {
      playback_counter = 0;
      freqcounter=0;
			freqcounterload =0;
			vol=0;
			lengthcounter=0;
      sc_ = sc;
      wavsample = 0;
      enabled = false;
      freqcounter = 0;
      sample = 0;
			envelope.raw = 0;
    }

		void LengthTick() {
			if ((sc_->nr34_ & 0x40) && lengthcounter-- == 0) {
				sc_->nr34_ &= ~0x80;
				enabled = false;
			}
		}

    uint8_t SampleTick() {
			if (!enabled) return 0;
			if ((sc_->nr24_ & 0x80)&&freqcounter-- == 0) {
				sample = sc_->wavram[playback_counter];
				playback_counter = (playback_counter + 1) & 0x1F;
				freqcounter = freqcounterload;
			}
      return sample;
    }

        /*
    real_t Sample() {
      if (!enabled) return 0;
      wavsample = (float(sc_->wavram[playback_counter])) / 15.0f;
      wavsample = (wavsample*2.0f) - 1;
      //wavsample /= samplecount;
      //samplecount = 0;
      auto result = (wavsample) * vol;
      wavsample = 0;
      return result;
    }
    void Test();*/
  } channel3;

	struct {
		SoundController* sc_;
		VolumeEnvelope envelope;
    void Initialize(SoundController* sc) {
      sc_ = sc;
			envelope.raw = 0;
    }
	} channel4;
  
  real_t channel1envcounterms;
  real_t channel2envcounterms;
  
  real_t channel4freq,channel4envcounterms,channel4polycounterms,channel4soundlengthcounter;
  SoundChannelControlVolumeRegister nr50_;
  SoundOutputTerminalRegister nr51_;
  uint8_t nr52_; 
};

}
}