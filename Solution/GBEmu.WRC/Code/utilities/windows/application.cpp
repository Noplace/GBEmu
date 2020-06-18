/*****************************************************************************************************************
* Copyright (c) 2012 Khalid Ali Al-Kooheji                                                                       *
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
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

namespace core {
namespace windows {


Application::Application() : ran_before_(nullptr) {
}

Application::Application(HINSTANCE instance , LPSTR command_line, int show_command) : ran_before_(nullptr) {

}

Application::~Application() {
  FreeConsole();
 if (ran_before_ != nullptr) {
   CloseHandle(ran_before_);
   ran_before_ = nullptr;
 }
}

void Application::InitConsole() {
  FreeConsole();
  AllocConsole();
  HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
  int hCrt = _open_osfhandle((intptr_t) handle_out, _O_TEXT);
  FILE* hf_out = _fdopen(hCrt, "w");
  setvbuf(hf_out, NULL, _IONBF, 1);
  *stdout = *hf_out;

  HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
  hCrt = _open_osfhandle((intptr_t) handle_in, _O_TEXT);
  FILE* hf_in = _fdopen(hCrt, "r");
  setvbuf(hf_in, NULL, _IONBF, 128);
  *stdin = *hf_in;
}

bool Application::RanBefore(LPCWSTR identifier) {
  ran_before_ = CreateMutex(NULL,TRUE,identifier);
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    return true;
  }
  return false;
}

}
}
