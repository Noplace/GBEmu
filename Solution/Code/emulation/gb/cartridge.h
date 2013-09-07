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

#include <WinCore/io/io.h>

namespace emulation {
namespace gb {


  /*
  Cartridge Type:
   00h  ROM ONLY                 13h  MBC3+RAM+BATTERY
  01h  MBC1                     15h  MBC4
  02h  MBC1+RAM                 16h  MBC4+RAM
  03h  MBC1+RAM+BATTERY         17h  MBC4+RAM+BATTERY
  05h  MBC2                     19h  MBC5
  06h  MBC2+BATTERY             1Ah  MBC5+RAM
  08h  ROM+RAM                  1Bh  MBC5+RAM+BATTERY
  09h  ROM+RAM+BATTERY          1Ch  MBC5+RUMBLE
  0Bh  MMM01                    1Dh  MBC5+RUMBLE+RAM
  0Ch  MMM01+RAM                1Eh  MBC5+RUMBLE+RAM+BATTERY
  0Dh  MMM01+RAM+BATTERY        FCh  POCKET CAMERA
  0Fh  MBC3+TIMER+BATTERY       FDh  BANDAI TAMA5
  10h  MBC3+TIMER+RAM+BATTERY   FEh  HuC3
  11h  MBC3                     FFh  HuC1+RAM+BATTERY
  12h  MBC3+RAM
  */
//#pragma pack(1)
struct CartridgeHeader {
  uint8_t entry_point[4];
  uint8_t nintendo_logo[0x30];
  union {
    char title[16];
    struct {
      char _unused0[12];
      char manufacturer[4];
    };
  };
  char license[2];
  uint8_t sgb_flag;
  uint8_t cartridge_type;
  uint8_t rom_size;
  uint8_t ram_size;
  char destination;
  char license_old;
  uint8_t version;
  uint8_t header_checksum;
  uint16_t global_checksum;


  uint8_t cgb_flag() { return title[15]; } 
  uint32_t rom_size_bytes() {
    if (rom_size < 10){
      return 0x8000 << rom_size;
    } else {
      switch (rom_size) {
        case 0x52:return 0x120000;
        case 0x53:return 0x140000;
        case 0x54:return 0x180000;
      }
    }
    return 0;
  }
 
  uint32_t ram_size_bytes() {
    if (cartridge_type==5 || cartridge_type==6) return 512;
    const int ramsizes[6] = { 
      0x00000000, 
      0x00000800, // 2K
      0x00002000, // 8K
      0x00008000, // 32K
      0x00020000, // 128K
      0x00010000  // 64K
    };
    auto size = ramsizes[ram_size];
    return size;
  }

};

class Cartridge : public Component {
 public:
  uint8_t hash;
  CartridgeHeader* header;
  Cartridge() : rom_(nullptr) {}
  ~Cartridge() {  }
  void Initialize(Emu* emu);
  void Deinitialize();
  void LoadFile(const char* filename, CartridgeHeader* header);
  void LoadRam();
  void SaveRam();
  uint8_t* rom() { return rom_; }
  uint8_t* GetMemoryPointer(uint16_t address);
  uint8_t Read(uint16_t address);
  void Write(uint16_t address, uint8_t data);
  void MBCStep(double dt);
  MemoryBankController* mbc;
 private:
  uint8_t* rom_;
  char cartridge_filename[256];
  char cartridge_path[1024];
  bool save_rtc;
};




}
}