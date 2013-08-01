#include "application.h"
#include "graphics/eagle.h"


void hq2x_filter_render(
  uint32_t *output, unsigned outputPitch,
  const uint32_t *input, unsigned inputPitch,
  unsigned width, unsigned height
);




namespace IO {
  extern HWND window_handle;
}

namespace app {

DisplayWindow::DisplayWindow() : Window() {
  output = new uint32_t[160*144];
//  counter = 0;
  instance = GetModuleHandle(nullptr);
}

DisplayWindow::~DisplayWindow() {
  delete []output;
}

void DisplayWindow::Init() {
  exit_signal_ = false;
  PrepareClass("GBEmu");
  window_class.hIcon = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,32,32,LR_DEFAULTSIZE));
  window_class.hIconSm = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,LR_DEFAULTSIZE));
  Create();
  SetMenu(LoadMenu(instance, MAKEINTRESOURCE(IDR_MENU1)));
  DragAcceptFiles(handle(),true);

  gfx.Initialize(handle(),0,0);
  
  /*nes.set_on_render([this]() {
    //InvalidateRect(handle(),nullptr,true);
    PostMessage(handle(),WM_PAINT,0,0);
  });

  nes.set_on_vertical_blank([this]() {
    MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0) {//while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  });*/

  //auto a = glGetError();
  //glPixelZoom(1, -1);
  //a = glGetError();
  //glRasterPos2i(0,0);
  //a = glGetError();
  glEnable( GL_TEXTURE_2D );
  auto a = glGetError();
  glGenTextures( 1, &texture );
  a = glGetError();
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  //wglMakeCurrent(NULL, NULL);

  //settings setup
  ResetTiming();
  
  OnCommand(ID_VIDEO_STD320X288,0);
  Center();
  ShowWindow(handle(), SW_SHOW); 
  UpdateWindow(handle()); 
  
  
  OnCommand(ID_MODE_NTSC,0);

  //emu_th = new std::thread(&app::DisplayWindow::Step,this);
  //emu_th->join();

  emu.Initialize();
}

void DisplayWindow::ResetTiming() {
  memset(&timing,0,sizeof(timing));
  timer.Calibrate();
  timing.prev_cycles = timer.GetCurrentCycles();
}

void DisplayWindow::Step() {
  //PostMessage(handle(),WM_PAINT,0,0);
 
  //gfx.SwitchThread();
  //while (exit_signal_ == false) {

    //if (nes.on == false) return;
    const double dt =  1000.0 / emulation::gb::clockspeed;//options.cpu_freq(); 0.00058f;//16.667f;
    timing.current_cycles = timer.GetCurrentCycles();
    double time_span =  (timing.current_cycles - timing.prev_cycles) * timer.resolution();
    if (time_span > 250.0) //clamping time
      time_span = 250.0;

    timing.span_accumulator += time_span;
    while (timing.span_accumulator >= dt) {
      timing.span_accumulator -= emu.Step(dt);
    }
    

    timing.total_cycles += timing.current_cycles-timing.prev_cycles;
    timing.prev_cycles = timing.current_cycles;
  

    //timing.render_time_span += time_span;
    //++timing.fps_counter;
    timing.fps_time_span += time_span;
    if (timing.fps_time_span >= 1000.0) {
      timing.fps = timing.fps_counter;
      timing.fps_counter = 0;
      timing.fps_time_span = 0;

      char caption[256];
      //sprintf(caption,"Freq : %0.2f MHz",nes.frequency_mhz());
      //sprintf(caption,"CPS: %llu ",nes.cycles_per_second());
    
      sprintf(caption,"GBEmu - FPS: %02d\0",timing.fps);
      SetWindowText(handle(),caption);
 
    }
    
  //}
}

int DisplayWindow::OnCreate(WPARAM wParam,LPARAM lParam) {
  return 0;
}

int DisplayWindow::OnDestroy(WPARAM wParam,LPARAM lParam) {
  exit_signal_ = true;
  PostQuitMessage(0);
  //emu_th->
  emu.Deinitialize();
  glDeleteTextures(1,&texture);
  gfx.Deinitialize();
  
  return 0;
}

