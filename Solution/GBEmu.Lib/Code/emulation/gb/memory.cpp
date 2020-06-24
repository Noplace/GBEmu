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

/*
General Memory Map
  0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
  4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
  8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
  A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
  FE00-FE9F   Sprite Attribute Table (OAM)
  FEA0-FEFF   Not Usable
  FF00-FF7F   I/O Ports
  FF80-FFFE   High RAM (HRAM)
  FFFF        Interrupt Enable Register
  */

#include "gb.h"
#include "mbc/mbc.h"
#include "rom.h"

namespace emulation {
namespace gb {



void Memory::Initialize(Emu* emu) {
  Component::Initialize(emu);
  vram_ = new uint8_t[0x4000];
  wram_ = new uint8_t[0x8000];
  memset(vram_,0,0x4000);
  Reset();
}

void Memory::Deinitialize() {
  SafeDeleteArray(&wram_);
  SafeDeleteArray(&vram_);
}

void Memory::UpdateMemoryMap() {
  uint16_t rom_bank_number = 1;
  uint8_t ram_bank_number = 0;
  auto mbc = emu_->cartridge()->mbc;
  if (mbc != nullptr) {
    rom_bank_number = mbc->rom_bank_number;
    ram_bank_number = mbc->ram_bank_number;
    memmap[0x0A] = &mbc->eram()[ram_bank_number<<13];
    memmap[0x0B] = &mbc->eram()[0x1000|(ram_bank_number<<13)];
  }
  memmap[0x00] = (uint8_t*)&rom_[0x0000];
  memmap[0x01] = (uint8_t*)&rom_[0x1000];
  memmap[0x02] = (uint8_t*)&rom_[0x2000];
  memmap[0x03] = (uint8_t*)&rom_[0x3000];
  memmap[0x04] = (uint8_t*)&rom_[(rom_bank_number<<14)];
  memmap[0x05] = (uint8_t*)&rom_[(rom_bank_number<<14)|0x1000];
  memmap[0x06] = (uint8_t*)&rom_[(rom_bank_number<<14)|0x2000];
  memmap[0x07] = (uint8_t*)&rom_[(rom_bank_number<<14)|0x3000];
  memmap[0x08] = &vram_[(vram_select<<13)];
  memmap[0x09] = &vram_[(vram_select<<13)|0x1000];

  memmap[0x0C] = &wram_[0];
  memmap[0x0D] = &wram_[(wram_select<<12)];
  memmap[0x0E] = &wram_[0];
  memmap[0x0F] = &wram_[(wram_select<<12)];
}

void Memory::Reset() {
  wram_select = 1;
  vram_select = 0;
  ZeroMemory(ioports_,sizeof(ioports_));
  memset(joypadflags,0,sizeof(joypadflags));
  ioports_[0] = 0x0F;
  last_address = 0;
  memset(&dma_request, 0, sizeof(dma_request));
   /*ioports_[0x05] = 0x00; // TIMA
   ioports_[0x06] = 0x00; // TMA
   ioports_[0x07] = 0x00; // TAC
   ioports_[0x10] = 0x80; // NR10
   ioports_[0x11] = 0xBF; // NR11
   ioports_[0x12] = 0xF3; // NR12
   ioports_[0x14] = 0xBF; // NR14
   ioports_[0x16] = 0x3F; // NR21
   ioports_[0x17] = 0x00; // NR22
   ioports_[0x19] = 0xBF; // NR24
   ioports_[0x1A] = 0x7F; // NR30
   ioports_[0x1B] = 0xFF; // NR31
   ioports_[0x1C] = 0x9F; // NR32
   ioports_[0x1E] = 0xBF; // NR33
   ioports_[0x20] = 0xFF; // NR41
   ioports_[0x21] = 0x00; // NR42
   ioports_[0x22] = 0x00; // NR43
   ioports_[0x23] = 0xBF; // NR30
   ioports_[0x24] = 0x77; // NR50
   ioports_[0x25] = 0xF3; // NR51
   ioports_[0x26] = 0xF1; //NR52
   ioports_[0x40] = 0x91; // LCDC
   ioports_[0x42] = 0x00; // SCY
   ioports_[0x43] = 0x00; // SCX
   ioports_[0x45] = 0x00; // LYC
   ioports_[0x47] = 0xFC; // BGP
   ioports_[0x48] = 0xFF; // OBP0
   ioports_[0x49] = 0xFF; // OBP1
   ioports_[0x4A] = 0x00; // WY
   ioports_[0x4B] = 0x00; // WX
   ioports_[0xFF] = 0x00; // IE*/
  UpdateMemoryMap();
}


uint8_t* Memory::GetMemoryPointer(uint16_t address) {

  if (address >= 0x0000 && address <= 0x3FFF) {
    if (ioports_[0x50] == 0 && address < 0x100) { 
      return nullptr;//&dmgrom[address&0xFF];
    } else {
      return emu_->cartridge()->GetMemoryPointer(address);
    }
  } else if (address >= 0x4000 && address <= 0x7FFF) {
    return emu_->cartridge()->GetMemoryPointer(address);
  } else if (address >= 0x8000 && address <= 0x9FFF) {
    return &vram_[(address&0x1FFF)|(vram_select<<13)];
  } else if (address >= 0xA000 && address <= 0xBFFF) {
    return emu_->cartridge()->GetMemoryPointer(address);
  } else if (address >= 0xC000 && address <= 0xCFFF) {
    return &wram_[address&0x0FFF];
  } else if (address >= 0xD000 && address <= 0xDFFF) {
    return &wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xE000 && address <= 0xFDFF) {
    if ((address&0xF000)==0xE000)
      return &wram_[address&0x0FFF];
    else if ((address&0xF000)==0xF000)
      return &wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xFE00 && address <= 0xFE9F) {
    return &oam_[address-0xFE00];
  } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    return nullptr;
  } else if (address >= 0xFF00 && address <= 0xFF7F) {
    return nullptr;
  } else if (address >= 0xFF80 && address <= 0xFFFE) {
    return &hram_[address-0xFF80];
  } else if (address == 0xFFFF) {
    return nullptr;
  }

