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

}
}