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

class MBC1 : public MemoryBankController {
 public:
  void Initialize(Cartridge* cartridge) {
    MemoryBankController::Initialize(cartridge);
    eram_enable = 0;
    ram_bank_number = 0;
    bank_select1 = 1;
    bank_select2 = 0;
    mode = 0;
    battery_ = cartridge->header->cartridge_type == 0x03;
    if (battery_)
      cartridge->LoadRam();
  }
  void Deinitialize() {
    if (battery_)
      cartridge->SaveRam();
    MemoryBankController::Deinitialize();
  }
  uint8_t* GetMemoryPointer(uint16_t address) {
    /*if (address >= 0x0000 && address <= 0x3FFF) {
      return &(cartridge->rom()[address]);
    } else if (address >= 0x4000 && address <= 0x7FFF) {
      return &(cartridge->rom()[(address&0x3FFF)+((rom_bank_number % max_rom_banks) <<14)]);
    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if ((eram_enable&0x0A)==0x0A && eram_size)
        return &eram_[(address&0x1FFF)|((ram_bank_number % max_ram_banks)<<13)];
      else
        return nullptr;
    }*/
    return nullptr;
  }
  uint8_t Read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x3FFF) {
      if (mode == 1) {
        return cartridge->rom()[address | (((bank_select2 << 5) % max_rom_banks) << 14)];
      } else {
        return cartridge->rom()[address];
      }

    } else if (address >= 0x4000 && address <= 0x7FFF) {

        return cartridge->rom()[address | ((((bank_select2 << 5) | bank_select1) % max_rom_banks) << 14)];


      //return cartridge->rom()[(address&0x3FFF)+((rom_bank_number % max_rom_banks) <<14)];
    } else if (address >= 0xA000 && address <= 0xBFFF) {


      if ((eram_enable & 0x0F) == 0x0A && eram_size)
        return eram_[(address & 0x1FFF) | ((ram_bank_number % max_ram_banks) << 13)];
      else
        return 0xFF; //0xFF considered unknown instead of 0
    }
    return 0;
  }

  void Write(uint16_t address, uint8_t data) {
    char line[55];
    if (address >= 0x0000 && address <= 0x1FFF) {

      eram_enable = data;
      //sprintf_s(line,"eram enable 0x%02x\n", data);
      //OutputDebugString(line);
    } else if (address >= 0x2000 && address <= 0x3FFF) {
      if (data == 0)
        data = 1;
      bank_select1 = data & 0x1F;
     // rom_bank_number = (rom_bank_number & ~0x1F) | (data & 0x1F);
      sprintf_s(line, "rom bank low %02x =  0x%04x\n", data, rom_bank_number);
      OutputDebugString(line);
      cartridge->emu()->memory()->UpdateMemoryMap();
    } else if (address >= 0x4000 && address <= 0x5FFF) {
      bank_select2 = data & 0x3;
      if (mode == 0) {
        //rom_bank_number = (rom_bank_number&~0xFFE0)|((data&0x3)<<5); //FFE0 because of 16bit var, instead of 0x60

        //if (rom_bank_number == 0x20)
        //  rom_bank_number = 0x21;
        ram_bank_number = 0;
      } else {

        //rom_bank_number &= 0x1F;
        ram_bank_number = bank_select2;

      }

    } else if (address >= 0x6000 && address <= 0x7FFF) {
      mode = data & 1;

    } else if (address >= 0xA000 && address <= 0xBFFF) {
      if ((eram_enable & 0x0F) == 0x0A && eram_size)
        eram_[(address & 0x1FFF) | ((ram_bank_number % max_ram_banks) << 13)] = data;
    }
  }
  uint8_t mode;
  uint8_t bank_select1;
  uint8_t bank_select2;
};


}
}