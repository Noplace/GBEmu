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
#include "application.h"
#include "graphics/eagle.h"


void hq2x_filter_render(
  uint32_t *output, unsigned outputPitch,
  const uint32_t *input, unsigned inputPitch,
  unsigned width, unsigned height
);


void dotmatrix_sim(uint32_t* inbuf,uint32_t* outbuf) {

  for(int y =0;y<256;++y) {
    auto srcline = &inbuf[y<<8];
    
    for (int x=0;x<256;++x) {
      //
      for (int z=0;z<5;++z) {
        auto destline = &outbuf[((y*6)+z)*(256*6)];
        destline[x*6] = srcline[x];
        destline[x*6+1] = srcline[x];
        destline[x*6+2] = srcline[x];
        destline[x*6+3] = srcline[x];
        destline[x*6+4] = srcline[x];
        destline[x*6+5] = 0xFF808E0B;
      }
        auto destline = &outbuf[((y*6)+5)*(256*6)];
        destline[x*6] = 0xFF808E0B;
        destline[x*6+1] = 0xFF808E0B;
        destline[x*6+2] = 0xFF808E0B;
        destline[x*6+3] = 0xFF808E0B;
        destline[x*6+4] = 0xFF808E0B;
        destline[x*6+5] = 0xFF808E0B;
    }
  }

}

namespace app {

DisplayWindow::DisplayWindow() : Window() {

//  counter = 0;
  instance = GetModuleHandle(nullptr);
}

DisplayWindow::~DisplayWindow() {

}

void DisplayWindow::Init() {
  output = new uint32_t[512*512];
  dotmatrix_output = new uint32_t[256*256*6*6];
//  exit_signal_ = false;
  PrepareClass("GBEmu");
  window_class.hIcon = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,32,32,LR_DEFAULTSIZE));
  window_class.hIconSm = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,LR_DEFAULTSIZE));
  set_style(style()|WS_SIZEBOX);
  Create();
  SetMenu(LoadMenu(instance, MAKEINTRESOURCE(IDR_MENU1)));
  DragAcceptFiles(handle(),true);

  gfx.Initialize(handle(),0,0);
  HWND windowhandle = handle();
  emu.set_on_render([windowhandle]() {
    //InvalidateRect(handle(),nullptr,true);
    SendMessage(windowhandle,WM_PAINT,0,0);
    //Render();
  });

  //auto a = glGetError();
  //glPixelZoom(1, -1);
  //a = glGetError();
  //glRasterPos2i(0,0);
  //a = glGetError();
  texture = gfx.CreateTexture();
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
  emu.Initialize(emulation::gb::default_gb_hz);
  //emu.lcd_driver()->lcdscreenmode_
  OnCommand(ID_MODE_GBC,0);
  emulation::gb::CartridgeHeader header;
  //emu.cartridge()->LoadFile("..\\test\\cpu_instrs\\cpu_instrs.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\instr_timing\\instr_timing\\instr_timing.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\interrupt_time\\interrupt_time\\interrupt_time.gb",&header);
  
  //emu.cartridge()->LoadFile("..\\test\\instr_timing\\instr_timing\\instr_timing.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\mem_timing-2\\mem_timing-2\\mem_timing.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\cgb_sound\\cgb_sound\\rom_singles\\03-trigger.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\oam_bug\\oam_bug\\rom_singles\\8-instr_effect.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\oam_bug\\oam_bug\\rom_singles\\6-timing_no_bug.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\halt_bug\\halt_bug.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\SPRITE.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\opus5.gb",&header);
  emu.cartridge()->LoadFile("..\\test\\Super Mario Land (World).gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\Pocket Camera (Japan) (Rev A).gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\Pokemon - Blue Version (UE) [S][!].gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\Legend of Zelda, The - Link's Awakening (U) (V1.2) [!].gb",&header);//not original rom, problem with window
  //emu.cartridge()->LoadFile("..\\test\\Final Fantasy Legend, The (U) [!].gb",&header); 
  //emu.cartridge()->LoadFile("D:\\Personal\\Dev\\GB\\roms\\Kirby's Dream Land (USA, Europe).gb",&header);

  //emu.cartridge()->LoadFile("..\\test\\Demotronic Final Demo (PD) [C].gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\Game Boy Color Promotional Demo (USA, Europe).gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\introcollection.gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\pht-mr.gbc",&header);


  //emu.cartridge()->LoadFile("..\\test\\Legend of Zelda, The - Link's Awakening DX (USA, Europe).gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\Pokemon Silver.gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\Grand Theft Auto.gbc",&header);
  emu.Run();
}

