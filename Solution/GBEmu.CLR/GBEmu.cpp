
#include "GBEmu.h"

namespace CLI {
  Class1::Class1()
  {
  }

  /*void Class1::Init() {

    emu.set_on_render([]() {
      //InvalidateRect(handle(),nullptr,true);
      //PostMessage(windowhandle, WM_PAINT, 0, 0);
      //Render();
      });

    emu.Initialize(emulation::gb::default_gb_hz);
    emu.set_mode(emulation::gb::EmuModeGB);

  }*/

  void Class1::LoadFile(System::String^ filename)
  {
    //emulation::gb::CartridgeHeader header;
   // emu.cartridge()->LoadFile(filename.c_str(), &header);

    int a = 1;
  }
  /*
  void Class1::Run() {

    emu.Run();
  }

  void Class1::Stop() {

    emu.Stop();
  }*/
}