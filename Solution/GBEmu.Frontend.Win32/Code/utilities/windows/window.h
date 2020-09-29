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
#pragma once

#include <GBEmu/Solution/Emu.Core/Code/utilities/hash_table.h>

namespace core {
namespace windows {

/*
  Class Name  : Window
  Description : this is a base class to use for creating windows
*/
class Window {
 public:
  Window();
  ~Window();
  void SetMenu(HMENU menu);
  void SetClientSize(int width,int height);
  void Center();
  void Show();
  void Hide();
  HWND handle() { return handle_; }
  DWORD style() { return style_; }
  void set_style(DWORD style) { style_ = style; }
 protected:
  LONG client_width_,client_height_;
  WNDCLASSEXA window_class;
  HMENU menu_;
  void PrepareClass(char* title);
  void Create();
  void Destroy();
  virtual int OnCreate(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnDestroy(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnCommand(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnResize(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnMouseMove(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnMouseWheel(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnLeftButtonDown(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnLeftButtonUp(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnMiddleButtonDown(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnMiddleButtonUp(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnRightButtonDown(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnRightButtonUp(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnCaptureChanged(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnInput(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnKeyDown(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnKeyUp(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnSysKeyDown(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnSysKeyUp(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnSysCommand(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnClose(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnActivate(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnDropFiles(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int OnPaint(WPARAM wParam,LPARAM lParam) { return -1; }
  virtual int PreProcessMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }
 protected:
  bool has_menu_;
  char title[255];
  char class_name_[255];
  HWND handle_;
  DWORD style_;
  ATOM class_;
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

}
}

