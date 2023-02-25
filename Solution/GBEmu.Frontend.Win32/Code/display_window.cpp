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
#include "gbemu.h"
#include "graphics/eagle.h"
#include <iostream>
#include <fstream>
#include <string>

void hq2x_filter_render(
  uint32_t *output, unsigned outputPitch,
  const uint32_t *input, unsigned inputPitch,
  unsigned width, unsigned height
);

//GLuint colours_vbo = 0;
struct myVertex2D {
  float x, y, u, v;
};


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



std::string ReadTextFile(const char* filename)
{

  std::ifstream infile(filename);
  
  std::string contents((std::istreambuf_iterator<char>(infile)),std::istreambuf_iterator<char>());

  return contents;
  /*std::ifstream hFile(filename);

  auto lines = new std::vector<GLchar>();
  std::string line;
  do {
    lines->push_back(hFile.get());
  } while(hFile.eof()==false);
  lines->push_back('\0');

  return lines;*/
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
  {

    texture = 0;
    vertex_shader = 0;
    fragment_shader = 0;
    prog = 0;
    points_vbo = 0;
    vao = 0;
    baseline_alpha_loc = 0;
    texture1_loc = 0;
  }



//  exit_signal_ = false;
  PrepareClass("GBEmu");
  window_class.hIcon = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,32,32,LR_DEFAULTSIZE));
  window_class.hIconSm = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,LR_DEFAULTSIZE));
  set_style(style()|WS_SIZEBOX);
  Create();
  SetMenu(LoadMenu(instance, MAKEINTRESOURCE(IDR_MENU1)));
  DragAcceptFiles(handle(),true);
  enable_stats_display = false;



  gfx.Initialize(handle(),0,0);
  HWND windowhandle = handle();
  emu.set_on_render([windowhandle]() {
    //InvalidateRect(handle(),nullptr,true);
    PostMessage(windowhandle,WM_PAINT,0,0);
    //Render();
  });

  emu.Initialize(emulation::gb::default_gb_hz);

  //auto a = glGetError();
  //glPixelZoom(1, -1);
  //a = glGetError();
  //glRasterPos2i(0,0);
  //a = glGetError();
  texture = gfx.CreateTexture();
  //wglMakeCurrent(NULL, NULL);

