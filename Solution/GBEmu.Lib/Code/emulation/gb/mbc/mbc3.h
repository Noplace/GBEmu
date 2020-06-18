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


namespace emulation {
namespace gb {
  enum MBC3Mode { MBC3ModeRAM = 0, MBC3ModeRTC = 1 };
class MBC3 : public MemoryBankController {
 public:
  void Initialize(Cartridge* cartridge) {
    MemoryBankController::Initialize(cartridge);
    rom_bank_number = 0;
    ram_bank_number = 0;
    mode = MBC3ModeRAM;
    
    ram_rtc_enable = false;

    rtc_select = 0;

    timecounter = 0;
    rtc_timer = 0;
    memset(rtc,0,sizeof(rtc));
    switch(cartridge->header->cartridge_type) {
      case 0x0F:battery_ = true; break;
      case 0x10:battery_ = true; break;
      case 0x13:battery_ = true; break;
    }
    if (battery_)
      cartridge->LoadRam();

    latch_seq = 0;
  }
  void Deinitialize() {
    if (battery_)
      cartridge->SaveRam();
    MemoryBankController::Deinitialize();
  }
  uint8_t* GetMemoryPointer(uint16_t address) {
    if (address >= 0x0000 && address <= 0x3FFF) {
      return &rom_[address];
    } else if (address >= 0x4000 && address <= 0x7FFF) {
      return &rom_[(address&0x3FFF)|((rom_bank_number)<<14)];
    } else if (address >= 0xA000 && address <= 0xBFFF) {


      if (ram_rtc_enable == true &&mode==0 && eram_size)
        return &eram_[(address&0x1FFF)|(ram_bank_number<<13)];
      
      if (ram_rtc_enable == true && mode== MBC3ModeRTC)
        return &rtc[rtc_select];

      return 0;
    }
    return 0;
  }

  uint8_t Read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x3FFF) {
      return rom_[address];
    } else if (address >= 0x4000 && address <= 0x7FFF) {
      return rom_[(address&0x3FFF)|((rom_bank_number)<<14)];
    } else if (address >= 0xA000 && address <= 0xBFFF) {

      if (ram_rtc_enable == true && mode == MBC3ModeRAM && eram_size)
        return eram_[(address&0x1FFF)|(ram_bank_number<<13)];
      
      if (ram_rtc_enable == true && mode == MBC3ModeRTC)
        return rtc[rtc_select];

      return 0;
    }
    return 0;
  }

  void Write(uint16_t address, uint8_t data) {
   
    if (address >= 0x0000 && address <= 0x1FFF) {
      
      if (data & 0x0A)  {
        ram_rtc_enable = true;
      }

      if (data == 0) {
        ram_rtc_enable = false;
      }


    } else if (address >= 0x2000 && address <= 0x3FFF) {
      rom_bank_number = data&0x7F;
      if (rom_bank_number == 0)
        rom_bank_number = 1;
    } else if (address >= 0x4000 && address <= 0x5FFF) {
      if (data>=0&&data<=3) {
        mode = MBC3ModeRAM;
        ram_bank_number = data;
      } else if (data>=0x08&&data<=0x0C) {
        mode = MBC3ModeRTC;
        rtc_select = data-0x08;
      }
    } else if (address >= 0x6000 && address <= 0x7FFF) {
      if (data == 0x00 && latch_seq == 0)
        latch_seq = 1;
      if (data == 0x01 && latch_seq == 1) {
        
        rtc[0] = rtc_timer % 60;
        rtc[1] = (rtc_timer/ 60) % 60;
        rtc[2] = (rtc_timer / 60 / 60) % 24;
        auto days = rtc_timer / 60 / 60 / 24;
        rtc[3] = days&0xFF;
        rtc[4] &= ~0x81;
        rtc[4] |= (days>>8)&0x1;
        rtc[4] |= (days>>2)&0x80;
        latch_seq = 0;
      }
  

    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if (ram_rtc_enable == true && mode== MBC3ModeRAM && eram_size)
        eram_[(address&0x1FFF)|(ram_bank_number<<13)] = data;
      
      if (ram_rtc_enable == true && mode== MBC3ModeRTC)
        rtc[rtc_select] = data;
    }
  }

  void Step(double dt) {
    timecounter += dt;
    if (timecounter >= 1000.0) { // 1sec ////32768Hz, for original gameboy

      if (rtc[4]&0x40) return;//halt
      ++rtc_timer;
     /* if (++rtc[0] == 60) { //sec
        if (++rtc[1] == 60) { //min
          if (++rtc[2] == 24) { //hour
            if (++rtc[3] == 0xFF) { //day
              if (rtc[4]&1)
                rtc[4] |= 0x80;
              else
                rtc[4] |= 1;
              rtc[3] = 0;
            }
            rtc[2] = 0;
          }
          rtc[1] = 0;
        }
        rtc[0] = 0;
      }*/
      timecounter -= 1000.0;
    }
  }

  bool ram_rtc_enable;
  uint8_t mode;
  uint8_t rtc[5];
  uint64_t rtc_timer;
  uint8_t rtc_select;
  uint8_t latch_seq;
  double timecounter;
};

}
}