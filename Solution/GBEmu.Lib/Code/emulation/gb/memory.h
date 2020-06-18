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

enum JoypadKeys { JoypadRight=0,JoypadLeft=1,JoypadUp=2,JoypadDown=3,JoypadA=4,JoypadB=5,JoypadSelect=6,JoypadStart=7};

class Memory : public Component {
 friend Cartridge;
 public:
  Memory(){}
  ~Memory() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void UpdateMemoryMap();
  void Reset();
  uint8_t* GetMemoryPointer(uint16_t address);
  uint8_t Read8(uint16_t address);
  void    Write8(uint16_t address, uint8_t data);
  uint8_t ClockedRead8(uint16_t address);
  void    ClockedWrite8(uint16_t address, uint8_t data);
  uint8_t* vram() { return vram_; }
  uint8_t* oam() { return oam_; }
  uint8_t* ioports() { return ioports_; }
  inline uint8_t& interrupt_enable() { return interrupt_enable_register_; }
  inline uint8_t& interrupt_flag() {  return ioports_[0x0F]; }
  uint16_t ClockedRead16(uint16_t address) {
    uint16_t nn;
    nn = ClockedRead8(address);
    nn |= ClockedRead8(address+1)<<8;
    return nn;
  }
  void JoypadPress(JoypadKeys key);
  void JoypadRelease(JoypadKeys key) {
    //ioports_[0] |= key;
    joypadflags[key] = false;
  }
  void Tick();
 private:
  uint8_t* memmap[16];
  const uint8_t* rom_;
  uint8_t* vram_;
  uint8_t* wram_;
  uint8_t oam_[160];
  uint8_t ioports_[128];
  uint8_t hram_[127];
  uint8_t interrupt_enable_register_;
  uint8_t wram_select;
  uint8_t vram_select;
  uint16_t last_address;
  bool joypadflags[8];
};

}
}