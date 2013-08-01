#include "gb.h"

namespace emulation {
namespace gb {

void Cartridge::Initialize(Emu* emu) {
  Component::Initialize(emu);
}

void Cartridge::Deinitialize() {
  core::io::DestroyFileBuffer(&rom_);
}

void Cartridge::ReadFile(const char* filename, CartridgeHeader* header) {
  core::io::DestroyFileBuffer(&rom_);
  uint8_t* data;
  size_t length;
  core::io::ReadWholeFileBinary(filename,&data,length);
  memcpy(header,data+0x100,0x50);
  if (header->rom_size_bytes() == 0) {
    //report error
    return;
  }
  rom_ = new uint8_t[header->rom_size_bytes()];
  memcpy(rom_,data,header->rom_size_bytes());
  core::io::DestroyFileBuffer(&data);
}

}
}