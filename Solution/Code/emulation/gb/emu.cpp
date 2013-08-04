#include "gb.h"

namespace emulation {
namespace gb {

void Emu::Initialize() {
  frame_buffer = new uint32_t[160*144];
  emulation::gb::CartridgeHeader header;
  cartridge_.Initialize(this);
  
  cartridge_.ReadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\cpu_instrs\\individual\\01-special.gb",&header);
  //cartridge_.ReadFile("D:\\Personal\\Projects\\GBEmu\\test\\cpu_instrs\\cpu_instrs.gb",&header);
  //cartridge_.ReadFile("D:\\Personal\\Projects\\GBEmu\\test\\Super Mario Land (World).gb",&header);
  lcd_driver_.Initialize(this);
  memory_.Initialize(this);
  cpu_.Initialize(this);
  /*uint32_t shades[4] = {0xffffffff,0xffaaaaaa,0xff666666,0xff000000};
  for (int y=0;y<144;++y)
  {
    uint32_t *line = &output[y*160];
    for (int x=0;x<160;++x)
    {
      line[x] = 0;
    }
  }
  int logow = 9;
  int logoh = 8;
  for (int y=0;y<logoh;++y)
  {
    uint32_t *line = &output[y*160];
    for (int x=0;x<logow;++x)
    {
      *line++ = shades[(header.nintendo_logo[y*logow+x]>>0)&0x3];
      *line++ = shades[(header.nintendo_logo[y*logow+x]>>2)&0x3];
      *line++ = shades[(header.nintendo_logo[y*logow+x]>>4)&0x3];
      *line++ = shades[(header.nintendo_logo[y*logow+x]>>6)&0x3];
    }
  }*/
}

void Emu::Deinitialize() {
  cpu_.Deinitialize();
  memory_.Deinitialize();
  lcd_driver_.Deinitialize();
  cartridge_.Deinitialize();
  delete [] frame_buffer;  
}

double Emu::Step(double dt) {

	cpu_.Step(dt);
	double r = dt*cpu_.cycles; 
 // lcd_driver_.Step(r);
  return r;
}

}
}
   