  return nullptr;
}

uint8_t Memory::Read8(uint16_t address) {
  uint8_t result = 0;

  /*
  memory map concept
  if ((address >= 0x8000 && address <= 0x9FFF)&&(emu_->lcd_driver()->stat().mode == 3)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
      return 0xFF;

  if (emu_->mode() == EmuModeGB && ioports_[0x50] == 0 && address < 0x100) { 
    result = dmgrom[address];
  } else if (emu_->mode() == EmuModeGBC &&  ioports_[0x50] == 0 && (address < 0x100 || (address >= 0x200 && address <= 0x8FF))) { 
    result = gbcrom[address];
  } else if (address < 0xFE00) {
    return memmap[address>>12][address&0xFFF];
  } else {
    if (address >= 0xFE00 && address <= 0xFE9F) {

    if ((emu_->lcd_driver()->stat().mode&0x2)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
      result = 0xFF;
    else
      result = oam_[address-0xFE00];
    } else if (address >= 0xFEA0 && address <= 0xFEFF) {
      int a = 1;
    } else if (address >= 0xFF00 && address <= 0xFF7F) {
      if (address >= 0xFF10 && address <= 0xFF3F) {
        result = emu_->apu()->Read(address);
      } else if (address >= 0xFF40 && address <= 0xFF4C) {
        result = emu_->lcd_driver()->Read(address);
      } else if (address >= 0xFF51 && address <= 0xFF55) { //HDMA
        result = emu_->lcd_driver()->Read(address);
      } else if (address >= 0xFF68 && address <= 0xFF6C) { //color pallete
        result = emu_->lcd_driver()->Read(address);
      } else if (address >= 0xFF04 && address <= 0xFF07) {
        result = emu_->timer()->Read(address);
      } else if (address == 0xFF0F) {
        result = 0xE0|ioports_[address&0xFF];
      } else {
        result = ioports_[address&0xFF];
      }
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
      result = hram_[address-0xFF80];
    } else if (address == 0xFFFF) {
      result = interrupt_enable_register_;
    }


  }*/




  
  if (address >= 0x0000 && address <= 0x3FFF) {
    if (emu_->mode() == EmuMode::EmuModeGB && ioports_[0x50] == 0 && address < 0x100) {
      result = dmgrom[address];
    } else if (emu_->mode() == EmuMode::EmuModeGBC &&  ioports_[0x50] == 0 && (address < 0x100 || (address >= 0x200 && address <= 0x8FF))) {
        result = gbcrom[address];
      //char str[255];
      //sprintf(str,"read gbc rom at 0x%04x\n",address);
      //OutputDebugString(str);
    } else {
      result = emu_->cartridge()->Read(address);
    }



  } else if (address >= 0x4000 && address <= 0x7FFF) {
    result = emu_->cartridge()->Read(address);
  } else if (address >= 0x8000 && address <= 0x9FFF) {

    if ((emu_->lcd_driver()->stat().mode == 3)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
      result = 0xFF;
    else
      result = vram_[(address&0x1FFF)|(vram_select<<13)];
  } else if (address >= 0xA000 && address <= 0xBFFF) {
    result = emu_->cartridge()->Read(address);//emu_->cartridge()->eram()[address&0x1FFF];
  } else if (address >= 0xC000 && address <= 0xCFFF) {
    result = wram_[address&0x0FFF];
  } else if (address >= 0xD000 && address <= 0xDFFF) {
    result = wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xE000 && address <= 0xFDFF) {
    if ((address&0xF000)==0xE000)
     result = wram_[address&0x0FFF];
    else if ((address&0xF000)==0xF000)
     result =  wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xFE00 && address <= 0xFE9F) {

    if ((emu_->lcd_driver()->stat().mode&0x2)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
      result = 0xFF;
    else
      result = oam_[address-0xFE00];
  } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    int a = 1;
  } else if (address >= 0xFF00 && address <= 0xFF7F) {
    if (address >= 0xFF10 && address <= 0xFF3F) {
      result = emu_->apu()->Read(address);
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
      result = emu_->lcd_driver()->Read(address);
    } else if (address >= 0xFF51 && address <= 0xFF55) { //HDMA
      result = emu_->lcd_driver()->Read(address);
    } else if (address >= 0xFF68 && address <= 0xFF6C) { //color pallete
      result = emu_->lcd_driver()->Read(address);
    } else if (address >= 0xFF04 && address <= 0xFF07) {
      result = emu_->timer()->Read(address);
    } else if (address == 0xFF0F) {
      result = 0xE0|ioports_[address&0xFF];
    }
    else  if (address == 0xFF4D) {
      if (emu_->mode() == EmuMode::EmuModeGB) {
        result = 0XFF;//always FF in DMG
      }
      else if (emu_->mode() == EmuMode::EmuModeGBC) {
        
        result = ((static_cast<int>(emu_->speed) << 6) & 0x80)|0x7F;

      }
    }
    else {
      result = ioports_[address&0xFF];
    }
  } else if (address >= 0xFF80 && address <= 0xFFFE) {
    result = hram_[address-0xFF80];
  } else if (address == 0xFFFF) {
    result = interrupt_enable_register_;
  }





  return result;
}

void Memory::Write8(uint16_t address, uint8_t data) {
  
  if (address >= 0x0000 && address <= 0x3FFF) {
    emu_->cartridge()->Write(address,data);
  } else if (address >= 0x4000 && address <= 0x7FFF) {
    emu_->cartridge()->Write(address,data);
  } else if (address >= 0x8000 && address <= 0x9FFF) {
    if ((emu_->lcd_driver()->stat().mode == 3)&&(emu_->lcd_driver()->lcdc().lcd_enable==1)) {
    } else {
      vram_[(address&0x1FFF)|(vram_select<<13)] = data;
    }
  } else if (address >= 0xA000 && address <= 0xBFFF) {
    emu_->cartridge()->Write(address,data);
  } else if (address >= 0xC000 && address <= 0xCFFF) {
     wram_[(address&0x0FFF)] = data;
  } else if (address >= 0xD000 && address <= 0xDFFF) {
     wram_[(address&0x0FFF)+(0x1000*wram_select)] = data;
  } else if (address >= 0xE000 && address <= 0xFDFF) {
    if ((address&0xF000)==0xE000)
     wram_[(address&0x0FFF)] = data;
    if ((address&0xF000)==0xF000)
     wram_[(address&0x0FFF)+(0x1000*wram_select)] = data;
  } else if (address >= 0xFE00 && address <= 0xFE9F) {
    if ((emu_->lcd_driver()->stat().mode&0x2)&&(emu_->lcd_driver()->lcdc().lcd_enable==1)) {
    } else {
     oam_[address-0xFE00] = data;
    }
  } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    //int a = 1;
     int a = 1;
  } else if (address >= 0xFF00 && address <= 0xFF7F) {
    
    if (address == 0xFF00) {
      ioports_[0]=data & ~0x0F;
    } else if (address == 0xFF01) {
      char str[255];
      sprintf_s(str,"%c",data);
      OutputDebugString(str);
    } else if (address >= 0xFF04 && address <= 0xFF07) {
      emu_->timer()->Write(address,data);
    } else if (address >= 0xFF10 && address <= 0xFF3F) {
      emu_->apu()->Write(address,data);
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
      emu_->lcd_driver()->Write(address,data);
    } else if ((address&0xFF) == 0x4F) { //FF4F - VBK - CGB Mode Only - VRAM Bank
      ioports_[address& 0x7F]=data;
      vram_select = data&0x1;
      UpdateMemoryMap();
    }
    else  if (address == 0xFF4D) {
      ioports_[0x4D] = data; //switch speed
    }
    else if (address >= 0xFF51 && address <= 0xFF55) { //HDMA
      emu_->lcd_driver()->Write(address,data);
    } else if (address >= 0xFF68 && address <= 0xFF6C) { //color pallete
      emu_->lcd_driver()->Write(address,data);
    } else if ((address&0xFF) == 0x70) { //FF70 - SVBK - CGB Mode Only - WRAM Bank
      ioports_[address& 0x7F]=data;
      wram_select = data&0x3;
      if (wram_select == 0) 
        wram_select = 1;

      UpdateMemoryMap();
    } else {
      ioports_[address&0x7F]=data;
    }
  } else if (address >= 0xFF80 && address <= 0xFFFE) {
    hram_[address-0xFF80] = data;

   

  } else if (address == 0xFFFF) {
    interrupt_enable_register_ = data;
  }
  
}

uint8_t Memory::ClockedRead8(uint16_t address) {
  emu_->MachineTick();
  return Read8(address);
}

void Memory::ClockedWrite8(uint16_t address, uint8_t data) {
  emu_->MachineTick();
  Write8(address,data);
}

void Memory::Tick() {


  //test dma
  if (emu_->cpu()->cpumode() == CpuModeNormal && emu_->cpu()->ticks_to_switchspeed == 0) {
    if (dma_request.transfer_counter != 0) {

      ++dma_request.clock_counter;
      if (dma_request.clock_counter == 4) { //every 4 clocks, one read write
        if (dma_request.transfer_counter != 161) { //skip first 4 clocks
          *dma_request.dest++ = Read8(dma_request.source_address++);
        }
        --dma_request.transfer_counter;
        dma_request.clock_counter = 0;
      }
    }
  }



  ioports_[0] |= 0x0F;

  if ((ioports_[0] & 0x10)==0) {
    for (int i=0;i<4;++i) {
      if (joypadflags[i]==true)//pressed
        ioports_[0] &= ~(1<<i);
      else
        ioports_[0] |= (1<<i);
    }
  }

  if ((ioports_[0] & 0x20)==0) {
    for (int i=0;i<4;++i) {
      if (joypadflags[i+4]==true)//pressed
        ioports_[0] &= ~(1<<i);
      else
        ioports_[0] |= (1<<i);
    }
  } 

  if ((ioports_[0]&0x0F) != 0x0F) 
    interrupt_flag() |= 16;

}

void Memory::JoypadPress(JoypadKeys key) {
  joypadflags[key] = true;
  emu_->cpu()->Wake();
  //ioports_[0] &= ~key;
}

}
}