/*
{
  auto vsFile = ReadTextFile("vs.glsl");
  auto fsFile = ReadTextFile("fs.glsl");
  auto arr1 = (GLchar  **)(&(*vsFile));
  auto arr2 = (GLchar  **)(&(*fsFile));
//,&(*fsFile)[0];

  graphics::Shader shader(arr1,arr2);
  delete fsFile;
  delete vsFile;
}*/
  SetupShaders();
  SetupVertexBuffer();


  //settings setup
  ResetTiming();
  
  OnCommand(ID_VIDEO_EAGLE512X480,0);
  Center();
  ShowWindow(handle(), SW_SHOW); 
  UpdateWindow(handle()); 
  OnCommand(ID_MODE_NTSC,0);

  //emu_th = new std::thread(&app::DisplayWindow::Step,this);
  //emu_th->join();
  
  //emu.lcd_driver()->lcdscreenmode_
  OnCommand(ID_MODE_GBC,0);
  
  //emu.cartridge()->LoadFile("..\\..\\test\\cpu_instrs\\cpu_instrs.gb",&header); //ok
  //emu.cartridge()->LoadFile("..\\..\\test\\blargg\\instr_timing\\instr_timing\\instr_timing.gb",&header); //ok
  //emu.cartridge()->LoadFile("..\\..\\test\\blargg\\interrupt_time\\interrupt_time\\interrupt_time.gb", &header);//ok 
  //emu.cartridge()->LoadFile("..\\..\\test\\blargg\\mem_timing-2\\mem_timing-2\\mem_timing.gb",&header); //ok

  //emu.cartridge()->LoadFile("..\\..\\test\\mooneye\\roms\\acceptance\\halt_ime0_ei.gb", &header);//ok
  
  //timer
  //emu.cartridge()->LoadFile("..\\..\\test\\mooneye\\roms\\acceptance\\timer\\rapid_toggle.gb", &header);//not ok


  //halt
  
   //emu.cartridge()->LoadFile("..\\..\\test\\blargg\\halt_bug\\halt_bug.gb",&header);//ok


  //emu.cartridge()->LoadFile("..\\..\\test\\blargg\\oam_bug\\oam_bug\\rom_singles\\4-scanline_timing.gb", &header); //not ok
  //emu.cartridge()->LoadFile("..\\..\\test\\blargg\\oam_bug\\oam_bug\\oam_bug.gb",&header);//not ok
  

  //mbc tests
  //emu.cartridge()->LoadFile("..\\..\\test\\other\\mbctest\\mbctest.gb", &header); //ok
  //emu.cartridge()->LoadFile("..\\..\\test\\mooneye\\roms\\emulator-only\\mbc5\\rom_32Mb.gb",&header);//not ok

  //emu.cartridge()->LoadFile("..\\..\\test\\SPRITE.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\opus5.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\cgb_sound\\cgb_sound\\rom_singles\\03-trigger.gb",&header);


  auto load_result = emu.cartridge()->LoadFile("..\\..\\test\\games\\Super Mario Land (World).gb",&header);
  //emu.cartridge()->LoadFile("..\\..\\test\\Pocket Camera (Japan) (Rev A).gb",&header);
  //emu.cartridge()->LoadFile("..\\..\\test\\Pokemon - Blue Version (UE) [S][!].gb",&header);
  //auto load_result = emu.cartridge()->LoadFile("..\\..\\test\\games\\Legend of Zelda, The - Link's Awakening (U) (V1.2) [!].gb",&header);//not original rom, problem with window

  //emu.cartridge()->LoadFile("..\\test\\Final Fantasy Legend, The (U) [!].gb",&header); 
  //emu.cartridge()->LoadFile("D:\\Personal\\Dev\\GB\\roms\\Kirby's Dream Land (USA, Europe).gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\Tamagotchi 3.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\Tamagotchi (USA, Europe).gb",&header);
  
  //hdma
  //emu.cartridge()->LoadFile("..\\..\\test\\gbc-hw-tests\\dma\\hdma_halt\\hdma_halt.gbc", &header);
  //emu.cartridge()->LoadFile("..\\..\\test\\demos\\pht-mr.gbc", &header);
  //emu.cartridge()->LoadFile("..\\..\\test\\demos\\Demotronic Final Demo (PD) [C].gbc",&header);//works as of 18/06/2020
  //emu.cartridge()->LoadFile("..\\..\\test\\demos\\Game Boy Color Promotional Demo (USA, Europe).gbc",&header);
  //emu.cartridge()->LoadFile("..\\..\\test\\introcollection.gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\pht-mr.gbc",&header);
 // emu.cartridge()->LoadFile("..\\..\\test\\games\\Mission Impossible (USA) (En,Fr,Es).gbc",&header);
 // emu.cartridge()->LoadFile("..\\..\\test\\games\\Legend of Zelda, The - Link's Awakening DX (USA, Europe).gbc",&header);
   //emu.cartridge()->LoadFile("..\\..\\test\\games\\Alone in the Dark - The New Nightmare (U) (M3) [C][!].gbc", &header);
 //emu.cartridge()->LoadFile("..\\..\\test\\games\\Pokemon - Silver Version (UE) [C][!].gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\games\\Grand Theft Auto.gbc",&header);
  //rom writing error mbc5 check
  //emu.cartridge()->LoadFile("C:\\Users\\U11111\\Documents\\GitHub\\GBEmu\\test\\mooneye-gb-master\\mooneye-gb_hwtests\\acceptance\\oam_dma\\sources-GS.gb",&header);
  

  if (load_result != S_OK) {
    MessageBox(handle(), "File Not Found.", "Error", MB_OK | MB_ICONWARNING);
  }
  else {
    //OnCommand(ID_MODE_GBC, 0);
    emu.Run();
  }
}



