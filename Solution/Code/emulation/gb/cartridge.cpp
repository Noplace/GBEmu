#include "gb.h"
#include "mbc.h"

namespace emulation {
namespace gb {

void Cartridge::Initialize(Emu* emu) {
  Component::Initialize(emu);
  rom_ = nullptr;
  mbc = nullptr;
}

void Cartridge::Deinitialize() {
  if (mbc) {
    SaveRam();
    mbc->Deinitialize();
  }
  SafeDelete(&mbc);
  core::io::DestroyFileBuffer(&rom_);
}

void Cartridge::LoadFile(const char* filename, CartridgeHeader* header) {
	SafeDeleteArray(&rom_);
  if (mbc) {
    mbc->Deinitialize();
		delete mbc;
	}
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

  switch (header->cartridge_type) {
    case 0:
      mbc = new MBCNone();
      break;
    case 1:
    case 2:
    case 3:
      mbc = new MBC1();
      break;
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
      mbc = new MBC3();
      break;
    default:
      DebugBreak();
  }

  mbc->Initialize(this);
  LoadRam();
  core::io::DestroyFileBuffer(&data);

  
}

void Cartridge::LoadRam() {
  uint8_t* data=nullptr;
  size_t length;
  char filename[256];
  sprintf(filename,"%s.ram",header->title);
  core::io::ReadWholeFileBinary(filename,&data,length);
  if (data)
    memcpy(mbc->eram(),data,length);
}

void Cartridge::SaveRam() {
  char filename[256];
  sprintf(filename,"%s.ram",header->title);
  FILE* fp = fopen(filename,"wb");
  fwrite(mbc->eram(),1,header->ram_size_bytes(),fp);
  fclose(fp);
}

uint8_t Cartridge::Read(uint16_t address) {
  return mbc->Read(address);
}

void Cartridge::Write(uint16_t address, uint8_t data) {
  mbc->Write(address,data);
}



}
}