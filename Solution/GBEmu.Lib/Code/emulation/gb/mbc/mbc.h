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

namespace emulation {
namespace gb {

class MemoryBankController {
 public:
  virtual void Initialize(Cartridge* cartridge) {
    battery_ = false;
    this->cartridge = cartridge;
    eram_size = cartridge->header->ram_size_bytes();
    sprintf_s(logstr, "MBC eram_size %08x\n", eram_size);
    cartridge->emu()->log_output(logstr);
    max_ram_banks = 1;
    if (eram_size) {
      max_ram_banks = max(eram_size / 0x2000,1);
      eram_ = new uint8_t[eram_size];
    }
    else {
      max_ram_banks = 0;
      eram_ = nullptr;
    }

    sprintf_s(logstr, "MBC max_ram_banks %08x\n", max_ram_banks);
    cartridge->emu()->log_output(logstr);

    rom_ = cartridge->rom();
    rom_bank_number = 1;
    ram_bank_number = 0;
    
    max_rom_banks = cartridge->header->rom_size_bytes() / 0x4000;
    sprintf_s(logstr, "MBC max_rom_banks %08x\n", max_rom_banks);
    cartridge->emu()->log_output(logstr);
  }
  virtual void Deinitialize() {
     SafeDeleteArray(&eram_); 
  };
  virtual uint8_t* GetMemoryPointer(uint16_t address) = 0;
  virtual uint8_t Read(uint16_t address) = 0;
  virtual void Write(uint16_t address, uint8_t data) = 0;
  virtual void Step(double dt) { }
  uint8_t* eram() { return eram_; }
  uint16_t rom_bank_number;
  uint8_t ram_bank_number;
  uint8_t max_ram_banks;
  uint16_t max_rom_banks;
 protected:
  char logstr[200];
  Cartridge* cartridge;
  uint8_t* rom_;
  uint8_t* eram_;
  uint8_t eram_enable;
  uint32_t eram_size;
  bool battery_;
};


}
}

#include "none.h"
#include "mbc1.h"
#include "mbc2.h"
#include "mbc3.h"
#include "mbc5.h"
#include "mbc_pocketcamera.h"
#include "mbc_tama5.h"
