#include "gb.h"




namespace emulation {
namespace gb {



void SoundController::Initialize(Emu* emu) {
  Component::Initialize(emu);
	ioports = emu_->memory()->ioports();
  output = new audio::output::DirectSound();
  output->Initialize(44100,2,16);
  sample_counter = 0;
  sample_ratio = uint32_t(clockspeed / 44100);
  osc1.set_sample_rate(44100);
  osc2.set_sample_rate(44100);
  noise.set_sample_rate(44100);
  nr50_.raw = 0;
  nr51_.raw = 0;
  nr52_ = 0;
  maincounter = 0;
  div256 = 0;
  time_t t;
  time(&t);
  srand((int)t);

  audio::synth::noiseseed = rand();
  Reset();
}

void SoundController::Reset() {
 channel1freq = 0;
  channel1sweepcounterms = 0;
  channel1freqsweep = 0;
  channel1envcounterms = 0;
  channel1soundlengthcounter = 0;
  channel2freq = 0;
  channel2envcounterms = 0;
  channel2soundlengthcounter = 0;
  
  channel4freq = 0;
  channel4envcounterms = 0;
  channel4polycounterms = 0;
  channel4soundlengthcounter = 0;
  channel1env.raw = 0;
  channel2env.raw = 0;
  channel4env.raw = 0;
 
  channel1.Initialize(this);
  channel3.Initialize(this);
  
  nr10_.raw = 0;
  nr11_.raw = 0;
  nr12_.raw = 0;
  nr13_ = 0;
  nr14_ = 0;
  nr21_.raw = 0;
  nr22_.raw = 0;
  nr23_ = 0;
  nr24_ = 0;
  nr30_ = nr31_ = nr32_ = nr33_ = nr34_ = 0;
  memset(wavram,0,sizeof(wavram));

  nr41_.raw = 0;
  nr42_.raw = 0;
  nr43_ = 0;
  nr44_ = 0;
}

void SoundController::Deinitialize() {
  output->Deinitialize(); 
  SafeDelete(&output);
}

void SoundController::Step(double dt) {
  if ((nr52_&0x80)==0)
    return;


  auto envelopeTick = [](VolumeEnvelope& envreg,real_t& counter){
    float env_step_ms = envreg.env_sweep/64.0f*1000.0f;

    if (env_step_ms != 0 && counter >= env_step_ms) {
        if (envreg.direction && envreg.vol<0xF)
          ++envreg.vol;
        if (!envreg.direction && envreg.vol>0)
          --envreg.vol;
      counter = 0;
    }
  };

  if (maincounter == 31) {
    if ((nr14_ & 0x80)&&channel1.freqcounter-- == 0) {
      channel1.Sample();
      channel1.freqcounter = channel1.freqcounterload;
    }
  }
  maincounter = (maincounter + 1) & 0x1F;

  if (div256++ == 255) {
    if ((nr14_ & 0x40) && channel1.lengthcounter-- == 0) {
      //nr14_ &= ~0x80;
    }
  }

  //channel1
  //if (nr14_ & 0x80)
  {
    channel1sweepcounterms += float(dt);
    real_t sweep_time_ms = nr10_.sweep_time *7.8f;
    if (channel1sweepcounterms >= sweep_time_ms) {
      channel1freq = channel1freq + channel1freqsweep;
      channel1sweepcounterms = 0;
    }
    //X(t) = X(t-1) +/- X(t-1)/2^n

    channel1envcounterms += float(dt);
    envelopeTick(channel1env,channel1envcounterms);

    channel1soundlengthcounter += float(dt);
    if (nr14_ & 0x40) {
      if (channel1soundlengthcounter >= nr11_.soundlength_ms()) {
        nr14_ &= ~0x80;
      }
    }
  }

  //channel2
  //if (nr24_ & 0x80)
  {
    channel2envcounterms += float(dt);
    envelopeTick(channel2env,channel2envcounterms);

    channel2soundlengthcounter += float(dt);
    if (nr24_ & 0x40) {
      if (channel2soundlengthcounter >= nr21_.soundlength_ms()) {
        nr24_ &= ~0x80;
      }
    }
  }
  
  //channel3
  //real_t wavsample = 0;
 // static int wavindex = 0;
  {
    //static real_t wavsamplecount = 0;
    channel3.Tick(float(dt));

  }

  //channel4
 // real_t noisesample = 0;
  //if (nr44_ & 0x80)
  {

    //channel4polycounterms += float(dt);
    //static bool alt = false;
    //real_t noisetimems = 1000.0f/channel4freq;
    //if (channel4polycounterms >= noisetimems) {
      //noisesample = (alt*(rand()%255))/255.0f;
   //   alt = !alt;
   //   channel4polycounterms = 0;
   // }
    channel4envcounterms += float(dt);
    envelopeTick(channel4env,channel4envcounterms);

    channel4soundlengthcounter += float(dt);
    if (nr44_ & 0x40) {
      if (channel4soundlengthcounter >= nr41_.soundlength_ms()) {
        nr44_ &= ~0x80;
      }
    }
  }

  ++sample_counter;
  if (sample_counter >= sample_ratio)
  {

    //
    auto channel1_sample = 0;//osc1.Tick(osc1.get_increment(channel1freq))* (channel1env.vol / 15.0f);//((2.0f*channel1.sample-1.0f) * (channel1env.vol / 15.0f));
    auto channel2_sample = 0;//osc2.Tick(osc2.get_increment(channel2freq)) * (channel2env.vol / 15.0f);

    auto channel3_sample = channel3.Sample(); 
    auto channel4_sample = 0;//noise.Tick(osc2.get_increment(channel4freq)) * (channel4env.vol / 15.0f);
    //wavsamplecount = 0;
    auto sample_left = ((nr51_.ch1so1 * channel1_sample)+(nr51_.ch2so1 * channel2_sample)+(nr51_.ch3so1 * channel3_sample)+(nr51_.ch4so1 * channel4_sample)) * 32767.0f;
    auto sample_right = ((nr51_.ch1so2 * channel1_sample)+(nr51_.ch2so2 * channel2_sample)+(nr51_.ch3so2 * channel3_sample)+(nr51_.ch4so2 * channel4_sample)) * 32767.0f;

    sample_left *= nr50_.so1vol / 7.0f;
    sample_right *= nr50_.so2vol / 7.0f;

    static short sbuf[8820]= {0,0};
    static int sindex = 0;
    sbuf[sindex++] = short(sample_left);
    sbuf[sindex++] = short(sample_right); 
    sample_counter -= sample_ratio;   

    if (sindex == 8820) {
      output->Write(sbuf,8820<<1);
      sindex = 0;
    }
  }

  
}

uint8_t SoundController::Read(uint16_t address) {

  if (address>=0xFF30 && address<=0xFF3F)
    return ioports[address-0xFF10];

  switch (address) {
    case 0xFF10:
      ioports[address-0xFF10] = nr10_.raw;
    case 0xFF11:
      ioports[address-0xFF10] = nr11_.raw;
    case 0xFF12:
      ioports[address-0xFF10] = nr12_.raw;
    case 0xFF13:
      ioports[address-0xFF10] = nr13_;
    case 0xFF14:
      ioports[address-0xFF10] = nr14_;
    case 0xFF16:
      ioports[address-0xFF10] = nr21_.raw;
    case 0xFF17:
      ioports[address-0xFF10] = nr22_.raw;
    case 0xFF18:
      ioports[address-0xFF10] = nr23_;
    case 0xFF19:
      ioports[address-0xFF10] = nr24_;

    case 0xFF1A:
      ioports[address-0xFF10] = nr30_;
    case 0xFF1B:
      ioports[address-0xFF10] = nr31_;
    case 0xFF1C:
      ioports[address-0xFF10] = nr32_;
    case 0xFF1D:
      ioports[address-0xFF10] = nr33_;
    case 0xFF1E:
      ioports[address-0xFF10] = nr34_;

    case 0xFF20:
      ioports[address-0xFF10] = nr41_.raw;
    case 0xFF21:
      ioports[address-0xFF10] = nr42_.raw;
    case 0xFF22:
      ioports[address-0xFF10] = nr43_;
    case 0xFF23:
      ioports[address-0xFF10] = nr44_;

    case 0xFF24:
      ioports[address-0xFF10] = nr50_.raw;
    case 0xFF25:
      ioports[address-0xFF10] = nr51_.raw; 
    case 0xFF26:
      ioports[address-0xFF10] = nr52_; 
  }
	return ioports[address-0xFF10];
}

void SoundController::Write(uint16_t address, uint8_t data) {
  ioports[address-0xFF10] = data;
  if (address>=0xFF30 && address<=0xFF3F) {
     int index = (address & 0x0F)<<1;
     wavram[index] = data>>4;
     wavram[index+1] = data&0xF;
  }

  switch (address) {
    case 0xFF10:
      nr10_.raw = data;
      if (nr10_.sweep_shift == 0)
        channel1freqsweep = 0;
      else
        channel1freqsweep = channel1freq/(powf(2,float(nr10_.sweep_shift)));
      if (nr10_.incdec)
        channel1freqsweep *= -1;
      break;
    case 0xFF11:
      nr11_.raw = data;
      channel1.wavepatternduty = (data&0xC0)>>3;
      channel1.lengthcounterload = 64 - (data&0x3F);
      break;
    case 0xFF12:
      nr12_.raw = data;    
      break;
    case 0xFF13:
      nr13_ = data;
      break;
    case 0xFF14: {
      nr14_ = data;
      uint32_t x = nr13_;
      x |= (nr14_&0x7)<<8;
      channel1.freqcounterload = 2048-x;

      channel1freq = 131072.0f/(2048-x);
      if (nr14_ & 0x80) {
        nr52_ |= 0x01;
        channel1envcounterms = 0;
        channel1sweepcounterms = 0;
        channel1soundlengthcounter = 0;
        channel1env.raw = nr12_.raw;
        channel1.freqcounter = channel1.freqcounterload;
        channel1.lengthcounter = channel1.lengthcounterload;
      }
      break;
    }


    case 0xFF16:
      nr21_.raw = data;
      break;
    case 0xFF17:
      nr22_.raw = data;
      break;
    case 0xFF18:
      nr23_ = data;
      break;
    case 0xFF19: {
      nr24_ = data;
      uint32_t x = nr23_;
      x |= (nr24_&0x7)<<8;
      channel2freq = 131072.0f/(2048-x);
      if (nr24_ & 0x80) {
        nr52_ |= 0x01;
        channel2envcounterms = 0;
        channel2soundlengthcounter = 0;
        channel2env.raw = nr22_.raw;
      }
      break;
    }

    case 0xFF1A:
      nr30_ = data;
      channel3.enabled = (nr30_&0x80)==0x80;
      break;
    case 0xFF1B:
      nr31_ = data;
      break;
    case 0xFF1C:
      nr32_ = data;  
      switch ((nr32_ & 0x60)>>5) {
        case 0x0:channel3.vol = 0; break;
        case 0x1:channel3.vol = 1.0f; break;
        case 0x2:channel3.vol = 0.5f; break;
        case 0x3:channel3.vol = 0.25f; break;
      }
      break;
    case 0xFF1D:
      nr33_ = data;
      break;
    case 0xFF1E: {
      nr34_ = data;
      uint32_t x = nr33_;
      x |= (nr34_&0x7)<<8;
      channel3.freq = 131072.0f/(2048-x);
      if (nr34_&0x40)
        channel3.soundlength_ms = 1000.0f * (256.0f-nr31_)*(1/256.0f);
      if ((nr34_&0x80)&&(nr30_&0x80)) {
        nr52_ |= 0x04;
        channel3.soundlengthcounter = 0;
        channel3.freqcounter = 0;
        channel3.playback_counter = 0;
        channel3.enabled = true;
        memset(&ioports[0x30],0,16);
      }
      break;
    }

    case 0xFF20:
      nr41_.raw = data;
      break;
    case 0xFF21:
      nr42_.raw = data;
      break;
    case 0xFF22:
      nr43_ = data;
      break;
    case 0xFF23: {
      nr44_ = data;
      uint32_t x = nr43_;
      x |= (nr44_&0x7)<<8;
      float r = float(nr43_&0x7);
      float s = float((nr43_&0xF0)>>4);
      channel4freq = 524288.0f/r/powf(2.0,s+1);
      if (nr24_ & 0x80) {
        nr52_ |= 0x08;
        channel4envcounterms = 0;
        channel4polycounterms = 0;
        channel4soundlengthcounter = 0;
        channel4env.raw = nr42_.raw;
      }
      break;
    }

    case 0xFF24:
      nr50_.raw = data;
      break;
    case 0xFF25:
      nr51_.raw = data;
      break;
    case 0xFF26:
      nr52_ = (data&0xF0) | (nr52_&0x0F);
      if ((nr52_&0x80)==0)
        Reset();
      break;
 
  }
}


}
}