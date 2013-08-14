#include "gb.h"

namespace emulation {
namespace gb {

void Emu::Initialize() {
  emulation::gb::CartridgeHeader header;
  cartridge_.Initialize(this);
  
  //cartridge_.LoadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\oam_bug\\oam_bug\\rom_singles\\2-causes.gb",&header);
  //cartridge_.LoadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\instr_timing\\instr_timing\\instr_timing.gb",&header);
  //cartridge_.LoadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\mem_timing-2\\mem_timing-2\\mem_timing.gb",&header);
  //cartridge_.LoadFile("D:\\Personal\\Projects\\GBEmu\\test\\cpu_instrs\\individual\\01-special.gb",&header);
	//cartridge_.ReadFile("D:\\Personal\\Projects\\GBEmu\\test\\cpu_instrs\\cpu_instrs.gb",&header);
	//cartridge_.LoadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\PUZZLE.gb",&header);
	//cartridge_.LoadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\opus5.gb",&header);
  cartridge_.LoadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\Super Mario Land (World).gb",&header);
  //cartridge_.LoadFile("C:\\Users\\Khalid\\Documents\\GitHub\\GBEmu\\test\\Pokemon - Blue Version (UE) [S][!].gb",&header);
	timer_.Initialize(this);
  lcd_driver_.Initialize(this);
  memory_.Initialize(this);
  cpu_.Initialize(this);
  sc_.Initialize(this);
}

void Emu::Deinitialize() {
  sc_.Deinitialize();
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
   