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
    if (eram_size)
       eram_ = new uint8_t[eram_size];
    else
      eram_ = nullptr;
    rom_ = cartridge->rom();
    rom_bank_number = 1;
    ram_bank_number = 0;
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
 protected:
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
#include "mbc1.cpp"
#include "mbc2.cpp"
#include "mbc3.cpp"
#include "mbc5.cpp"
#include "mbc_pocketcamera.cpp"
#include "mbc_tama5.cpp"
