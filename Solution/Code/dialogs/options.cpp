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
#include "../application.h"
#include <CommCtrl.h>

#define CPU_MAX_FREQ 8000000.0
#define CPU_MIN_FREQ 1000000.0

template<typename TR> inline
TR convertRange(double old_min, double old_max, double new_min, double new_max, double val) {
  return TR(new_min + ( ((val - old_min) / (old_max-old_min)) * (new_max-new_min) ));
}

namespace app {
namespace dialogs {

Options::Options() : init_(false) {
  //values.cpu_freq = 1789800.0;
}

Options::~Options() {

}

void Options::Show(core::windows::Window* parent) {
  parent_ = parent;
  HWND parenthWnd = nullptr;
  if (parent_) {
    parenthWnd = parent_->handle();
  }
   
  create_result = DialogBoxParam(GetModuleHandle(nullptr),MAKEINTRESOURCE(IDD_OPTIONS),parenthWnd,DialogProc,(LPARAM)this);




}

void Options::OnInit(HWND hwndDlg) {
  handle_ = hwndDlg;
  auto slider1 = GetDlgItem(hwndDlg,IDC_SLIDER1);

  

  copy_to_volatile();

//  set_cpu_freq_text(nes_settings_volatile.cpu_freq_hz);
  WPARAM value = 0;//convertRange<WPARAM>(CPU_MIN_FREQ,CPU_MAX_FREQ,0.0,100.0,nes_settings_volatile.cpu_freq_hz);
  SendDlgItemMessage(handle_,IDC_SLIDER1,TBM_SETRANGEMIN,1,(LPARAM)0);
  SendDlgItemMessage(handle_,IDC_SLIDER1,TBM_SETRANGEMAX,1,(LPARAM)100);
  SendDlgItemMessage(handle_,IDC_SLIDER1,TBM_SETPOS,1,(LPARAM)value);
}

 

void Options::set_cpu_freq_text(double cpu_freq) {
  char str[25];
  sprintf(str,"%0.2f Mhz",cpu_freq*0.000001);
  SetDlgItemText(handle_,IDC_FREQ_LABEL,str);
}

void Options::copy_to_volatile() {

}

void Options::copy_from_volatile() {

}

INT_PTR CALLBACK Options::DialogProc(__in  HWND hwndDlg,__in  UINT uMsg,__in  WPARAM wParam,__in  LPARAM lParam) {
  
  Options* self = nullptr;
  if (uMsg == WM_INITDIALOG) {
    SetWindowLongPtr(hwndDlg,DWLP_USER,lParam);
    self = reinterpret_cast<Options*>(lParam);
    self->init_ = false;
    return TRUE;
  }
  self = reinterpret_cast<Options*>(GetWindowLongPtr(hwndDlg,DWLP_USER));
  if (self == nullptr)
    return FALSE;

  if (self->init_ == false) {
    self->OnInit(hwndDlg);
    self->init_ = true;
  }
  switch (uMsg) {
  	case WM_CLOSE:
		  EndDialog(hwndDlg,self->create_result);
      return TRUE;
    case WM_HSCROLL:{
        auto slider1 = GetDlgItem(hwndDlg,IDC_SLIDER1);
        if (slider1 == (HWND)lParam) {
          auto slider_value = SendMessage(slider1,TBM_GETPOS,0,0);
        }
      int a = 1;
  }
      break;

    case WM_COMMAND:{
      if (LOWORD(wParam) == IDOK) {
        self->copy_from_volatile();
        EndDialog(hwndDlg,self->create_result);
        return true;
      }
      if (LOWORD(wParam) == IDCANCEL) {
        EndDialog(hwndDlg,self->create_result);
        return true;
      }
  }
      break;
  };

  return FALSE;
}

}
}