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

class MBC5 : public MemoryBankController {
 public:
  void Initialize(Cartridge* cartridge) {
    MemoryBankController::Initialize(cartridge);
    eram_enable = 0;
    eram_enable = 0;
    rom_bank_number = 1;
    ram_bank_number = 0;
    switch(cartridge->header->cartridge_type) {
      case 0x1B:battery_ = true; break;
      case 0x1E:battery_ = true; break;
    }
    if (battery_)
      cartridge->LoadRam();
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
      if ((eram_enable&0x0A)==0x0A && eram_size)
        return &eram_[(address&0x1FFF)|(ram_bank_number<<13)];
      else
        return 0;
    }
    return 0;
  }

  uint8_t Read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x3FFF) {
      return rom_[address];
    } else if (address >= 0x4000 && address <= 0x7FFF) {
      //if (rom_bank_number >= max_rom_banks)
       // return 0;
      return rom_[(address&0x3FFF)|((rom_bank_number%max_rom_banks)<<14)];
    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if ((eram_enable&0x0F)==0x0A && eram_size)
        return eram_[(address&0x1FFF)|(ram_bank_number<<13)];//*0x2000
      else
        return 0xFF;
    }
    return 0;
  }
  //still not right
  void Write(uint16_t address, uint8_t data) {
   if (address >= 0x0000 && address <= 0x1FFF) {
      eram_enable = data;
    } else if (address >= 0x2000 && address <= 0x2FFF) {
      char str[25];
      sprintf_s(str, "rom select low %02x\n", data);
      OutputDebugString(str);
      auto r = (rom_bank_number&0xFF00)|data;
     // if (r < max_rom_banks)
        rom_bank_number = r;
    } else if (address >= 0x3000 && address <= 0x3FFF) {
      char str[25];
      sprintf_s(str, "rom select high %02x\n", data);
      OutputDebugString(str);
      auto r =  ((data & 0x1) << 8) | (rom_bank_number & 0xFF);
     // if (r < max_rom_banks)
        rom_bank_number = r;
    } else if (address >= 0x4000 && address <= 0x5FFF) {
      ram_bank_number = data&0xF;
    } else if (address >= 0x6000 && address <= 0x7FFF) {

    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if ((eram_enable&0x0F)==0x0A && eram_size)
        eram_[(address&0x1FFF)|(ram_bank_number<<13)] = data;
    }
  }


};

}
}