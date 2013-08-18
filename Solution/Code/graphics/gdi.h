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
#ifndef UISYSTEM_GRAPHICS_GDI_H
#define UISYSTEM_GRAPHICS_GDI_H

#include <windows.h>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>


#include "graphics.h"

namespace graphics {

class GDI : public Graphics {
 public:
  GDI();
  ~GDI();
  void Initialize(HWND window_handle, int width, int height);
  void Deinitialize();
  void Render();
  void Clear(RGBQUAD color);
  void SetClippingArea(int x, int y, int width, int height);
  void BeginFill(RGBQUAD color, double alpha);
  void BeginGradientFill(const RGBQUAD* colors,const double* alphas, const double* ratios, int count, FillMode mode);
  void EndFill();
  void DrawRectangle(int x, int y, int width, int height);
  void DrawCircle(int x, int y, int radius);
  void DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
  void DrawLine(int x0, int y0, int x1, int y1);
  RGBQUAD* frame_buffer() const { return frame_buffer_; }
  RGBQUAD* back_buffer() const { return back_buffer_; }
 private:
  bool TestBoundry(int x,int y) {
   return ( x >= clip_.x && y >= clip_.y && x < clip_.width && y < clip_.height );
  }
 
  void SetPixel(int x, int y, RGBQUAD color) { 
    back_buffer_[x+(y*display_width_)] = color;
  }
  
  void FillPixel(int x, int y, double xs, double ys) {
    int index = x+(y*display_width_);
    if (fill_.mode == kSolid) { //Solid Fill
      back_buffer_[index] = interpolate_color(back_buffer_[index],fill_.colors[0],fill_.alphas[0]);
    }
    else if (fill_.mode == kGradientHorizontal) {
      RGBQUAD c = interpolate_color(fill_.colors[0],fill_.colors[1],xs);
      back_buffer_[index] = interpolate_color(back_buffer_[index],c,fill_.alphas[0]);
    }  
  }
  
  void ClearFill() {

  }
  

  BITMAPINFO bitmap_info;
  HBITMAP hbmp;
  HDC window_dc_;
  RGBQUAD* back_buffer_;
  RGBQUAD* frame_buffer_;
  Fill fill_;
  Clip clip_;
  //std::queue<Command> command_queue_;
};

}

#endif