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
#ifndef UISYSTEM_DISPLAY_WINDOW_H
#define UISYSTEM_DISPLAY_WINDOW_H

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
    bool exit_signal() { return exit_signal_; }
  protected:
    int OnCommand(WPARAM wParam,LPARAM lParam);
    int OnKeyDown(WPARAM wParam,LPARAM lParam);
    int OnKeyUp(WPARAM wParam,LPARAM lParam);
    int OnCreate(WPARAM wParam,LPARAM lParam);
    int OnDestroy(WPARAM wParam,LPARAM lParam);
    int OnDropFiles(WPARAM wParam,LPARAM lParam);
    int OnPaint(WPARAM wParam,LPARAM lParam);
    int Render();
  private:
    std::thread* emu_th;
    std::atomic_bool exit_signal_;
    int display_mode;
    int machine_mode;
    uint32_t* output;
    //graphics::GDI gfx;
    graphics::OpenGL gfx;
    emulation::gb::Emu emu;
    GLuint texture;
    dialogs::Options options;

    utilities::Timer timer;
    struct {
      uint64_t extra_cycles;
      uint64_t current_cycles;
      uint64_t prev_cycles;
      uint64_t total_cycles;
      uint32_t fps_counter;
      uint32_t ups_counter;
      uint32_t fps;
      uint32_t ups;
      double render_time_span;
      double fps_time_span;
      double span_accumulator;

    } timing;
    HINSTANCE instance;
};

}

#endif