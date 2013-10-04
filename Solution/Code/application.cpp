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
#include "application.h"
#include <crtdbg.h>
namespace app {

Application* Application::current_app_ = nullptr;

Application::Application(HINSTANCE instance , LPSTR command_line, int show_command) {
  //OleInitialize(NULL);
  current_app_ = this;
}

Application::~Application() {
 current_app_ = nullptr;
 _CrtDumpMemoryLeaks();
}

int Application::Run() {
  CoInitializeEx(nullptr,COINIT_MULTITHREADED);
  //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  //unsigned old_fp_state;
  //_controlfp_s(&old_fp_state, _PC_53, _MCW_PC);

  time_t t;
  time(&t);
  srand((int)t);

  display_window_.Init();
  
  MSG msg;
  do {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      //display_window_.Step(); 
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
  } while(msg.message!=WM_QUIT);

 CoUninitialize();
 //Return the exit code to the system. 



 
 return static_cast<int>(msg.wParam);
}

}

