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
#include "opengl.h"

#pragma comment(lib,"opengl32")
#pragma comment(lib,"glu32")

namespace graphics {


OpenGL::OpenGL() :init_(false),device_context_(nullptr),render_context_(nullptr) {

}

OpenGL::~OpenGL() {
  Deinitialize();
}

void OpenGL::Initialize(HWND window_handle,int width,int height) {
  PIXELFORMATDESCRIPTOR pfd;
  int iFormat;
  if (init_ == true)
    return;
  Graphics::Initialize(window_handle,width,height);

  // get the device context (DC)
  device_context_ = GetDC( window_handle_ );
  if (device_context_ == nullptr) {
    //error
    return;
  }
  // set the pixel format for the DC
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize = sizeof( pfd );
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI |
                PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 24;
  pfd.iLayerType = PFD_MAIN_PLANE;
  iFormat = ChoosePixelFormat( device_context_, &pfd );
  SetPixelFormat( device_context_, iFormat, &pfd );
  auto e = glGetError();
  // create and enable the render context (RC)
  render_context_ = wglCreateContext(device_context_);
  e = glGetError();
  wglMakeCurrent(device_context_,render_context_);
  e = glGetError();
  //const GLubyte* str = glGetString(GL_EXTENSIONS);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  e = glGetError();
  SetDisplaySize(width,height);
  
  fontlistbase = 1000;
  SelectObject(device_context_, GetStockObject (SYSTEM_FONT)); 
  wglUseFontBitmaps(device_context_, 0, 255, fontlistbase); 
  init_ = true;
}

void OpenGL::Deinitialize() {
  if (init_ == true) {
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( render_context_ );
    ReleaseDC(window_handle_,device_context_);
    init_ = false;
  }
}

void OpenGL::SetDisplaySize(int width,int height) {
  display_width_ = width;
  display_height_ = height;
  glClearColor(0.25f, 0.25f, 0.5f, 0.0f);
  auto e = glGetError();
  glViewport(0, 0, display_width_, display_height_);					// Reset The Current Viewport
  e = glGetError();
	/*glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)display_width_/(GLfloat)display_height_,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
  */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, display_width_, display_height_, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();							// Reset The Modelview Matrix
  e = glGetError();
}

void OpenGL::PrintText(int x,int y,const char* str,size_t len) {

  glPushAttrib(GL_LIST_BIT);
  glPushAttrib(GL_LIGHTING_BIT);
  glPushAttrib(GL_TEXTURE_BIT);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  glColor3ub(0xFF,0xFF,0xFF);
  glRasterPos2i(x,y);
  glListBase(fontlistbase);
  glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);


  glPopAttrib();
  glPopAttrib();
  glPopAttrib();
}

void OpenGL::Clear(RGBQUAD color) {

}

void OpenGL::Render() {
 /* HFONT font,oldfont;
  font = (HFONT)GetStockObject(ANSI_VAR_FONT);
  if (oldfont = (HFONT)SelectObject(device_context_,font)) {
    TextOut(device_context_,0,0,"Hello World",11);
    SelectObject(device_context_,oldfont);
  }*/
  //PAINTSTRUCT ps;
  //auto hdc = BeginPaint(handle(), &ps);
  // Draw the text string
  //SetTextColor(device_context_, RGB(255, 0, 0));

  // Release the device context
  //EndPaint(handle(), &ps);
  SwapBuffers(device_context_);
}


GLuint OpenGL::CreateTexture() {
  GLuint texture;
  glEnable( GL_TEXTURE_2D );
  auto a = glGetError();
  glGenTextures( 1, &texture );
  a = glGetError();
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  
  return texture;
}

}