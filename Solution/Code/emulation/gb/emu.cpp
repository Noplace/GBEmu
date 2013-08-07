#include "gb.h"

namespace emulation {
namespace gb {

void Emu::Initialize() {
  emulation::gb::CartridgeHeader header;
  cartridge_.Initialize(this);
  

	
  //cartridge_.ReadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\cpu_instrs\\individual\\01-special.gb",&header);
  cartridge_.LoadFile("D:\\Personal\\Projects\\GBEmu\\test\\cpu_instrs\\individual\\01-special.gb",&header);
	//cartridge_.ReadFile("D:\\Personal\\Projects\\GBEmu\\test\\cpu_instrs\\cpu_instrs.gb",&header);
	//cartridge_.ReadFile("D:\\Personal\\Projects\\GBEmu\\test\\Runtime - Test Rom (PD).gb",&header);
	//cartridge_.ReadFile("D:\\Personal\\Projects\\GBEmu\\test\\opus5.gb",&header);
  //cartridge_.ReadFile("D:\\Personal\\Projects\\GBEmu\\test\\Super Mario Land (World).gb",&header);
	timer_.Initialize(this);
  lcd_driver_.Initialize(this);
  memory_.Initialize(this);
  cpu_.Initialize(this);
}

void Emu::Deinitialize() {
  cpu_.Deinitialize();
  memory_.Deinitialize();
  lcd_driver_.Deinitialize();
	timer_.Deinitialize();
  cartridge_.Deinitialize();
}

double Emu::Step(double dt) {
	cpu_.Step(dt);
	double r = dt*cpu_.cycles; 
  return r;
}

}
}
   