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




  /*

  class PPU {
  public:
    uint8_t write(uint32_t address, uint8_t data) {
      std::cout << "possible?";
      return 0;
    }
  };


  class App {
    PPU ppu;
    Bus<uint32_t, uint8_t> bus;
  public:
    void run() {



      auto f = [](uint32_t a, uint8_t d) {
        std::cout << "hello";
        return (uint8_t)0;
      };
      bus.Register(0x0000, 0x2000, [&](uint32_t address, uint8_t data) {

        return ppu.write(address, data);
        });

      bus.Write(0x1000, 1);
    }
  };*/



void Memory::Initialize(Emu* emu) {
  Component::Initialize(emu);
  vram_ = new uint8_t[0x4000];
  wram_ = new uint8_t[0x8000];
  memset(vram_, 0, 0x4000);

  bus.Initialize();

  bus.RegisterRead(0x0000, 0x3FFF,std::bind(&Memory::ReadROM0,this, std::placeholders::_1));
  bus.RegisterRead(0x4000, 0x7FFF, std::bind(&Cartridge::Read, emu_->cartridge(), std::placeholders::_1));
  bus.RegisterRead(0x8000, 0x9FFF, std::bind(&Memory::ReadVRAM, this, std::placeholders::_1));
  bus.RegisterRead(0xA000, 0xBFFF, std::bind(&Cartridge::Read, emu_->cartridge(), std::placeholders::_1));
  bus.RegisterRead(0xC000, 0xCFFF, std::bind(&Memory::ReadWRAM0,this, std::placeholders::_1));
  bus.RegisterRead(0xD000, 0xDFFF, std::bind(&Memory::ReadWRAMX, this, std::placeholders::_1));
  bus.RegisterRead(0xE000, 0xFDFF, std::bind(&Memory::ReadWRAMEcho, this, std::placeholders::_1));






  bus.RegisterRead(0xFE00, 0xFE9F, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    if ((emu_->lcd_driver()->stat().mode & 0x2) && (emu_->lcd_driver()->lcdc().lcd_enable == 1))
      result = 0xFF;
    else
      result = oam_[address - 0xFE00];
    return result;
    });


  bus.RegisterRead(0xFEA0, 0xFEFF, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    
    return result;
    });


  bus.RegisterRead(0xFF00, 0xFF03, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = ioports_[address & 0x7F];
    return result;
    });

  bus.RegisterRead(0xFF04, 0xFF07, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = emu_->timer()->Read(address);
    return result;
    });

  bus.RegisterRead(0xFF08, 0xFF0E, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = ioports_[address & 0x7F];
    return result;
    });

  bus.RegisterRead(0xFF0F, 0xFF0F, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = 0xE0 | ioports_[address & 0x7F];
    return result;
    });


  bus.RegisterRead(0xFF10, 0xFF3F, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = emu_->apu()->Read(address);
    return result;
    });

  bus.RegisterRead(0xFF40, 0xFF4C, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = emu_->lcd_driver()->Read(address);
    return result;
    });



  bus.RegisterRead(0xFF4D, 0xFF4D, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    if (emu_->mode() == EmuMode::EmuModeGB) {
      result = 0XFF;//always FF in DMG
    } else if (emu_->mode() == EmuMode::EmuModeGBC) {

      result = ((static_cast<int>(emu_->speed) << 6) & 0x80) | 0x7F;

    }
    return result;
    });

  bus.RegisterRead(0xFF4E, 0xFF50, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = ioports_[address & 0x7F];

    if (address == 0xFF4F) {
      result |= 0xFE;
    }


    return result;
    });

  bus.RegisterRead(0xFF51, 0xFF55, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = emu_->lcd_driver()->Read(address);
    return result;
    });

  bus.RegisterRead(0xFF56, 0xFF67, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = ioports_[address & 0x7F];
    return result;
    });

  bus.RegisterRead(0xFF68, 0xFF6C, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = emu_->lcd_driver()->Read(address);
    return result;
    });

  bus.RegisterRead(0xFF6D, 0xFF7F, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = ioports_[address & 0x7F];
    return result;
    });


  bus.RegisterRead(0xFF80, 0xFFFE, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = hram_[address - 0xFF80];
    return result;
    });

  bus.RegisterRead(0xFFFF, 0xFFFF, [&](uint32_t address, uint8_t) {
    uint8_t result = 0;
    result = interrupt_enable_register_;
    return result;
    });


    
     // 
    


  bus.RegisterWrite(0x0000, 0x3FFF, [&](uint32_t address, uint8_t data) {
    emu_->cartridge()->Write(address, data);
    return 0;
    });

  bus.RegisterWrite(0x4000, 0x7FFF, [&](uint32_t address, uint8_t data) {
    emu_->cartridge()->Write(address, data);
    return 0;
    });


  bus.RegisterWrite(0x8000, 0x9FFF, [&](uint32_t address, uint8_t data) {
    if ((emu_->lcd_driver()->stat().mode == 3) && (emu_->lcd_driver()->lcdc().lcd_enable == 1)) {
    } else {
      vram_[(address & 0x1FFF) | (vram_select << 13)] = data;
    }
    return 0;
    });



  bus.RegisterWrite(0xA000, 0xBFFF, [&](uint32_t address, uint8_t data) {
    emu_->cartridge()->Write(address, data);
    return 0;
    });


  bus.RegisterWrite(0xC000, 0xCFFF, [&](uint32_t address, uint8_t data) {
    wram_[(address & 0x0FFF)] = data;
    return 0;
    });

  bus.RegisterWrite(0xD000, 0xDFFF, [&](uint32_t address, uint8_t data) {
    wram_[(address & 0x0FFF) + (0x1000 * wram_select)] = data;
    return 0;
    });

  bus.RegisterWrite(0xE000, 0xFDFF, [&](uint32_t address, uint8_t data) {
    if ((address & 0xF000) == 0xE000)
      wram_[(address & 0x0FFF)] = data;
    if ((address & 0xF000) == 0xF000)
      wram_[(address & 0x0FFF) + (0x1000 * wram_select)] = data;
    return 0;
    });

  bus.RegisterWrite(0xFE00, 0xFE9F, [&](uint32_t address, uint8_t data) {
    if ((emu_->lcd_driver()->stat().mode & 0x2) && (emu_->lcd_driver()->lcdc().lcd_enable == 1)) {
    } else {
      oam_[address - 0xFE00] = data;
    }
    return 0;
    });




  bus.RegisterWrite(0xFF00, 0xFF7F, [&](uint32_t address, uint8_t data) {
    if (address == 0xFF00) {
      ioports_[0] = data & ~0x0F;
    } else if (address == 0xFF01) {
      char str[255];
      sprintf_s(str, "%c", data);
      OutputDebugString(str);
    } else if (address >= 0xFF04 && address <= 0xFF07) {
      emu_->timer()->Write(address, data);
    } else if (address >= 0xFF10 && address <= 0xFF3F) {
      emu_->apu()->Write(address, data);
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
      emu_->lcd_driver()->Write(address, data);
    } else if ((address & 0xFF) == 0x4F) { //FF4F - VBK - CGB Mode Only - VRAM Bank
      ioports_[address & 0x7F] = data;
      vram_select = data & 0x1;
      
    } else  if (address == 0xFF4D) {
      ioports_[0x4D] = data; //switch speed
    } else if (address >= 0xFF51 && address <= 0xFF55) { //HDMA
      emu_->lcd_driver()->Write(address, data);
    } else if (address >= 0xFF68 && address <= 0xFF6C) { //color pallete
      emu_->lcd_driver()->Write(address, data);
    } else if (address == 0xFF70) { //FF70 - SVBK - CGB Mode Only - WRAM Bank
      ioports_[address & 0x7F] = data;
      wram_select = data & 0x3;
      if (wram_select == 0)
        wram_select = 1;

      
    } else {
      ioports_[address & 0x7F] = data;
    }
    return 0;
    });


  bus.RegisterWrite(0xFF80, 0xFFFE, [&](uint32_t address, uint8_t data) {
    hram_[address - 0xFF80] = data;
    return 0;
    });


  bus.RegisterWrite(0xFFFF, 0xFFFF, [&](uint32_t address, uint8_t data) {
    interrupt_enable_register_ = data;
    return 0;
    });


 

  Reset();
}

