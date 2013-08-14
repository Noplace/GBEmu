#pragma once

namespace emulation {
namespace gb {

class MBCNone : public MemoryBankController {
 public:
  void Deinitialize() {
    MemoryBankController::Deinitialize();
  }
  uint8_t Read(uint16_t address) {
      if (address >= 0x0000 && address <= 0x3FFF) {
	    return cartridge->rom()[address];
	  } else if (address >= 0x4000 && address <= 0x7FFF) {
      return cartridge->rom()[address];
    } else if (address >= 0xA000 && address <= 0xBFFF) {
      return 0;
	  }
    return 0;
  }
  void Write(uint16_t address, uint8_t data) {
    int a = 1;
  }
};

class MBC1 : public MemoryBankController {
 public:
  void Initialize(Cartridge* cartridge) {
    MemoryBankController::Initialize(cartridge);
    eram_enable = 0;
    eram_enable = 0;
    rom_bank_number = 1;
    ram_bank_number = 0;
    mode = 0;
  }
  void Deinitialize() {
    MemoryBankController::Deinitialize();
  }
  uint8_t Read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x3FFF) {
	    return cartridge->rom()[address];
	  } else if (address >= 0x4000 && address <= 0x7FFF) {
      return cartridge->rom()[address+0x4000*(rom_bank_number-1)];
    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if ((eram_enable&0x0A)==0x0A)
        return eram_[(address&0x1FFF)+(0x2000*ram_bank_number*mode)];
      else
        return 0;
    }
    return 0;
  }
  void Write(uint16_t address, uint8_t data) {
   if (address >= 0x0000 && address <= 0x1FFF) {
	    eram_enable = data;
	  } else if (address >= 0x2000 && address <= 0x3FFF) {
      rom_bank_number = data&0x1F;
      if (rom_bank_number == 0)
        rom_bank_number = 1;
    } else if (address >= 0x4000 && address <= 0x5FFF) {
      if (mode==0) {
        rom_bank_number |= (data&0x3)<<5;
        //ram_bank_number = 0;
      } else {
        //rom_bank_number &= 0x1F;
        ram_bank_number = data&3;
      }
    } else if (address >= 0x6000 && address <= 0x7FFF) {
       mode = data&1;

    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if ((eram_enable&0x0A)==0x0A)
        eram_[address&0x1FFF] = data;
	  }
  }
  uint8_t rom_bank_number;
  uint8_t mode;
  uint8_t ram_bank_number;
};

class MBC3 : public MemoryBankController {
 public:
  void Initialize(Cartridge* cartridge) {
    MemoryBankController::Initialize(cartridge);
    eram_enable = 0;
    eram_enable = 0;
    rom_bank_number = 1;
    ram_bank_number = 0;
    mode = 0;
    rtc_enable = 0;
    rtc_select = 0;
    counter1 = 0;
    counter2 = 0;
    memset(rtc,0,sizeof(rtc));
  }
  void Deinitialize() {
    MemoryBankController::Deinitialize();
  }
  uint8_t Read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x3FFF) {
	    return cartridge->rom()[address];
	  } else if (address >= 0x4000 && address <= 0x7FFF) {
      return cartridge->rom()[address+0x4000*(rom_bank_number-1)];
    } else if (address >= 0xA000 && address <= 0xBFFF) {


      if ((eram_enable&0x0A)==0x0A&&mode==0)
        return eram_[(address&0x1FFF)+(0x2000*ram_bank_number*mode)];
      
      if ((rtc_enable&0x0A)==0x0A&&mode==1)
        return rtc[rtc_select];

      return 0;
    }
    return 0;
  }
  void Write(uint16_t address, uint8_t data) {
   if (address >= 0x0000 && address <= 0x1FFF) {
	    eram_enable = data;
      rtc_enable = data;
	  } else if (address >= 0x2000 && address <= 0x3FFF) {
      rom_bank_number = data&0x7F;
      if (rom_bank_number == 0)
        rom_bank_number = 1;
    } else if (address >= 0x4000 && address <= 0x5FFF) {
      if (data>=0&&data<=3) {
        mode = 0;
        ram_bank_number = data;
      } else if (data>=0x08&&data<=0x0C) {
        mode = 1;
        rtc_select = data-0x08;
      }
    } else if (address >= 0x6000 && address <= 0x7FFF) {
      

    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if ((eram_enable&0x0A)==0x0A&&mode==0)
        eram_[address&0x1FFF] = data;
      
      if ((rtc_enable&0x0A)==0x0A&&mode==1)
        rtc[rtc_select] = data;
	  }
  }
  void Tick() {
    if (++counter1 == 128) { //32768Hz, for original gameboy
      if (++counter2 == 32768) {
        if (rtc[4]&0x40) return;//halt
        if (++rtc[0] == 60) { //sec
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
        }
        counter2 =0 ;
      }
      counter1 = 0;
    }
  }
  uint8_t rom_bank_number;
  uint8_t mode;
  uint8_t ram_bank_number;
  uint8_t rtc[5];
  uint8_t rtc_enable;
  uint8_t rtc_select;
  uint8_t counter1;
  uint16_t counter2;
};



}
}