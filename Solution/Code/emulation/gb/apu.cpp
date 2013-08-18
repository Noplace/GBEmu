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
#include "gb.h"

namespace emulation {
namespace gb {



void Apu::Initialize(Emu* emu) {
  Component::Initialize(emu);
	ioports = emu_->memory()->ioports();
  output = new audio::output::DirectSound();
  output->Initialize(44100,2,16);
  //WASAPI_Initialize(44100,2,16);

  sample_counter = 0;
  sample_ratio = uint32_t(clockspeed / 44100);
  noise.set_sample_rate(44100);
  maincounter = 0;
  ulencounterclock = 0;
  time_t t;
  time(&t);
  srand((int)t);

  audio::synth::noiseseed = rand();
  Reset();
}

void Apu::Reset() {
  
  channel4freq = 0;
  channel4polycounterms = 0;
 
  channel1.Initialize(this);
	channel2.Initialize(this);
  channel3.Initialize(this);
	channel4.Initialize(this);
  
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
  nr50_.raw = 0;
  nr51_.raw = 0;
  nr52_ = 0;
  nr52_ = 0x10;
  
  Write(0xFF10,0x80);
  Write(0xFF11,0x3F);
  Write(0xFF12,0x00);
  Write(0xFF13,0xFF);
  Write(0xFF14,0xBF);
  Write(0xFF15,0xFF);
  Write(0xFF16,0x3F);
  Write(0xFF17,0x00);
  Write(0xFF18,0xFF);
  Write(0xFF19,0xBF);
  Write(0xFF1A,0x7F);
  Write(0xFF1B,0xFF);
  Write(0xFF1C,0x9F);
  Write(0xFF1D,0xFF);
  Write(0xFF1E,0xBF);
  Write(0xFF1F,0xFF);
  Write(0xFF20,0xFF);
  Write(0xFF21,0x00);
  Write(0xFF22,0x00);
  Write(0xFF23,0xBF);
  Write(0xFF24,0x00);
  Write(0xFF25,0x00);
  //Write(0xFF26,nr52_&0x80?0xF0:0x70);
  Write(0xFF27,0xFF);
  Write(0xFF28,0xFF);
  Write(0xFF29,0xFF);
  Write(0xFF2A,0xFF);
  Write(0xFF2B,0xFF);
  Write(0xFF2C,0xFF);
  Write(0xFF2D,0xFF);
  Write(0xFF2E,0xFF);
  Write(0xFF2F,0xFF);

  {
    const uint8_t wavraminitial[16]={0x84,0x40,0x43,0xAA,0x2D,0x78,0x92,0x3C,0x60,0x59,0x59,0xB0,0x34,0xB8,0x2E,0xDA};
    for (int i=0;i<16;++i)
      Write(0xFF30+i,wavraminitial[i]);
  }
}

void Apu::Deinitialize() {
  output->Deinitialize(); 
  SafeDelete(&output);
  //WASAPI_Deinitialize();
}

void Apu::Step(double dt) {
  if ((nr52_&0x80)==0)
    return;

  if ((maincounter & 0x3) == 0) {// its like every 32/8 because 8 samples in duty cycle
      channel1.SampleTick();
			channel2.SampleTick();
  }

	if ((maincounter & 0x1) == 0) {
      channel3.SampleTick();
  }
  channel4polycounterms += float(dt);
  channel4.SampleTick();

	maincounter = (maincounter+1) & 0x1F;

  if (ulencounterclock++ == 16384) { //256hz from original cpu speed
		channel1.LengthTick();
		channel2.LengthTick();
		channel3.LengthTick();
    channel4.LengthTick();
		static bool sweep_tick = false;
		if (sweep_tick) { //128hz
			channel1.SweepTick();
		}
		sweep_tick = !sweep_tick;
		ulencounterclock = 0;
  }

  channel1.EnvelopeTick();
  channel2.EnvelopeTick();
  channel4.EnvelopeTick();


  ++sample_counter;
  if (sample_counter >= sample_ratio)
  {

    //osc1.Tick(osc1.get_increment(channel1freq))* (channel1env.vol / 15.0f);//
    auto channel1_sample = ((channel1.sample) * (channel1.envelope.vol / 15.0f));
    auto channel2_sample = ((channel2.sample) * (channel2.envelope.vol / 15.0f));//osc2.Tick(osc2.get_increment(channel2freq)) * (channel2env.vol / 15.0f);

    auto channel3_sample = (channel3.sample / 15.0f)*channel3.vol;

    auto channel4_sample = float(channel4.sample) * (channel4.envelope.vol / 15.0f);
   
    auto sample_left = ((nr51_.ch1so1 * channel1_sample)+(nr51_.ch2so1 * channel2_sample)+(nr51_.ch3so1 * channel3_sample)+(nr51_.ch4so1 * channel4_sample));
    auto sample_right = ((nr51_.ch1so2 * channel1_sample)+(nr51_.ch2so2 * channel2_sample)+(nr51_.ch3so2 * channel3_sample)+(nr51_.ch4so2 * channel4_sample));
		
		sample_left = ((sample_left*2.0f)-1) * 0.25f * 32767.0f * 0.5f;
		sample_right = ((sample_right*2.0f)-1) * 0.25f * 32767.0f * 0.5f;
    sample_left *= nr50_.so1vol / 7.0f;
    sample_right *= nr50_.so2vol / 7.0f;

    static short sbuf[8820]= {0,0};
    static int sindex = 0;
    sbuf[sindex++] = short(sample_left);
    sbuf[sindex++] = short(sample_right); 
    sample_counter -= sample_ratio;   

    if (sindex == 8820) {
      output->Write(sbuf,8820<<1);
      //WASAPI_WriteData(sbuf,8820);
      sindex = 0;
    }
  }

  
}

uint8_t Apu::Read(uint16_t address) {

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

void Apu::Write(uint16_t address, uint8_t data) {
  ioports[address-0xFF10] = data;
  if (address>=0xFF30 && address<=0xFF3F) {
     int index = (address & 0x0F)<<1;
     wavram[index] = data>>4;
     wavram[index+1] = data&0xF;
  }

  switch (address) {
    case 0xFF10:
      nr10_.raw = data;
			
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
      channel1.freqcounterload = (2048-x);

      //channel1freq = 131072.0f/(2048-x);
      if (nr14_ & 0x80) {
        nr52_ |= 0x01;
        channel1.envelope.raw = nr12_.raw;
        channel1.envcounterload = (channel1.envelope.env_sweep*4194304/64);
        channel1.envcounter = channel1.envcounterload;
				channel1.sweepcounter = nr10_.sweep_time;
				channel1.sweepfreqcounter = channel1.freqcounterload;
        channel1.freqcounter = channel1.freqcounterload;
        channel1.lengthcounter = channel1.lengthcounterload;
      }
      break;
    }


    case 0xFF16:
      nr21_.raw = data;
      channel2.wavepatternduty = (data&0xC0)>>3;
      channel2.lengthcounterload = 64 - (data&0x3F);
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
			channel2.freqcounterload = 2048-x;
      //channel2freq = 131072.0f/(2048-x);
      if (nr24_ & 0x80) {
        nr52_ |= 0x02;
        channel2.envelope.raw = nr22_.raw;
        channel2.envcounterload = (channel2.envelope.env_sweep*4194304/64);
        channel2.envcounter = channel2.envcounterload;
        channel2.freqcounter = channel2.freqcounterload;
        channel2.lengthcounter = channel2.lengthcounterload;
      }
      break;
    }

    case 0xFF1A:
      nr30_ = data;
      channel3.enabled = (nr30_&0x80)==0x80;
      break;
    case 0xFF1B:
      nr31_ = data;
			channel3.lengthcounterload = 256 - data;
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
			channel3.freqcounterload = 2048-x;
      //if (nr34_&0x40)
        //channel3.soundlength_ms = 1000.0f * (256.0f-nr31_)*(1/256.0f);
      if ((nr34_&0x80)&&(nr30_&0x80)) {
        nr52_ |= 0x04;
        channel3.lengthcounter = channel3.lengthcounterload;
        channel3.freqcounter = channel3.freqcounterload;
        channel3.playback_counter = 0;
        channel3.enabled = true;
        memset(&ioports[0x30],0,16);
      }
      break;
    }

    case 0xFF20:
      nr41_.raw = data;
      channel4.lengthcounterload = 64 - (data&0x3F);
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
        channel4polycounterms = 0;
        channel4.lengthcounter = channel4.lengthcounterload;
        channel4.envelope.raw = nr42_.raw;
        channel4.envcounterload = (channel4.envelope.env_sweep*4194304/64);
        channel4.envcounter = channel4.envcounterload;
        channel4.shiftreg = 0xFF;
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
      nr52_ = (data&0x80) | (nr52_&0x7F);
      if ((nr52_&0x80)==0)
        Reset();
      break;
 
  }
}


}
}