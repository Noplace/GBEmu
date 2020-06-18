#include "pch.h"
#include "Class1.h"

using namespace GBEmu_WRC;
using namespace Platform;

Class1::Class1()
{
}

void Class1::Init() {

  emu.set_on_render([]() {
    //InvalidateRect(handle(),nullptr,true);
    //PostMessage(windowhandle, WM_PAINT, 0, 0);
    //Render();
    });

  emu.Initialize(emulation::gb::default_gb_hz);
  emu.set_mode(emulation::gb::EmuModeGB);

}

void Class1::LoadFile(Platform::String filename)
{
  emulation::gb::CartridgeHeader header;
  emu.cartridge()->LoadFile(filename.c_str(), &header);
}

void Class1::Run() {

  emu.Run();
}

void Class1::Stop() {

  emu.Stop();
}