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

class Shader {
    GLuint vertex_shader, fragment_shader, prog;

    template <int N>
    GLuint compile(GLuint type, char const *(&source)[N]) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, N, source, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &log[0]);
            throw std::logic_error(log);
            return false;
        }
        return shader;
    }
public:
    template <int N, int M>
    Shader(GLchar const *(&v_source)[N], GLchar const *(&f_source)[M]) {
        vertex_shader = compile(GL_VERTEX_SHADER, v_source);
        fragment_shader = compile(GL_FRAGMENT_SHADER, f_source);
        prog = glCreateProgram();
        glAttachShader(prog, vertex_shader);
        glAttachShader(prog, fragment_shader);
        glLinkProgram(prog);
    }

    operator GLuint() { return prog; }
    void operator()() { glUseProgram(prog); }

    ~Shader() {
        glDeleteProgram(prog);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
};

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
  init_ = true;
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
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 16;
  pfd.iLayerType = PFD_MAIN_PLANE;
  iFormat = ChoosePixelFormat( device_context_, &pfd );
  SetPixelFormat( device_context_, iFormat, &pfd );

  // create and enable the render context (RC)
  render_context_ = wglCreateContext( device_context_ );
  wglMakeCurrent( device_context_, render_context_ );
  
  //const GLubyte* str = glGetString(GL_EXTENSIONS);
  glShadeModel(GL_SMOOTH);
  SetDisplaySize(width,height);
}

void OpenGL::Deinitialize() {
  if (init_ == true) {
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( render_context_ );
    ReleaseDC( window_handle_, device_context_ );
    init_ = false;
  }
}

void OpenGL::SetDisplaySize(int width,int height) {
  display_width_ = width;
  display_height_ = height;
  glClearColor(0.25f, 0.25f, 0.5f, 0.0f);
  glViewport(0, 0, display_width_, display_height_);					// Reset The Current Viewport
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
}

void OpenGL::Clear(RGBQUAD color) {

}

void OpenGL::Render() {
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