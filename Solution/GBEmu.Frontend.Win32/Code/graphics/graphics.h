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

namespace graphics {

enum FillMode    { kSolid , kGradientHorizontal, kGradientVertical, kGradientRadial };
enum CommandType { kBeginFill, kEndFill, kDrawRectangle , kDrawLine , kDrawTriangle };

struct Command {
  CommandType type;
  void* data;
};

struct Fill {
  RGBQUAD colors[4];
  double alphas[4];  
  double ratios[4];
  int count;
  FillMode mode;
};

struct Clip {
  int x;
  int y;
  int width;
  int height;
};

inline RGBQUAD Color(BYTE r,BYTE g,BYTE b) {
  RGBQUAD c;
  c.rgbRed = r;
  c.rgbGreen = g;
  c.rgbBlue = b;
  return c;
}

inline void RGBQUAD_to_float_rgba(RGBQUAD c, float* out) {
  out[0] = c.rgbRed / 255.0f; out[1] = c.rgbBlue / 255.0f; out[2] = c.rgbGreen / 255.0f; out[3] = 1.0f;
}


inline double interpolate_int(int a,int b,double s) {
  return a + s*(b-a);
}

inline double interpolate_double(double a,double b,double s) {
  return a + s*(b-a);
}

inline void swap_int(int& a,int& b) {
  int tmp;
  tmp = a;
  a   = b;
  b   = tmp;
}

inline RGBQUAD interpolate_color(RGBQUAD a,RGBQUAD b,double s) {
  RGBQUAD r;
  r.rgbRed   = static_cast<BYTE>(interpolate_int(a.rgbRed,b.rgbRed,s));
  r.rgbGreen = static_cast<BYTE>(interpolate_int(a.rgbGreen,b.rgbGreen,s));
  r.rgbBlue  = static_cast<BYTE>(interpolate_int(a.rgbBlue,b.rgbBlue,s));
  return r;
}

class Graphics {
 public:
  Graphics() {}
  virtual ~Graphics() {}
  void Initialize(HWND window_handle, int width, int height) {
    window_handle_ = window_handle;
    display_width_ = width;
    display_height_ = height;
  }
  void Deinitialize() {
  }
  virtual void Clear(RGBQUAD color) = 0;
  virtual void Render() = 0;
  HWND window_handle() { return window_handle_; }
 protected:
  HWND window_handle_;
  int display_width_;
  int display_height_;
};

}