int DisplayWindow::SetupShaders() {

  const char* vs, * fs;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  auto vsFile = ReadTextFile("vs.glsl");
  auto fsFile = ReadTextFile("fs.glsl");

  vs = vsFile.c_str();
  fs = fsFile.c_str();

  const char* vv = vs;
  const char* ff = fs;

  glShaderSource(vertex_shader, 1, &vv, NULL);
  auto e = glGetError();
  glShaderSource(fragment_shader, 1, &ff, NULL);
  e = glGetError();
  //free(vs);free(fs);

  glCompileShader(vertex_shader);
  e = glGetError();
  glCompileShader(fragment_shader);
  e = glGetError();

  prog = glCreateProgram();
  e = glGetError();
  glAttachShader(prog, vertex_shader);
  e = glGetError();
  glAttachShader(prog, fragment_shader);
  e = glGetError();
  //glBindAttribLocation (prog, 0, "vertex_position");
  //glBindAttribLocation (prog, 1, "vertex_colour");
  glLinkProgram(prog);
  e = glGetError();


  glLinkProgram(prog);
  e = glGetError();
  baseline_alpha_loc = glGetUniformLocation(prog, "baseline_alpha");
  e = glGetError();
  texture1_loc = glGetUniformLocation(prog, "texture1");
  e = glGetError();

  return S_OK;
}

int DisplayWindow::SetVertexBufferSize(float w, float h) {
  if (points_vbo == 0)
    return S_FALSE;
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);

  myVertex2D points[] = {
    {0.0f, 0.0f,  0.0f, 0.0f},
    {w,    0.0f,  0.625f, 0.0f},
    {w,    h,     0.625f, 0.5625f},
    {0.0f, h,     0.0f, 0.5625f},
  };
  //glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
  //glTexCoord2d(0.625,0.0); glVertex2d(client_width_,0.0);
  ///glTexCoord2d(0.625,0.5625); glVertex2d(client_width_,client_height_);
  //glTexCoord2d(0.0,0.5625); glVertex2d(0.0,client_height_);

  glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex2D) * 4, points, GL_STATIC_DRAW);

  return S_OK;
}


int DisplayWindow::SetupVertexBuffer() {
  glGenBuffers(1, &points_vbo);
  SetVertexBufferSize(20, 20);

  float colours[] = {
    1.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 0.0f,  1.0f,
    1.0f, 1.0f,  1.0f,
  };

  //glGenBuffers (1, &colours_vbo);
  //glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  //glBufferData (GL_ARRAY_BUFFER, 3 * sizeof (float) * 4, colours, GL_STATIC_DRAW);


  glGenVertexArrays(1, &vao);
  auto e = glGetError();
  glBindVertexArray(vao);
  e = glGetError();
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  e = glGetError();
  glVertexPointer(2, GL_FLOAT, sizeof(myVertex2D), (GLvoid*)offsetof(myVertex2D, x));
  glTexCoordPointer(2, GL_FLOAT, sizeof(myVertex2D), (GLvoid*)offsetof(myVertex2D, u));


  //Enable vertex and texture coordinate arrays
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  //glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  //glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  //glEnableVertexAttribArray (0);
  //glEnableVertexAttribArray (1);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  return S_OK;
}



void DisplayWindow::ResetTiming() {
  
}