void DisplayWindow::ResetTiming() {
  
}

void DisplayWindow::Step() {

}

int DisplayWindow::OnCreate(WPARAM wParam,LPARAM lParam) {
  return 0;
}

int DisplayWindow::OnDestroy(WPARAM wParam,LPARAM lParam) {
  //exit_signal_ = true;
  //
  emu.Stop();
  emu.Deinitialize();
  glDeleteTextures(1,&texture);
  gfx.Deinitialize();
  delete []output;
  SafeDeleteArray(&dotmatrix_output);
  PostQuitMessage(0);
  return 0;
}

int DisplayWindow::OnCommand(WPARAM wParam,LPARAM lParam) {

  switch (LOWORD(wParam)) {
    case ID_MODE_GB:
      CheckMenuItem(menu_,ID_MODE_GB,MF_CHECKED);
      CheckMenuItem(menu_,ID_MODE_GBC,MF_UNCHECKED);
      emu.set_mode(emulation::gb::EmuModeGB); 
      break;
    case ID_MODE_GBC:
      CheckMenuItem(menu_,ID_MODE_GBC,MF_CHECKED);
      CheckMenuItem(menu_,ID_MODE_GB,MF_UNCHECKED);
      emu.set_mode(emulation::gb::EmuModeGBC); 
      break;
  }

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
      SetClientSize(640,288*2);
      gfx.SetDisplaySize(640,288*2);
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
      emu.Reset();
      ResetTiming();
      emu.Run();
    }

    if (LOWORD(wParam)==ID_MACHINE_PAUSE) {
      emu.Pause();
    }
     
    if (LOWORD(wParam)==ID_EDIT_OPTIONS) {
      options.Show(this);
    }
  
  }
  return 0;
}

int DisplayWindow::OnKeyDown(WPARAM wParam,LPARAM lParam) {

  if (wParam == 'Z')
    emu.memory()->JoypadPress(emulation::gb::JoypadA);
  if (wParam == 'X')
    emu.memory()->JoypadPress(emulation::gb::JoypadB);
  if (wParam == VK_RETURN)
    emu.memory()->JoypadPress(emulation::gb::JoypadStart);
  if (wParam == VK_SPACE)
    emu.memory()->JoypadPress(emulation::gb::JoypadSelect);
  if (wParam == VK_UP)
    emu.memory()->JoypadPress(emulation::gb::JoypadUp);
  if (wParam == VK_DOWN)
    emu.memory()->JoypadPress(emulation::gb::JoypadDown);
  if (wParam == VK_LEFT)
    emu.memory()->JoypadPress(emulation::gb::JoypadLeft);
  if (wParam == VK_RIGHT)
    emu.memory()->JoypadPress(emulation::gb::JoypadRight);

  return 0;
}

