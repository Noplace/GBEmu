#pragma once

//#pragma warning( disable:4800 )
#include <WinCore/types.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>
#include <functional>
#include <thread>
#include <atomic>
//#include <WinCore/log/log_manager.h>
#include "../../debug.h"

namespace emulation {
namespace gb {
class Emu;
class Memory;
class Cartridge;
class LCDDriver;
class SoundController;
class Cpu;
class MemoryBankController;
class Timer;
class Component {
 public:
  virtual void Initialize(Emu* emu) {
    emu_ = emu;
  }
	virtual void Deinitialize() {
	}
	virtual void Reset() {
	}
 protected:
   Emu* emu_;
};

}
}

#include "cartridge.h"
#include "memory.h"
#include "lcd_driver.h"
#include "sound_controller.h"
#include "timer.h"
#include "cpu.h"
#include "emu.h"