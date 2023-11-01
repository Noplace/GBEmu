#pragma once

//#include "../GBEmu.Lib/Code/emulation/gb/gb.h"

namespace CLI
{

  public ref class Class1
  {
  public:
    Class1();
    virtual ~Class1() {
     // emu.Deinitialize();
    }
    

    //void Init();
    //void Run();
    //void Stop();
    void LoadFile(System::String^ filename);
  private:
    //emulation::gb::Emu emu;
  };
}
