#pragma once
#include "Code/gbemu.h"
namespace GBEmu_WRC
{

    public ref class Class1 sealed
    {
    public:
        Class1();
        virtual ~Class1() {
          emu.Deinitialize();
        }
        void Init();
        void Run();
        void Stop();
        void LoadFile(Platform::String filename);
    private:
       emulation::gb::Emu emu;
    };
}