void Memory::Deinitialize() {
  SafeDeleteArray(&wram_);
  SafeDeleteArray(&vram_);
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

}

/*
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
}*/


uint8_t Memory::ReadROM0(uint16_t address ) {
  uint8_t result = 0;
  if (emu_->mode() == EmuMode::EmuModeGB && ioports_[0x50] == 0 && address < 0x100) {
    result = dmgrom[address];
  } else if (emu_->mode() == EmuMode::EmuModeGBC && ioports_[0x50] == 0 && (address < 0x100 || (address >= 0x200 && address <= 0x8FF))) {
    result = gbcrom[address];
    //char str[255];
    //sprintf(str,"read gbc rom at 0x%04x\n",address);
    //OutputDebugString(str);
  } else {
    result = emu_->cartridge()->Read(address);
  }
  return result;
}

uint8_t Memory::ReadROMX(uint16_t address) {
  uint8_t result = 0;
  return result;
}

uint8_t Memory::ReadVRAM(uint16_t address) {
  uint8_t result = 0;
  if ((emu_->lcd_driver()->stat().mode == 3) && (emu_->lcd_driver()->lcdc().lcd_enable == 1))
    result = 0xFF;
  else
    result = this->vram_[(address & 0x1FFF) | (vram_select << 13)];
  return result;
}

uint8_t Memory::ReadSRAM(uint16_t address) {
  uint8_t result = 0;

  return result;
}





uint8_t Memory::ReadWRAM0(uint16_t address) {
  uint8_t result = 0;
  result = wram_[address & 0x0FFF];
  return result;
}


uint8_t Memory::ReadWRAMX(uint16_t address) {
  uint8_t result = 0;
  result = wram_[(address & 0x0FFF) + (0x1000 * wram_select)];
  return result;
}

uint8_t Memory::ReadWRAMEcho(uint16_t address) {
  uint8_t result = 0;
  if ((address & 0xF000) == 0xE000)
    result = wram_[address & 0x0FFF];
  else if ((address & 0xF000) == 0xF000)
    result = wram_[(address & 0x0FFF) + (0x1000 * wram_select)];
  return result;
}


uint8_t Memory::Read8(uint16_t address) {


  return bus.Read(address);
  
}

void Memory::Write8(uint16_t address, uint8_t data) {
  bus.Write(address, data);
  
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