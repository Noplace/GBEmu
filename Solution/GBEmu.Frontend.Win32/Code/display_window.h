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

namespace app {

/*
  Class Name  : DisplayWindow
  Description : this is the application's main window
*/
class DisplayWindow: public core::windows::Window {
  public:
    DisplayWindow();
    ~DisplayWindow();
    void Init();
    void ResetTiming();
    void Step();
    //bool exit_signal() { return exit_signal_; }
  protected:
    int OnCommand(WPARAM wParam,LPARAM lParam);
    int OnKeyDown(WPARAM wParam,LPARAM lParam);
    int OnKeyUp(WPARAM wParam,LPARAM lParam);
    int OnCreate(WPARAM wParam,LPARAM lParam);
    int OnDestroy(WPARAM wParam,LPARAM lParam);
    int OnDropFiles(WPARAM wParam,LPARAM lParam);
    int OnResize(WPARAM wparam, LPARAM lparam);
    int OnPaint(WPARAM wParam,LPARAM lParam);
    int RenderNormal1();
    int RenderWithShaders1();
  private:
    //std::thread* emu_th;
    //std::atomic_bool exit_signal_;
    bool enable_stats_display;
    int display_mode;
    int machine_mode;
    uint32_t* output;
    uint32_t* dotmatrix_output;
    //graphics::GDI gfx;
    graphics::OpenGL gfx;
    emulation::gb::Emu emu;
    emulation::gb::CartridgeHeader header;
   
    dialogs::Options options;
    HINSTANCE instance;



    GLuint texture;
    GLuint vertex_shader, fragment_shader, prog;
    GLuint points_vbo;
    GLuint vao;
    GLint baseline_alpha_loc;
    GLint texture1_loc;

    int SetupShaders();
    int SetupVertexBuffer();
    int SetVertexBufferSize(float w, float h);

};

}