int DisplayWindow::OnKeyUp(WPARAM wParam,LPARAM lParam) {
  if (wParam == 'Z')
    emu.memory()->JoypadRelease(emulation::gb::JoypadA);
  if (wParam == 'X')
    emu.memory()->JoypadRelease(emulation::gb::JoypadB);
  if (wParam == VK_RETURN)
    emu.memory()->JoypadRelease(emulation::gb::JoypadStart);
  if (wParam == VK_SPACE)
    emu.memory()->JoypadRelease(emulation::gb::JoypadSelect);
  if (wParam == VK_UP)
    emu.memory()->JoypadRelease(emulation::gb::JoypadUp);
  if (wParam == VK_DOWN)
    emu.memory()->JoypadRelease(emulation::gb::JoypadDown);
  if (wParam == VK_LEFT)
    emu.memory()->JoypadRelease(emulation::gb::JoypadLeft);
  if (wParam == VK_RIGHT)
    emu.memory()->JoypadRelease(emulation::gb::JoypadRight);
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

int DisplayWindow::Render() {
  return 0;
}

int DisplayWindow::OnResize(WPARAM wparam, LPARAM lparam) {
  gfx.SetDisplaySize(LOWORD(lparam),HIWORD(lparam));
  client_width_ = LOWORD(lparam);
  client_height_ = HIWORD(lparam);
  return 0;
}

int DisplayWindow::OnPaint(WPARAM wparam, LPARAM lparam) {
  if (emu.state == 0) return 0;
  glDisable( GL_TEXTURE_2D );
  
  glColor4ub(0x72,0x7E,0x01,0xFF);
  glColor4ub(0x80,0x8E,0x0B,0xFF);
  glBegin( GL_QUADS );
  glVertex2d(0.0,0.0);
  glVertex2d(client_width_,0.0);
  glVertex2d(client_width_,client_height_);
  glVertex2d(0.0,client_height_);
  glEnd();

  glEnable( GL_TEXTURE_2D );
  glColor3ub(0xFF,0xFF,0xFF);

  switch(display_mode) {
    case ID_VIDEO_STD320X288:
      //glDrawPixels(256,240,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
      //glEnable(GL_TEXTURE_2D);



      //memcpy(output,emu.lcd_driver()->frame_buffer,256*256*4);
      glBindTexture( GL_TEXTURE_2D, texture );
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,256,256,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,emu.lcd_driver()->frame_buffer);
      glBegin( GL_QUADS );
      glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
      glTexCoord2d(0.625,0.0); glVertex2d(client_width_,0.0);
      glTexCoord2d(0.625,0.5625); glVertex2d(client_width_,client_height_);
      glTexCoord2d(0.0,0.5625); glVertex2d(0.0,client_height_);
      glEnd();
    break;
      
    case ID_VIDEO_EAGLE512X480:
      ScaleImageEagle2X(emu.lcd_driver()->frame_buffer,256,256,output);
      glBindTexture( GL_TEXTURE_2D, texture );
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,512,512,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
      glBegin( GL_QUADS );
      glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
      glTexCoord2d(0.625,0.0); glVertex2d(client_width_,0.0);
      glTexCoord2d(0.625,0.5625); glVertex2d(client_width_,client_height_);
      glTexCoord2d(0.0,0.5625); glVertex2d(0.0,client_height_);
      glEnd();
      //hq2x_filter_render(output,256,nes.frame_buffer,256,256,240);
      //glDrawPixels(512,480,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
    break;

    case 3:
      dotmatrix_sim(emu.lcd_driver()->frame_buffer,dotmatrix_output);
      glBindTexture( GL_TEXTURE_2D, texture );
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,256*6,256*6,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,dotmatrix_output);
      glBegin( GL_QUADS );
      glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
      glTexCoord2d(0.625,0.0); glVertex2d(client_width_,0.0);
      glTexCoord2d(0.625,0.5625); glVertex2d(client_width_,client_height_);
      glTexCoord2d(0.0,0.5625); glVertex2d(0.0,client_height_);
      glEnd();
    break;
  }
  

  char caption[256];
  glColor4ub(0,0,0,150);
  //glRecti(0,0,225,62);
  sprintf(caption,"FPS\t: %02.2f Hz\0",emu.fps());
  gfx.PrintText(0,14,caption,strlen(caption));
  sprintf(caption,"Freq\t: %0.2f Mhz\0",emu.frequency_mhz());
  gfx.PrintText(0,28,caption,strlen(caption));
  sprintf(caption,"CPS\t: %llu\0",emu.cycles_per_second());
  gfx.PrintText(0,42,caption,strlen(caption));

  //char* mbc3_rtc_test();
  //auto str = mbc3_rtc_test();
  //gfx.PrintText(0,43,str,strlen(str));

  gfx.Render();
  return 0;
}

}

