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


namespace emulation {
namespace gb {
	
const uint8_t dmgrom[0x100] = {
0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e, 0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3, 0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b, 0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9, 0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99, 0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04, 0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2, 0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62, 0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20, 0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17, 0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9, 0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c, 0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20, 0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};

/*const uint8_t dmgrom2[0x100] = {
0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e, 0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3, 0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b, 0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9, 0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99, 0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04, 0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2, 0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62, 0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20, 0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17, 0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9, 0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c, 0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20, 0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};*/

void Memory::Initialize(Emu* emu) {
  Component::Initialize(emu);
  vram_ = new uint8_t[0x2000];
	wram1_ = new uint8_t[0x1000];
	wram2_ = new uint8_t[0x1000];
  Reset();
}

void Memory::Deinitialize() {
	SafeDeleteArray(&wram2_);
	SafeDeleteArray(&wram1_);
	SafeDeleteArray(&vram_);
}

void Memory::Reset() {
	ZeroMemory(ioports_,sizeof(ioports_));
	memset(joypadflags,0,sizeof(joypadflags));
	ioports_[0] = 0x0F;
  last_address = 0;
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

uint8_t Memory::Read8(uint16_t address) {
  uint8_t result = 0;
  emu_->cpu()->Tick();emu_->cpu()->Tick();emu_->cpu()->Tick();emu_->cpu()->Tick(); 
	if (address >= 0x0000 && address <= 0x3FFF) {
    if (ioports_[0x50] == 0 && address < 0x100) { 
			result = dmgrom[address&0xFF];
    } else {
	    result = emu_->cartridge()->Read(address);
    }
	} else if (address >= 0x4000 && address <= 0x7FFF) {
		result = emu_->cartridge()->Read(address);
	} else if (address >= 0x8000 && address <= 0x9FFF) {

		if ((emu_->lcd_driver()->stat().mode == 3)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
			result = 0xFF;
    else
		  result = vram_[address&0x1FFF];
	} else if (address >= 0xA000 && address <= 0xBFFF) {
    result = emu_->cartridge()->Read(address);//emu_->cartridge()->eram()[address&0x1FFF];
	} else if (address >= 0xC000 && address <= 0xCFFF) {
		result = wram1_[address&0x0FFF];
	} else if (address >= 0xD000 && address <= 0xDFFF) {
		result = wram2_[address&0x0FFF];
	} else if (address >= 0xE000 && address <= 0xFDFF) {
	  if ((address&0xF000)==0xE000)
		 result = wram1_[address&0xFFF];
		else if ((address&0xF000)==0xF000)
		 result = wram2_[address&0xFFF];
	} else if (address >= 0xFE00 && address <= 0xFE9F) {

		if ((emu_->lcd_driver()->stat().mode&0x2)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
			result = 0xFF;
    else
		  result = oam_[address-0xFE00];
	} else if (address >= 0xFEA0 && address <= 0xFEFF) {
		int a = 1;
	} else if (address >= 0xFF00 && address <= 0xFF7F) {
		if (address >= 0xFF10 && address <= 0xFF3F) {
      ioports_[address&0xFF] = emu_->apu()->Read(address);
		} else if (address >= 0xFF40 && address <= 0xFF4B) {
			ioports_[address&0xFF] = emu_->lcd_driver()->Read(address);
		} else if (address >= 0xFF04 && address <= 0xFF07) {
      ioports_[address&0xFF] = emu_->timer()->Read(address);
		}
		result = ioports_[address&0xFF];
	} else if (address >= 0xFF80 && address <= 0xFFFE) {
		result = hram_[address-0xFF80];
	} else if (address == 0xFFFF) {
		result = interrupt_enable_register_;
	}

	return result;
}

void Memory::Write8(uint16_t address, uint8_t data) {
  emu_->cpu()->Tick();emu_->cpu()->Tick();emu_->cpu()->Tick();emu_->cpu()->Tick();
	if (address >= 0x0000 && address <= 0x3FFF) {
    emu_->cartridge()->Write(address,data);
	} else if (address >= 0x4000 && address <= 0x7FFF) {
		emu_->cartridge()->Write(address,data);
	} else if (address >= 0x8000 && address <= 0x9FFF) {
		if ((emu_->lcd_driver()->stat().mode == 3)&&(emu_->lcd_driver()->lcdc().lcd_enable==1)) {
    } else {
		 vram_[address&0x1FFF] = data;
    }
	} else if (address >= 0xA000 && address <= 0xBFFF) {
		emu_->cartridge()->Write(address,data);
	} else if (address >= 0xC000 && address <= 0xCFFF) {
		wram1_[address&0x0FFF] = data;
	} else if (address >= 0xD000 && address <= 0xDFFF) {
		wram2_[address&0x0FFF] = data;
	} else if (address >= 0xE000 && address <= 0xFDFF) {
		if ((address&0xF000)==0xE000)
		 wram1_[address&0xFFF] = data;
		if ((address&0xF000)==0xF000)
		 wram2_[address&0xFFF] = data;
	} else if (address >= 0xFE00 && address <= 0xFE9F) {
		if ((emu_->lcd_driver()->stat().mode&0x2)&&(emu_->lcd_driver()->lcdc().lcd_enable==1)) {
    } else {
		 oam_[address-0xFE00] = data;
    }
	} else if (address >= 0xFEA0 && address <= 0xFEFF) {
		//int a = 1;
     int a = 1;
	} else if (address >= 0xFF00 && address <= 0xFF7F) {
    ioports_[address&0xFF]=data;
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
    } else if (address >= 0xFF40 && address <= 0xFF4B) {
      emu_->lcd_driver()->Write(address,data);
    }
	} else if (address >= 0xFF80 && address <= 0xFFFE) {
		hram_[address-0xFF80] = data;
	} else if (address == 0xFFFF) {
		interrupt_enable_register_ = data;
	}
  
}

void Memory::Tick() {
  ioports_[0] |= 0x0F;
	if ((ioports_[0] & 0x30)==0x20) {
		for (int i=0;i<4;++i) {
			if (joypadflags[i]==true)//pressed
				ioports_[0] &= ~(1<<i);
			else
				ioports_[0] |= (1<<i);
		}
	} else if ((ioports_[0] & 0x30)==0x10) {
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