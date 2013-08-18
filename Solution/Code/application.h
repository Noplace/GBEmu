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
#ifndef UISYSTEM_APPLICATION_H
#define UISYSTEM_APPLICATION_H

#include "debug.h"
#include <math.h>
#include <WinCore/windows/windows.h>
#include <WinCore/windows/application.h>
#include <WinCore/timer/timer2.h>
#include <Shellapi.h>
#include <thread>
#include <atomic>
#include "emulation/gb/gb.h"
#include "graphics/gdi.h"
#include "graphics/opengl.h"
#include "resource.h"
#include "dialogs/options.h"
#include "display_window.h"


namespace app {

/*
  Class Name  : Application
  Description : this is the application's class
*/
class Application : core::windows::Application {
  public:
    explicit Application(HINSTANCE instance , LPSTR command_line, int show_command);
    ~Application();
    int Run();
    DisplayWindow& display_window() {
      return display_window_;
    }
    static Application* Current() {
      return current_app_;
    }
  protected:
    static Application* current_app_;
  private:
    DisplayWindow display_window_;
};



}

#endif