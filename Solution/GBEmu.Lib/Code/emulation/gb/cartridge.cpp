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
#include "gb.h"
#include "mbc/mbc.h"

namespace emulation {
namespace gb {

Cartridge* thisc;

void Cartridge::Initialize(Emu* emu) {
  Component::Initialize(emu);
  rom_ = nullptr;
  mbc = nullptr;
  thisc = this;
  
}

void Cartridge::Deinitialize() {
  if (mbc) {
    //SaveRam();
    mbc->Deinitialize();
  }
  SafeDelete(&mbc);
  core::io::DestroyFileBuffer(&rom_);
  cart_loaded_ = false;
}

void Cartridge::LoadFile(const char* filename, CartridgeHeader* header) {
  cart_loaded_ = false;
  emu_->Reset();
  if (mbc) {
    mbc->Deinitialize();
    delete mbc;
  }
  SafeDeleteArray(&rom_);
  uint8_t* data;
  size_t length;
  core::io::ReadWholeFileBinary(filename,&data,length);
  if (data == nullptr) {
    header = nullptr;
    return;
  }
  memcpy(header,data+0x100,0x50);
  if (header->rom_size_bytes() == 0) {
    //report error
    return;
  }


  auto copy_size = max(header->rom_size_bytes(),length);
  rom_ = new uint8_t[copy_size];

  //rom_ = new uint8_t[header->rom_size_bytes()];

  

  memcpy(rom_, data, length);// header->rom_size_bytes());
  this->header = (CartridgeHeader*)&rom_[0x100];

  hash=0;
  for (int i=0;i<16;++i) 
    hash += header->title[i];


  if (header->cgb_flag() == 0xC0 ) {
    emu_->set_mode(EmuMode::EmuModeGBC); //force gbc
  } else if (header->cgb_flag() == 0x80) {
    //gbc optional, use user setting
  } else {
    emu_->set_mode(EmuMode::EmuModeGB); //force original gb
  }

  save_rtc = false;
  switch (header->cartridge_type) {
    case 0:
      mbc = new MBCNone();
      break;
    case 1:
    case 2:
    case 3:
      mbc = new MBC1();
      break;
    case 5:
    case 6:
      mbc = new MBC2();
      break;
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
      save_rtc = true;
      mbc = new MBC3();
      break;
    case 0x19:
    case 0x1A:
    case 0x1B:
      mbc = new MBC5();
      break;
    case 0xFC:
      mbc = new MBCPocketCamera();
      break;
    case 0xFD:
      mbc = new MBCTAMA5();
      break;
    default:
      DebugBreak();
  }
  core::io::DestroyFileBuffer(&data);
  const char* a = strrchr(filename,'\\');
  memset(cartridge_path,0,sizeof(cartridge_path));
  strncpy_s(cartridge_path,filename,a-filename+1);
  cartridge_path[a-filename+1] = '\0';
  strcpy_s(cartridge_filename,a+1);

  mbc->Initialize(this);
  emu_->memory()->rom_ = emu_->cartridge()->rom();

  cart_loaded_ = true;
}

void Cartridge::LoadRam() {
  uint8_t* data=nullptr;
  size_t length;
  char filename[256*5];
  sprintf_s(filename,"%s%s.ram",cartridge_path,cartridge_filename);
  core::io::ReadWholeFileBinary(filename,&data,length);
  if (data)
    memcpy(mbc->eram(),data,length);

  if (save_rtc == true) {
    sprintf_s(filename,"%s%s.rtc",cartridge_path,cartridge_filename);
    FILE* fp = null;
    fopen_s(&fp,filename, "rb");
    if (fp) {
      MBC3* mbc3 = (MBC3*)mbc;
      fread(&mbc3->rtc_timer,1,8,fp);
      fclose(fp);
    }
  }
}

void Cartridge::SaveRam() {
  char filename[256*5];
  sprintf_s(filename,"%s%s.ram",cartridge_path,cartridge_filename);
  FILE* fp = null;
  fopen_s(&fp, filename, "wb");
  fwrite(mbc->eram(),1,header->ram_size_bytes(),fp);
  fclose(fp);

  if (save_rtc == true) {
    sprintf_s(filename,"%s%s.rtc",cartridge_path,cartridge_filename);
    FILE* fp = null;
    fopen_s(&fp, filename, "wb");
    MBC3* mbc3 = (MBC3*)mbc;
    fwrite(&mbc3->rtc_timer,1,8,fp);
    fclose(fp);
  }
}

uint8_t* Cartridge::GetMemoryPointer(uint16_t address) {
  return mbc->GetMemoryPointer(address);
}

uint8_t Cartridge::Read(uint16_t address) {
  return mbc->Read(address);
}

void Cartridge::Write(uint16_t address, uint8_t data) {
  mbc->Write(address,data);
}

void Cartridge::MBCStep(double dt) {
  mbc->Step(dt);
}



}
}


char* mbc3_rtc_test() {
  emulation::gb::MBC3* mbc3 = (emulation::gb::MBC3*)emulation::gb::thisc->mbc;

  static char str[256];
  int day = mbc3->rtc[3];
  day |= mbc3->rtc[4] << 8;
  day |= (mbc3->rtc[4]>>7) << 9;
  sprintf_s(str,"timer:%I64d - days:%d hours:%d mins:%d secs:%d",mbc3->rtc_timer,day,mbc3->rtc[2], mbc3->rtc[1], mbc3->rtc[0]);
  return str;
}