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
#pragma once

//#pragma warning( disable:4800 )
#include "../../../Emu.Core/Code/utilities/types.h"
#include "../../../Emu.Core/Code/utilities/timer.h"
//#include <GBEmu/Solution/Emu.Core/Code/utilities/timer.h>

//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <condition_variable>
#include <mutex>

//#include <WinCore/log/log_manager.h>
//#include "../../debug.h"

namespace emulation {
namespace gb {
class Emu;
class Memory;
class Cartridge;
class LCDDriver;
class Apu;
class Cpu;
class MemoryBankController;
class Timer;
class Disassembler;
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
#include "apu.h"
#include "timer.h"
#include "cpu.h"
#include "disassembler.h"
#include "emu.h"
#include "debugger.h"