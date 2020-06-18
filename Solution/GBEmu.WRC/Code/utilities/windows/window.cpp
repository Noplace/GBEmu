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
#include <stdio.h>
#include "windows.h"

namespace core {
namespace windows {

//core::collections::HashTable<HWND,Window*,100> Window::windows_map_;

Window::Window() {
  set_style(WS_BORDER|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
  has_menu_ = false;
  menu_ = nullptr;
}

Window::~Window() {
  Destroy();
}

void Window::SetMenu(HMENU menu) {
  if (menu!=nullptr) {
    ::SetMenu(handle_,menu);
    menu_ = menu;
    has_menu_ = true;
  } else {
    menu_ = nullptr;
    has_menu_ = false;
    ::SetMenu(handle_,nullptr);
  }
  SetClientSize(client_width_,client_height_);
}

void Window::SetClientSize(int width,int height) {
  client_width_ = width;
  client_height_ = height;
  RECT rectangle;
  SetRect(&rectangle,0,0,client_width_,client_height_);
  AdjustWindowRectEx(&rectangle,style(),has_menu_,0);
  
 
  int actual_width  = static_cast<int>(rectangle.right  - rectangle.left);
  int actual_height = static_cast<int>(rectangle.bottom - rectangle.top);

  SetWindowPos(handle(),0,0,0,actual_width,actual_height,SWP_NOACTIVATE|SWP_NOMOVE);  

}

void Window::Center() {
  RECT rectangle;
  GetWindowRect(handle(),&rectangle);
  
  int screen_width  = GetSystemMetrics(SM_CXSCREEN);
  int screen_height = GetSystemMetrics(SM_CYSCREEN);
  
  int left   = (screen_width - rectangle.right)/2;
  int top    = (screen_height - rectangle.bottom)/2;
  
  SetWindowPos(handle(),0,left,top,0,0,SWP_NOACTIVATE|SWP_NOSIZE);  
  
}

void Window::Show() {
  ShowWindow(handle(), SW_SHOW); 
  UpdateWindow(handle()); 
}

void Window::Hide() {
  ShowWindow(handle(), SW_HIDE); 
}

void Window::PrepareClass(char* title) {
  strcpy(this->title,title);
  sprintf(class_name_,"%s_class",title);
  memset(&window_class,0,sizeof(window_class));
  window_class.cbSize = sizeof(window_class);
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  window_class.lpfnWndProc = WindowProc;
  window_class.lpszClassName = class_name_;
  window_class.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
  window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)); 
}

void Window::Create() {
  class_ = RegisterClassEx(&window_class);
  CreateWindow(class_name_,title,WS_CLIPSIBLINGS | WS_CLIPCHILDREN | style(),0,0,100,100,NULL,NULL,NULL,this);
  SetWindowText(handle_,title);
}

void Window::Destroy() {
  UnregisterClass(class_name_,0);
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  
 
  switch( uMsg ) {
    case WM_NCCREATE: {
      CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
      Window* current_window = reinterpret_cast<Window*>(create_struct->lpCreateParams);
      current_window->handle_ = hwnd;
      SetWindowLongPtr(hwnd,GWLP_USERDATA,(ULONG_PTR)(current_window));
      return TRUE;
    }  
    break;
  }
  
  Window* current_window = reinterpret_cast<Window*>(static_cast<LONG_PTR>(GetWindowLongPtr(hwnd,GWLP_USERDATA)));
  LRESULT result = -1;
  if (current_window != NULL) {
    result = current_window->PreProcessMessages(hwnd,uMsg,wParam,lParam);
    if (result == -1) {
      switch( uMsg ) {
        case WM_CREATE: {
            result = current_window->OnCreate(wParam,lParam);
          }
          break;
        case WM_DESTROY: {
           result = current_window->OnDestroy(wParam,lParam);
         }
         break;
        case WM_COMMAND: {
            result = current_window->OnCommand(wParam,lParam);
          }
          break;
        case WM_CLOSE: {
            result = current_window->OnClose(wParam,lParam);
          }
          break;
        case WM_ACTIVATE : {
            result = current_window->OnActivate(wParam,lParam);
          }
          break;
        case WM_SIZE: {
            result = current_window->OnResize(wParam,lParam);
          }
          break;
        case WM_INPUT: {
            result = current_window->OnInput(wParam,lParam);
          }
          break;
        case WM_KEYDOWN: {
            result = current_window->OnKeyDown(wParam,lParam);
          }
          break;
        case WM_KEYUP: {
            result = current_window->OnKeyUp(wParam,lParam);
          }
          break;

        case WM_SYSKEYDOWN: {
            result = current_window->OnSysKeyDown(wParam,lParam);
          }
          break;
        case WM_SYSKEYUP: {
            result = current_window->OnSysKeyUp(wParam,lParam);
          }
          break;
        case WM_CAPTURECHANGED: {
            result = current_window->OnCaptureChanged(wParam,lParam);
          }
          break;
        case WM_MOUSEMOVE: {
            result = current_window->OnMouseMove(wParam,lParam);
          }
          break;
        case WM_MOUSEWHEEL: {
            result = current_window->OnMouseWheel(wParam,lParam);
          }
          break;
        case WM_LBUTTONDOWN: {
            result = current_window->OnLeftButtonDown(wParam,lParam);
          }
          break;
        case WM_LBUTTONUP: {
            result = current_window->OnLeftButtonUp(wParam,lParam);
          }
          break;
        case WM_MBUTTONDOWN: {
            result = current_window->OnMiddleButtonDown(wParam,lParam);
          }
          break;
        case WM_MBUTTONUP: {
            result = current_window->OnMiddleButtonUp(wParam,lParam);
          }
          break;
        case WM_RBUTTONDOWN: {
            result = current_window->OnRightButtonDown(wParam,lParam);
          }
          break;
        case WM_RBUTTONUP: {
            result = current_window->OnRightButtonUp(wParam,lParam);
          }
          break;
       case WM_DROPFILES: {
          result = current_window->OnDropFiles(wParam,lParam);
        }
       case WM_PAINT: {
         result = current_window->OnPaint(wParam,lParam);
       }
        default:
          result = -1;
          break;
      
      }
    }

    if (result == -1)
      result = DefWindowProc(hwnd,uMsg,wParam,lParam);
  }
  return result;
}


}
}