void DisplayWindow::Step() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
      emu.set_mode(emulation::gb::EmuMode::EmuModeGB); 
      break;
    case ID_MODE_GBC:
      CheckMenuItem(menu_,ID_MODE_GBC,MF_CHECKED);
      CheckMenuItem(menu_,ID_MODE_GB,MF_UNCHECKED);
      emu.set_mode(emulation::gb::EmuMode::EmuModeGBC);
      break;
  }

  if (HIWORD(wParam)==0) {
    
    if (LOWORD(wParam)==ID_VIDEO_STD320X288) {
      CheckMenuItem(menu_,ID_VIDEO_STD320X288,MF_CHECKED);
      CheckMenuItem(menu_,ID_VIDEO_EAGLE512X480,MF_UNCHECKED);
      SetClientSize(320,288);
      gfx.SetDisplaySize(320,288);
      SetVertexBufferSize(320,288);
      display_mode = ID_VIDEO_STD320X288;
    }

    if (LOWORD(wParam)==ID_VIDEO_EAGLE512X480) {
      CheckMenuItem(menu_,ID_VIDEO_EAGLE512X480,MF_CHECKED);
      CheckMenuItem(menu_,ID_VIDEO_STD320X288,MF_UNCHECKED);
      SetClientSize(640,288*2);
      gfx.SetDisplaySize(640,288*2);
      SetVertexBufferSize(640, 288 * 2);
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
      DestroyWindow(handle());
     // PostQuitMessage(0);
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


  if (wParam == VK_F3) {
    enable_stats_display = !enable_stats_display;
  }

  if (wParam == '1')
    emu.lcd_driver()->ToggleBG();
  if (wParam == '2')
    emu.lcd_driver()->ToggleWindow();
  if (wParam == '3')
    emu.lcd_driver()->ToggleSprite();

  if (wParam == 'A')
    emu.Stop();
  if (wParam == 'B')
    emu.Run();

  if (wParam == VK_F5)
    emu.Step();

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

  if (strcmp(ext,".gbc")==0) {
    emu.Stop();
    //OnCommand(ID_MODE_GBC, 0);
    emulation::gb::CartridgeHeader header;
    emu.cartridge()->LoadFile(filename, &header);
    OnCommand(ID_MACHINE_RESET,0);
  }

  if (strcmp(ext, ".gb") == 0) {
    emu.Stop();
    //OnCommand(ID_MODE_GB, 0);
    emulation::gb::CartridgeHeader header;
    emu.cartridge()->LoadFile(filename, &header);
    OnCommand(ID_MACHINE_RESET, 0);
  }

  SafeDeleteArray(&filename);

  return 0;
}

int DisplayWindow::RenderNormal1() {

  //char title[200];
  //sprintf_s(title, "GBEmu - Freq\t: %0.2f Mhz\0", emu.frequency_mhz());
  //SetWindowText(handle(), title);
  /*
  PAINTSTRUCT ps;
  BeginPaint(handle(), &ps);
  FillRect(ps.hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
  EndPaint(handle(), &ps);

  return S_OK;*/
  //glDisable( GL_TEXTURE_2D );

  //glColor4ub(0x72,0x7E,0x01,0xFF);
  /*
  glColor4ub(0, 255,0, 0x8E);
  glBegin( GL_QUADS );
  glVertex2d(0.0,0.0);
  glVertex2d(client_width_,0.0);
  glVertex2d(client_width_,client_height_);
  glVertex2d(0.0,client_height_);
  glEnd();*/
  //ScaleImageEagle2X(emu.lcd_driver()->frame_buffer, 256, 256, output);
  dotmatrix_sim(emu.lcd_driver()->frame_buffer, dotmatrix_output);
  glClearColor(0.25f, 0.25f, 0.25f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4ub(255, 255, 255, 255);
  glEnable(GL_TEXTURE_2D);
  auto e = glGetError();
  glBindTexture(GL_TEXTURE_2D, texture);
  e = glGetError();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256*6, 256*6, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, dotmatrix_output);//emu.lcd_driver()->frame_buffer);
  e = glGetError();
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
  glTexCoord2d(0.625, 0.0); glVertex2d(client_width_, 0.0);
  glTexCoord2d(0.625, 0.5625); glVertex2d(client_width_, client_height_);
  glTexCoord2d(0.0, 0.5625); glVertex2d(0.0, client_height_);
  glEnd();


  if (enable_stats_display) {

    char caption[1024];
    int y = 1;
    sprintf_s(caption, "FPS\t: %02.2f Hz\0", emu.fps());
    gfx.PrintText(1, y += 14, caption, strlen(caption));
    sprintf_s(caption, "Freq\t: %0.2f Mhz\0", emu.frequency_mhz());
    gfx.PrintText(1, y += 14, caption, strlen(caption));
    sprintf_s(caption, "CPS\t: %llu\0", emu.cycles_per_second());
    gfx.PrintText(1, y += 14, caption, strlen(caption));
    sprintf_s(caption, "Cartridge Type: 0x%02x", header.cartridge_type);
    gfx.PrintText(1, y += 14, caption, strlen(caption));
    sprintf_s(caption, "Emulation Mode: 0x%02x", emu.mode());
    gfx.PrintText(1, y += 14, caption, strlen(caption));
    sprintf_s(caption, "CGB Flag: 0x%02x", header.cgb_flag());
    gfx.PrintText(1, y += 14, caption, strlen(caption));

    if (header.cartridge_type >= 0x0F && header.cartridge_type <= 0x13) {
      char* mbc3_rtc_test();
      auto str = mbc3_rtc_test();
      gfx.PrintText(0, y += 14, str, strlen(str));
    }
  }


  //gfx.Render();
  glFlush();
  glFinish();
  //gfx.Render();
  //Sleep(15);
  glDrawBuffer(GL_FRONT);
  return S_OK;

}


int DisplayWindow::RenderWithShaders1() {
  auto e = glGetError();
  /*glLinkProgram(prog);
  auto e = glGetError();
  auto baseline_alpha_loc = glGetUniformLocation(prog, "baseline_alpha");
  e = glGetError();
  auto texture1_loc = glGetUniformLocation(prog, "texture1");
  e = glGetError();*/


  glUseProgram(prog);
  e = glGetError();
  glUniform1f(baseline_alpha_loc, 1.0f);
  e = glGetError();

  glUniform1i(texture1_loc, 0);
  e = glGetError();

  glActiveTexture(GL_TEXTURE0);
   e = glGetError();
  glBindTexture(GL_TEXTURE_2D, texture);
  e = glGetError();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, emu.lcd_driver()->frame_buffer);
  e = glGetError();
  glBindTexture(GL_TEXTURE_2D, texture);
  //glActiveTexture(0);
  e = glGetError();


  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  //glUseProgram(prog);
  //e = glGetError();
  glBindVertexArray(vao);
  e = glGetError();
  glDrawArrays(GL_QUADS, 0, 4);
  e = glGetError();
  //glUseProgram(0);
  //e = glGetError();





  /*const GLfloat texture_coordinates[] = {0, 1,
                                         0, 0,
                                         1, 1,
                                         1, 0};
  //glTexCoordPointer(2, GL_FLOAT, 0, texture_coordinates);

  const GLfloat vertices[] = {0, (float)client_height_,
                              0, 0,
                              (float)client_width_, (float)client_height_,
                              float(client_width_), 0};
  glVertexPointer(2, GL_FLOAT, 0, vertices);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/
  //glDisableClientState(GL_VERTEX_ARRAY);



  glFlush();
  glFinish();
  //gfx.Render();
  //Sleep(15);
  glDrawBuffer(GL_FRONT);
  return S_OK;
}

int DisplayWindow::OnResize(WPARAM wparam, LPARAM lparam) {
  gfx.SetDisplaySize(LOWORD(lparam),HIWORD(lparam));
  client_width_ = LOWORD(lparam);
  client_height_ = HIWORD(lparam);
  SetVertexBufferSize((float)client_width_, (float)client_height_);
  return 0;
}

int DisplayWindow::OnPaint(WPARAM wparam, LPARAM lparam) {
  if (emu.state == emulation::gb::EmuState::EmuStateStopped) return 0;

  //return RenderNormal1();
  

  
   RenderWithShaders1();
  //older

   return S_OK;

  glEnable( GL_TEXTURE_2D );
  glColor3ub(0xFF,0xFF,0xFF);

  switch(display_mode) {
    case ID_VIDEO_STD320X288:

      //glDrawPixels(256,240,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
      //glEnable(GL_TEXTURE_2D);


	    //glUseProgram(prog);
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
  sprintf_s(caption,"FPS\t: %02.2f Hz\0",emu.fps());
  gfx.PrintText(0,14,caption,strlen(caption));
  sprintf_s(caption,"Freq\t: %0.2f Mhz\0",emu.frequency_mhz());
  gfx.PrintText(0,28,caption,strlen(caption));
  sprintf_s(caption,"CPS\t: %llu\0",emu.cycles_per_second());
  gfx.PrintText(0,42,caption,strlen(caption));



  gfx.Render();
  return 0;
}

}