int DisplayWindow::OnCommand(WPARAM wParam,LPARAM lParam) {
  if (HIWORD(wParam)==0) {
    
    if (LOWORD(wParam)==ID_VIDEO_STD320X288) {
      CheckMenuItem(menu_,ID_VIDEO_STD320X288,MF_CHECKED);
      CheckMenuItem(menu_,ID_VIDEO_EAGLE512X480,MF_UNCHECKED);
      SetClientSize(320,288);
      gfx.SetDisplaySize(320,288);
      display_mode = ID_VIDEO_STD320X288;
    }

    if (LOWORD(wParam)==ID_VIDEO_EAGLE512X480) {
      CheckMenuItem(menu_,ID_VIDEO_EAGLE512X480,MF_CHECKED);
      CheckMenuItem(menu_,ID_VIDEO_STD320X288,MF_UNCHECKED);
      SetClientSize(512,480);
      gfx.SetDisplaySize(512,480);
      display_mode = ID_VIDEO_EAGLE512X480;
    }

    if (LOWORD(wParam)==ID_MODE_NTSC) {
      CheckMenuItem(menu_,ID_MODE_NTSC,MF_CHECKED);
      CheckMenuItem(menu_,ID_MODE_PAL,MF_UNCHECKED);
      machine_mode = ID_MODE_NTSC;
    }

    if (LOWORD(wParam)==ID_MODE_PAL) {
      CheckMenuItem(menu_,ID_MODE_PAL,MF_CHECKED);
      CheckMenuItem(menu_,ID_MODE_NTSC,MF_UNCHECKED);
    
      machine_mode = ID_MODE_PAL;
    }
    

    if (LOWORD(wParam)==ID_FILE_EXIT) {
      PostQuitMessage(0);
    }

    if (LOWORD(wParam)==ID_MACHINE_RESET) {

      ResetTiming();
    }
     
    if (LOWORD(wParam)==ID_EDIT_OPTIONS) {
      options.Show(this);
    }
  
  }
  return 0;
}

int DisplayWindow::OnKeyDown(WPARAM wParam,LPARAM lParam) {

  /*if (wParam == 'Z')
    IO::joy1.a = true;
  if (wParam == 'X')
    IO::joy1.b = true;
  if (wParam == VK_RETURN)
    IO::joy1.start = true;
  if (wParam == VK_SPACE)
    IO::joy1.select = true;
  if (wParam == VK_UP)
    IO::joy1.up = true;
  if (wParam == VK_DOWN)
    IO::joy1.down = true;
  if (wParam == VK_LEFT)
    IO::joy1.left = true;
  if (wParam == VK_RIGHT)
    IO::joy1.right = true;*/

  return 0;
}

int DisplayWindow::OnKeyUp(WPARAM wParam,LPARAM lParam) {
  /*if (wParam == 'Z')
    IO::joy1.a = false;
  if (wParam == 'X')
    IO::joy1.b = false;
  if (wParam == VK_RETURN)
    IO::joy1.start = false;
  if (wParam == VK_SPACE)
    IO::joy1.select = false;
  if (wParam == VK_UP)
    IO::joy1.up = false;
  if (wParam == VK_DOWN)
    IO::joy1.down = false;
  if (wParam == VK_LEFT)
    IO::joy1.left = false;
  if (wParam == VK_RIGHT)
    IO::joy1.right = false;*/
  return 0;
}

int DisplayWindow::OnDropFiles(WPARAM wParam,LPARAM lParam) {
  auto len = DragQueryFile((HDROP)wParam,0,NULL,0);
  if (len==0)
    return 0;

  ++len;
  char* filename = new char[len];
  DragQueryFile((HDROP)wParam,0,filename,len);
  
  auto ext = strrchr(filename,'.');
  for (size_t i=0;i<strlen(ext);++i) {
    ext[i] = tolower(ext[i]);
  }

  if (strcmp(ext,".nes")==0) {
    //nes.Open(filename);
    OnCommand(ID_MACHINE_RESET,0);
  }

  SafeDeleteArray(&filename);

  return 0;
}

int DisplayWindow::OnPaint(WPARAM wParam,LPARAM lParam) {
  switch(display_mode) {
    case ID_VIDEO_STD320X288:
      //glDrawPixels(256,240,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
      //glEnable(GL_TEXTURE_2D);
      glBindTexture( GL_TEXTURE_2D, texture );
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,160,144,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
      glBegin( GL_QUADS );
      glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
      glTexCoord2d(1.0,0.0); glVertex2d(320,0.0);
      glTexCoord2d(1.0,1.0); glVertex2d(320,288);
      glTexCoord2d(0.0,1.0); glVertex2d(0.0,288);
      glEnd();
    break;
      
    case ID_VIDEO_EAGLE512X480:
      //ScaleImageEagle2X(nes.frame_buffer,256,240,output);
      glBindTexture( GL_TEXTURE_2D, texture );
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,512,480,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
      glBegin( GL_QUADS );
      glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
      glTexCoord2d(1.0,0.0); glVertex2d(256.0*2,0.0);
      glTexCoord2d(1.0,1.0); glVertex2d(256.0*2,240.0*2);
      glTexCoord2d(0.0,1.0); glVertex2d(0.0,240.0*2);
      glEnd();
      //hq2x_filter_render(output,256,nes.frame_buffer,256,256,240);
      //glDrawPixels(512,480,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
    break;
  }

  gfx.Render();
  timing.render_time_span = 0;
  ++timing.fps_counter;
  return 0;
}

}

