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
#include "../../gbemu.h"

#include "../../audio/synth/lowpass.h"

namespace emulation {
namespace gb {

audio::synth::filters::LowPassFilter lowpass_left,lowpass_right;



float high_pass( float in, bool dacs_enabled )
{
  static float capacitor = 0.0;
     float out = 0.0;
     if ( dacs_enabled )
     {
         out = in - capacitor;
         
         // capacitor slowly charges to 'in' via their difference
         capacitor = in - out * 0.996f; // use 0.998943 for MGB&CGB
     }
     return out;
}

void Apu::Initialize(Emu* emu) {
  Component::Initialize(emu);
  output_buffer_ = new short[4410*2];
  output_ = new audio::output::NullAudio();
  //output_->set_window_handle(app::GBEmu::Current()->display_window().handle());
  output_->set_buffer_size(4*44100*2);
  output_->Initialize(44100,2,16);
  output_->Play();

  sample_counter_ = 0;
  sample_ratio_ = uint32_t(emu_->base_freq_hz() / 44100);
  //noise.set_sample_rate(44100);
  maincounter = 0;
  ulencounterclock = 0;
  frame_seq_clock = 0;
  frame_seq_step = 0;

  audio::synth::noiseseed = rand();
  Reset();

  lowpass_left.set_sample_rate(44100);
  lowpass_left.set_cutoff_freq(22050);
  lowpass_left.Initialize();
  
  lowpass_right.set_sample_rate(44100);
  lowpass_right.set_cutoff_freq(22050);
  lowpass_right.Initialize();
}

void Apu::Reset() {
  
  //channel4freq = 0;
 // channel4polycounterms = 0;
 
  channel1.Initialize(this,1);
  channel2.Initialize(this,2);
  channel3.Initialize(this);
  channel4.Initialize(this);
  
  io_0xff15_ = 0;
  io_0xff1f_ = 0;
  io_0xff27_ = 0;
  io_0xff28_ = 0;
  io_0xff29_ = 0;
  io_0xff2a_ = 0;
  io_0xff2b_ = 0;
  io_0xff2c_ = 0;
  io_0xff2d_ = 0;
  io_0xff2e_ = 0;
  io_0xff2f_ = 0;


  memset(channel3.wavedata,0,sizeof(channel3.wavedata));

  
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
    const uint8_t wavedatainitial[16]={0x84,0x40,0x43,0xAA,0x2D,0x78,0x92,0x3C,0x60,0x59,0x59,0xB0,0x34,0xB8,0x2E,0xDA};
    for (int i=0;i<16;++i)
      Write(0xFF30+i,wavedatainitial[i]);
  }
}

void Apu::Deinitialize() {

  output_->Stop();
  output_->Deinitialize(); 
  SafeDelete(&output_);
  SafeDeleteArray(&output_buffer_);
}

void Apu::Tick() {
  //const double dt =  1000.0 / emu_->base_freq_hz();
  if ((nr52_&0x80)==0)
    return;

  //if ((maincounter & 0x3) == 0) {// its like every 32/8 because 8 samples in duty cycle
      channel1.SampleTick();
      channel2.SampleTick();
  //}

  //if ((maincounter & 0x1) == 0) {
      channel3.SampleTick();
  //}
  //channel4polycounterms += float(dt);
  channel4.SampleTick();

  maincounter = (maincounter+1) & 0x1F;
  //++ulencounterclock;
  if (++frame_seq_clock == 8192*emu_->speed) {
    switch (frame_seq_step) {
      case 0:LengthTick(); break;
      case 1: break;
      case 2:LengthTick(); channel1.SweepTick(); break;
      case 3: break;
      case 4:LengthTick();  break;
      case 5: break;
      case 6:LengthTick(); channel1.SweepTick();break;
      case 7:EnvelopeTick(); break;
    }

    frame_seq_step = ++frame_seq_step & 0x7;
    frame_seq_clock = 0;
  }
 

  
  channel1.acc_sample_left += channel1.sample*channel1.envelope.vol*nr51_.ch1so2;
  channel2.acc_sample_left += channel2.sample*channel2.envelope.vol*nr51_.ch2so2;
  channel3.acc_sample_left += (channel3.sample>>channel3.volshift)*nr51_.ch3so2;
  channel4.acc_sample_left += channel4.sample*channel4.envelope.vol*nr51_.ch4so2;

  channel1.acc_sample_right += channel1.sample*channel1.envelope.vol*nr51_.ch1so1;
  channel2.acc_sample_right += channel2.sample*channel2.envelope.vol*nr51_.ch2so1;
  channel3.acc_sample_right += (channel3.sample>>channel3.volshift)*nr51_.ch3so1;
  channel4.acc_sample_right += channel4.sample*channel4.envelope.vol*nr51_.ch4so1;

  if (++sample_counter_ == sample_ratio_)
  {

    auto sample_right = (channel1.acc_sample_right+channel2.acc_sample_right+channel3.acc_sample_right+channel4.acc_sample_right)/(15.0f*sample_counter_)*(nr50_.so1vol+1);
    auto sample_left =  (channel1.acc_sample_left+channel2.acc_sample_left+channel3.acc_sample_left+channel4.acc_sample_left)/(15.0f*sample_counter_)*(nr50_.so2vol+1);
    //sample_right = high_pass(sample_right,channel1.dac_enable);
    sample_left= lowpass_left.Tick(sample_left);
    sample_right= lowpass_right.Tick(sample_right);
    sample_right = ((sample_right*2.0f)-1) * 0.25f * 32767.0f * 0.125f; //0.125 = master volume attentuation,0.25 to reduce for 4 channels
    sample_left = ((sample_left*2.0f)-1) * 0.25f * 32767.0f * 0.125f;


    
    static int sindex = 0;
    output_buffer_[sindex++] = short(sample_left*0.5f);
    output_buffer_[sindex++] = short(sample_right*0.5f); 
    sample_counter_ -= sample_ratio_;   

    if (sindex == 8820) {
      output_->Write(output_buffer_,8820<<1);
      sindex = 0;
    }

    channel1.acc_sample_left = channel2.acc_sample_left = channel3.acc_sample_left = channel4.acc_sample_left = 0;
    channel1.acc_sample_right = channel2.acc_sample_right = channel3.acc_sample_right = channel4.acc_sample_right = 0;
  }

  
}

uint8_t Apu::Read(uint16_t address) {
  const uint8_t masks[32] = {
     0x80,0x3F,0x00,0xFF,0xBF,
     0xFF,0x3F,0x00,0xFF,0xBF,
     0x7F,0xFF,0x9F,0xFF,0xBF,
     0xFF,0xFF,0x00,0x00,0xBF,
     0x00,0x00,0x70,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
  };
  uint8_t result=0;

  if (address>=0xFF30 && address<=0xFF3F) {
    result = waveram[address-0xFF30];
  } else {


    switch (address) {
      case 0xFF10:
        result = channel1.reg0.raw | masks[0];
        break;
      case 0xFF11:
        result = channel1.reg1 | masks[1];
        break;
      case 0xFF12:
        result = channel1.reg2 | masks[2];
        break;
      case 0xFF13:
        result = channel1.reg3 | masks[3];
        break;
      case 0xFF14:
       result = channel1.reg4 | masks[4];
        break;

      case 0xFF15:
        result = io_0xff15_ | masks[5];
        break;
      case 0xFF16:
        result = channel2.reg1| masks[6];
        break;
      case 0xFF17:
        result = channel2.reg2| masks[7];
        break;
      case 0xFF18:
        result = channel2.reg3| masks[8];
        break;
      case 0xFF19:
        result = channel2.reg4| masks[9];
        break;

      case 0xFF1A:
        result = channel3.reg0.raw | masks[10];
        break;
      case 0xFF1B:
        result = channel3.reg1 | masks[11];
        break;
      case 0xFF1C:
        result = channel3.reg2 | masks[12];
        break;
      case 0xFF1D:
        result = channel3.reg3 | masks[13];
        break;
      case 0xFF1E:
        result = channel3.reg4 | masks[14];
        break;

      case 0xFF1F:
        result = io_0xff1f_| masks[15];
        break;

      case 0xFF20:
        result = channel4.reg1| masks[16];
        break;
      case 0xFF21:
        result = channel4.reg2| masks[17];
        break;
      case 0xFF22:
        result = channel4.reg3| masks[18];
        break;
      case 0xFF23:
        result = channel4.reg4| masks[19];
        break;

      case 0xFF24:
        result = nr50_.raw| masks[20];
        break;
      case 0xFF25:
        result = nr51_.raw| masks[21]; 
        break;
      case 0xFF26:
        result = nr52_| masks[22]; 
        break;

      case 0xFF27:
        result = io_0xff27_ | masks[23]; 
        break;
      case 0xFF28:
        result = io_0xff28_ | masks[24]; 
        break;
      case 0xFF29:
        result = io_0xff29_ | masks[25]; 
        break;
      case 0xFF2A:
        result = io_0xff2a_ | masks[26];  
        break;
      case 0xFF2B:
        result = io_0xff2b_ | masks[27]; 
        break;
      case 0xFF2C:
        result = io_0xff2c_ | masks[28]; 
        break;
      case 0xFF2D:
        result = io_0xff2d_ | masks[29]; 
        break;
      case 0xFF2E:
        result = io_0xff2e_ | masks[30]; 
        break;
      case 0xFF2F:
        result = io_0xff2f_ | masks[31]; 
        break;
    }
  }
#ifdef APU_LOG
  char str[255];
  sprintf(str,"PC:0x%04x - Apu::Read(0x%04x) = 0x%02x\n",emu_->cpu()->opcode_pc,address,result);
  OutputDebugString(str);
#endif
  return result;
}

void Apu::Write(uint16_t address, uint8_t data) {
#ifdef APU_LOG
  char str[255];
  sprintf(str,"PC:0x%04x - Apu::Write(0x%04x,0x%02x)\n",emu_->cpu()->opcode_pc,address,data);
  OutputDebugString(str);
#endif

  if (address>=0xFF30 && address<=0xFF3F) {
     int index = (address & 0x0F)<<1;
     waveram[address-0xFF30] = data;
     channel3.wavedata[index] = data>>4;
     channel3.wavedata[index+1] = data&0xF;
     return;
  }

  if ((nr52_&0x80)==0 && address != 0xFF26)
    return;

  switch (address) {
    case 0xFF10:
      channel1.reg0.raw = data;
      break;
    case 0xFF11:
      channel1.reg1 = data;
      channel1.wavepatternduty = (data&0xC0)>>3;
      channel1.lengthcounter = 64 - (data&0x3F);
      if (channel1.lengthcounter == 0)
        nr52_ &= ~0x01;
      break;
    case 0xFF12:
      channel1.reg2 = data;  
      if ((channel1.reg2&0xF8)==0x00) {
        nr52_ &= ~0x01;
        channel1.dac_enable = false;
      } else  {
        channel1.dac_enable = true;
      }
      break;
    case 0xFF13:
      channel1.reg3 = data;
      break;
    case 0xFF14: {



      channel1.reg4 = data;
      uint32_t x = channel1.reg3;
      x |= (channel1.reg4&0x7)<<8;
      channel1.freqcounterload = ((2048-x)<<2)*emu_->speed; //*4
      
      //channel1freq = 131072.0f/(2048-x);
      if (channel1.reg4 & 0x80) {
        if (channel1.dac_enable) //dac
          nr52_ |= 0x01;
        channel1.envelope.raw = channel1.reg2;
        channel1.envcounterload = (channel1.envelope.env_sweep);
        channel1.envcounter = channel1.envcounterload;
        channel1.sweepcounter = channel1.reg0.sweep_time;
        channel1.sweepfreqcounter = ((x>>channel1.reg0.sweep_shift)<<2)*emu_->speed;
        channel1.freqcounter = channel1.freqcounterload;
        
      

        if (!channel1.lengthcounter) {
          channel1.lengthcounter = 64;
          if (frame_seq_step%2==1&&(data&0x40))
          --channel1.lengthcounter;
          if ((data&0x40)&&(channel1.enable_length_clock==false)) {
            channel1.enable_length_clock = true;
            channel1.LengthTick(nr52_);
          }
        }
         

      }


      if (frame_seq_step%2==1)
        if ((data&0x40)&&channel1.enable_length_clock==false) {
          channel1.enable_length_clock = true;
          channel1.LengthTick(nr52_);
        }
      channel1.enable_length_clock = (data&0x40)==0x40;
      break;
    }

    case 0xFF15:io_0xff15_ = data; break;

    case 0xFF16:
      channel2.reg1 = data;
      channel2.wavepatternduty = (data&0xC0)>>3;
      channel2.lengthcounter = 64 - (data&0x3F);
      if (channel2.lengthcounter == 0)
        nr52_ &= ~0x02;
      break;
    case 0xFF17:
      channel2.reg2 = data;
      if ((channel2.reg2&0xF8)==0x00) {
        nr52_ &= ~0x02;
        channel2.dac_enable = false;
      } else  {
        channel2.dac_enable = true;
      }
      break;
    case 0xFF18:
      channel2.reg3 = data;
      break;
    case 0xFF19: {
      channel2.reg4 = data;
      uint32_t x = channel2.reg3;
      x |= (channel2.reg4&0x7)<<8;
      channel2.freqcounterload = ((2048-x)<<2)*emu_->speed;
      //channel2freq = 131072.0f/(2048-x);
      channel2.enable_length_clock = (data&0x40)==0x40;
      if (channel2.reg4 & 0x80) {
        if (channel2.dac_enable) //dac
          nr52_ |= 0x02;
        channel2.envelope.raw = channel2.reg2;
        channel2.envcounterload = (channel2.envelope.env_sweep);
        channel2.envcounter = channel2.envcounterload;
        channel2.freqcounter = channel2.freqcounterload;
        if (!channel2.lengthcounter)
          channel2.lengthcounter = 64;
      }
      break;
    }

    case 0xFF1A:
      channel3.reg0.raw = data;
      channel3.dac_enable = (channel3.reg0.raw&0x80)==0x80;
      if (channel3.dac_enable==false) 
        nr52_ &= ~0x04;
              
      break;
    case 0xFF1B:
      channel3.reg1 = data;
      channel3.lengthcounter = 256 - data;
      if (channel3.lengthcounter == 0)
        nr52_ &= ~0x04;
      break;
    case 0xFF1C:
      channel3.reg2 = data;  
      switch ((channel3.reg2 & 0x60)>>5) {
        case 0x0:channel3.vol = 0; channel3.volshift = 4; break;
        case 0x1:channel3.vol = 1.0f;channel3.volshift = 0; break;
        case 0x2:channel3.vol = 0.5f; channel3.volshift = 1;break;
        case 0x3:channel3.vol = 0.25f;channel3.volshift = 2; break;
      }
      break;
    case 0xFF1D:
      channel3.reg3 = data;
      break;
    case 0xFF1E: {
      channel3.reg4 = data;
      uint32_t x = channel3.reg3;
      x |= (channel3.reg4&0x7)<<8;
      channel3.freqcounterload = ((2048-x)<<1)*emu_->speed;
      //if (nr34_&0x40)
        //channel3.soundlength_ms = 1000.0f * (256.0f-nr31_)*(1/256.0f);
      //channel3.enable_length_clock = data&0x40==0x40;
      if ((channel3.reg4&0x80)&&(channel3.reg0.raw&0x80)) {
        if(channel3.dac_enable)
          nr52_ |= 0x04;
        channel3.freqcounter = channel3.freqcounterload;
        channel3.playback_counter = 0;
        memset(waveram,0,sizeof(waveram));
        if (!channel3.lengthcounter)
          channel3.lengthcounter = 256;
      }
      break;
    }
    case 0xFF1F:io_0xff1f_ = data; break;
    case 0xFF20:
      channel4.reg1 = data;
      channel4.lengthcounter = 64 - (data&0x3F);
      if (channel4.lengthcounter == 0)
        nr52_ &= ~0x08;
      break;
    case 0xFF21:
      channel4.reg2 = data;
      if ((channel4.reg2&0xF8)==0x00) {
        nr52_ &= ~0x08;
        channel4.dac_enable = false;
      } else  {
        channel4.dac_enable = true;
      }
      break;
    case 0xFF22:
      channel4.reg3 = data;
      break;
    case 0xFF23: {
      channel4.reg4 = data;
     // float r = float(channel4.reg3&0x7);
      //float s = float((channel4.reg3&0xF0)>>4);
      //channel4freq = 524288.0f/r/powf(2.0,s+1);
      channel4.enable_length_clock = (data&0x40)==0x40;
      const uint16_t divisors[8] = {8,16,32,48,64,80,96,112};
      auto divisor = divisors[channel4.reg3&0x7];
      auto shift = (channel4.reg3&0xF0)>>4;
      channel4.freqcounterload = (divisor<<shift)*emu_->speed;
      if (channel4.reg4 & 0x80) {
        if (channel4.dac_enable) //dac
          nr52_ |= 0x08;
        channel4.freqcounter = channel4.freqcounterload;
        //channel4polycounterms = 0;
        channel4.envelope.raw = channel4.reg2;
        channel4.envcounterload = (channel4.envelope.env_sweep);
        channel4.envcounter = channel4.envcounterload;
        channel4.shiftreg = 0xFF;
        if (!channel4.lengthcounter)
          channel4.lengthcounter = 64;
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
      if ((nr52_&0x80)==0) {
        channel1.reg0.raw = channel1.reg1 = channel1.reg2 = channel1.reg3 = channel1.reg4 = 0;
        io_0xff15_ = 0;
        channel2.reg1 = channel2.reg2 = channel2.reg3 = channel2.reg4 = 0;
        channel3.reg0.raw = channel3.reg1 = channel3.reg2 = channel3.reg3 = channel3.reg4 = 0;
        io_0xff1f_ = 0;
        channel4.reg1 = channel4.reg2 = channel4.reg3 = channel4.reg4 = 0;
        nr50_.raw =  0;
        nr51_.raw =  0;
        nr52_ &= ~0x7F;
        //output_->Stop();
      } else {
        //output_->Play();
        frame_seq_step = 0;
        channel3.playback_counter = 0;
        channel1.wavepatterncounter = channel2.wavepatterncounter = 0;
      }
      break;
      case 0xFF27:
         io_0xff27_ = data;
        break;
      case 0xFF28:
         io_0xff28_ = data;
        break;
      case 0xFF29:
        io_0xff29_ = data;
        break;
      case 0xFF2A:
        io_0xff2a_ = data;
        break;
      case 0xFF2B:
        io_0xff2b_ = data;
        break;
      case 0xFF2C:
        io_0xff2c_  = data;
        break;
      case 0xFF2D:
        io_0xff2d_  = data;
        break;
      case 0xFF2E:
        io_0xff2e_  = data;
        break;
      case 0xFF2F:
        io_0xff2f_  = data;
        break;

  }
}


}
}