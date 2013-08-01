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


  char cgb_flag() { return title[15]; } 
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
    switch (ram_size) {
      case 0: if (cartridge_type==5 || cartridge_type==6) return 256; else return 0;
      case 1:return 2*1024;
      case 2:return 8*1024;
      case 3:return 32*1024;
    }
  }

};

class Cartridge : public Component {
 public:
  Cartridge() : rom_(nullptr) {}
  ~Cartridge() {  }
  void Initialize(Emu* emu);
  void Deinitialize();
  void ReadFile(const char* filename, CartridgeHeader* header);
  const uint8_t* rom() { return rom_; }
 private:
  uint8_t* rom_;


};

}
}