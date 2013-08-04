#include "gb.h"

namespace emulation {
namespace gb {

void Cartridge::Initialize(Emu* emu) {
  Component::Initialize(emu);
  rom_ = nullptr;
  mbc = nullptr;
}

void Cartridge::Deinitialize() {
  core::io::DestroyFileBuffer(&rom_);
  if (mbc)
    mbc->Deinitialize();
  SafeDelete(&mbc);
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
  this->header = (CartridgeHeader*)&rom_[0x100];
  if (header->cartridge_type == 0) {
    mbc = new MBCNone();
  } else if (header->cartridge_type == 1) {
    mbc = new MBC1();
  }
  mbc->Initialize(this);
  core::io::DestroyFileBuffer(&data);

  
}

uint8_t Cartridge::Read(uint16_t address) {
  return mbc->Read(address);
}

void Cartridge::Write(uint16_t address, uint8_t data) {
  mbc->Write(address,data);
}

}
}