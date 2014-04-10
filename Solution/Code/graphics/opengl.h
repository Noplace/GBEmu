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
#ifndef UISYSTEM_GRAPHICS_OPENGL_H
#define UISYSTEM_GRAPHICS_OPENGL_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#define GL_GLEXT_PROTOTYPES
#include <gl/glext.h>
#include "graphics.h"
#include "shader.h"

namespace graphics {

class OpenGL : public Graphics {
 public:
  OpenGL();
  ~OpenGL();
  void Initialize(HWND window_handle,int width,int height);
  void Deinitialize();
  void SetDisplaySize(int width,int height);
  void PrintText(int x,int y,const char* str,size_t len);
  void Clear(RGBQUAD color);
  void Render();
  void SwitchThread() {
    wglMakeCurrent( device_context_, render_context_ );
  }
  GLuint CreateTexture();
 private:
  HDC device_context_;
  HGLRC render_context_;
  GLuint fontlistbase;
  bool init_;
};

}

#endif