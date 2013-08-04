#include "gb.h"


namespace emulation {
namespace gb {



void LCDDriver::Initialize(Emu* emu) {
  Component::Initialize(emu);
  lcdc.raw = 0;
  stat.raw = 0;
  ly = 0;
  lyc = 0;
  scroll_x = scroll_y = 0;
  wx = wy = 0;
  counter1 = counter2 = 0;
  vsync = 0;
  hsync = 0;
}

void LCDDriver::Deinitialize() {
  auto ioports = emu_->memory()->ioports();


}

void LCDDriver::Step(double dt) {
  counter1 += emu_->cpu()->cycles;
  hsync += dt;
  if (hsync >= 0.10871928680147858230050010871929) {
    ++ly;
    if (ly > 153) ly = 0;
    hsync = 0;
  }

  vsync += dt;
  if (vsync >= 16.742005692281935375858027791729) {
    vsync = 0;
    counter1 = 0;
  }
}

uint8_t LCDDriver::Read(uint16_t address) {

  switch (address) {
    case 0xFF40:
      return lcdc.raw;
    case 0xFF41:
      return stat.raw;
    case 0xFF42:
      return scroll_x;
    case 0xFF43:
      return scroll_y;
    case 0xFF44:
      return ly;
    case 0xFF45:
      return lyc;

    case 0xFF47:
      return bg_pallete_data;
    case 0xFF48:
      return obj_pallete1_data;
    case 0xFF49:
      return obj_pallete2_data;
    case 0xFF4A:
      return wx;
    case 0xFF4B:
      return wy;
  }
	return 0;
}

void LCDDriver::Write(uint16_t address, uint8_t data) {
  switch (address) {
    case 0xFF40:
      lcdc.raw = data;
      break;
    case 0xFF41:
      stat.raw = (data & ~0x7) | (stat.raw&0x7);
      break;
    case 0xFF42:
      scroll_x = data;
      break;
    case 0xFF43:
      scroll_y = data;
      break;
      break;
    case 0xFF44:
      ly = 0;
      break;
    case 0xFF45:
      lyc = data;
      break;
    case 0xFF46: {
      uint16_t srcaddr = data<<8;
      auto dest = emu_->memory()->oam();
      for (int i=0;i<160;++i)
        dest[i] = emu_->memory()->Read8(srcaddr++);
      break;
    }
    case 0xFF47:
      bg_pallete_data = data;
      bg_pal[0] = data&0x3;
      bg_pal[1] = (data>>2)&0x3;
      bg_pal[2] = (data>>4)&0x3;
      bg_pal[3] = (data>>6)&0x3;
      break;
    case 0xFF48:
      obj_pallete1_data = data;
      obj_pal1[0] = data&0x3;
      obj_pal1[1] = (data>>2)&0x3;
      obj_pal1[2] = (data>>4)&0x3;
      obj_pal1[3] = (data>>6)&0x3;
      break;
    case 0xFF49:
      obj_pallete2_data = data;
      obj_pal2[0] = data&0x3;
      obj_pal2[1] = (data>>2)&0x3;
      obj_pal2[2] = (data>>4)&0x3;
      obj_pal2[3] = (data>>6)&0x3;
      break;
    case 0xFF4A:
      wx = data;
      break;
    case 0xFF4B:
      wy = data;
      break;
  }
}

}
}