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
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace ImGui {
static auto vector_getter = [](void* vec, int idx, const char** out_text) {
  auto& vector = *static_cast<std::vector<std::string>*>(vec);
  if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
  *out_text = vector.at(idx).c_str();
  return true;
};

bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
  if (values.empty()) { return false; }
  return Combo(label, currIndex, vector_getter,
    static_cast<void*>(&values), (int)values.size());
}

bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values) {
  if (values.empty()) { return false; }
  return ListBox(label, currIndex, vector_getter,
    static_cast<void*>(&values), (int)values.size());
}

}

namespace app {


  static ImGuiTextBuffer log;
  std::vector<std::string> disassembly;
  int diassembler_index = 0;
  void OutputDebugString2(const char* str) {
    log.append(str);
  }

DisplayWindow2::DisplayWindow2() : Window() {

//  counter = 0;
  instance = GetModuleHandle(nullptr);
}

DisplayWindow2::~DisplayWindow2() {

}


void DisplayWindow2::Init() {
  //output = new uint32_t[512*512];
  //dotmatrix_output = new uint32_t[256*256*6*6];
//  exit_signal_ = false;
  /*PrepareClass("GBEmu");
  window_class.style = CS_CLASSDC;
  window_class.hIcon = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,32,32,LR_DEFAULTSIZE));
  window_class.hIconSm = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,LR_DEFAULTSIZE));
  set_style(style()|WS_SIZEBOX);
  Create();*/
  
  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "ImGui Example", NULL };
  ::RegisterClassEx(&wc);
  handle_ = ::CreateWindow(wc.lpszClassName, "Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, this);

  
  DragAcceptFiles(handle(),true);
  enable_stats_display = false;
 

  graphics_.Initialize(handle(),0,0);


  emu.set_on_render([this]() {
    //InvalidateRect(handle(),nullptr,true);
    //PostMessage(windowhandle,WM_PAINT,0,0);
    //Render();

  });
  emu.set_log_output([](const char* str) {
    log.append(str);
  });

  machine_freq = emulation::gb::default_gb_hz;
  emu.Initialize(machine_freq);

  //settings setup
  ResetTiming();
  
  //OnCommand(ID_VIDEO_EAGLE512X480,0);
  Center();
  ShowWindow(handle(), SW_SHOW); 
  UpdateWindow(handle()); 
  //OnCommand(ID_MODE_NTSC,0);
  //OnCommand(ID_MODE_GBC,0);
  emu.set_mode(emulation::gb::EmuMode::EmuModeGBC);
  


  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();
  // Setup Platform/Renderer bindings
  ImGui_ImplWin32_Init(handle());
  ImGui_ImplDX11_Init(graphics_.device(), graphics_.context());


  
  CreateTextures();

  auto f = emu.lcd_driver()->frame_buffer;
  for (int i = 0; i < 256 * 256; ++i) {
    *f = 0xffff00ff;
    ++f;
  }


  
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
  //emu.cartridge()->LoadFile("..\\..\\test\\mooneye\\roms\\emulator-only\\mbc5\\rom_32Mb.gb",&header);// ok
  //emu.cartridge()->LoadFile("..\\..\\test\\other\\mbc3.gb", &header);// ok

  //emu.cartridge()->LoadFile("..\\..\\test\\SPRITE.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\opus5.gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\cgb_sound\\cgb_sound\\rom_singles\\03-trigger.gb",&header);


  //emu.cartridge()->LoadFile("..\\..\\test\\games\\Super Mario Land (World).gb",&header);
  //emu.cartridge()->LoadFile("..\\..\\test\\Pocket Camera (Japan) (Rev A).gb",&header);
  //emu.cartridge()->LoadFile("..\\..\\test\\Pokemon - Blue Version (UE) [S][!].gb",&header);
  //emu.cartridge()->LoadFile("..\\test\\Legend of Zelda, The - Link's Awakening (U) (V1.2) [!].gb",&header);//not original rom, problem with window
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
  //emu.cartridge()->LoadFile("..\\..\\test\\games\\Mission Impossible (USA) (En,Fr,Es).gbc",&header);
  //emu.cartridge()->LoadFile("..\\..\\test\\games\\Legend of Zelda, The - Link's Awakening DX (USA, Europe).gbc",&header);
   //emu.cartridge()->LoadFile("..\\..\\test\\games\\Alone in the Dark - The New Nightmare (U) (M3) [C][!].gbc", &header);
 emu.cartridge()->LoadFile("..\\..\\test\\games\\Pokemon - Silver Version (UE) [C][!].gbc",&header);
  //emu.cartridge()->LoadFile("..\\test\\games\\Grand Theft Auto.gbc",&header);
  //rom writing error mbc5 check
  //emu.cartridge()->LoadFile("C:\\Users\\U11111\\Documents\\GitHub\\GBEmu\\test\\mooneye-gb-master\\mooneye-gb_hwtests\\acceptance\\oam_dma\\sources-GS.gb",&header);

 //disassembly.push_back("hello");
 emu.disassembler()->set_pc(0x0000);
 for (int i = 0; i < 100; ++i) {
  // emu.disassembler()->FormatInstruction();
   //disassembly.push_back(emu.disassembler()->current_str());
 }

//  emu.Run();
 /*
 while (emu.cpu()->regs().PC != 0x100) {
   emu.cpu()->Step();
 }



 auto r = emu.memory()->Read8(0xFF50);
 r = emu.memory()->Read8(0xFF05);
 r = emu.memory()->Read8(0xFF06);
 r = emu.memory()->Read8(0xFF07);
 r = emu.memory()->Read8(0xFF10);
 r = emu.memory()->Read8(0xFF11);
 r = emu.memory()->Read8(0xFF12);
 r = emu.memory()->Read8(0xFF14);
 r = emu.memory()->Read8(0xFF16);
 r = emu.memory()->Read8(0xFF17);
 r = emu.memory()->Read8(0xFF19);
 r = emu.memory()->Read8(0xFF1A);
 r = emu.memory()->Read8(0xFF1B);
 r = emu.memory()->Read8(0xFF1C);
 r = emu.memory()->Read8(0xFF1E);
 r = emu.memory()->Read8(0xFF20);
 r = emu.memory()->Read8(0xFF21);
 r = emu.memory()->Read8(0xFF22);
 r = emu.memory()->Read8(0xFF23);
 r = emu.memory()->Read8(0xFF24);
 r = emu.memory()->Read8(0xFF25);
 r = emu.memory()->Read8(0xFF26);
 r = emu.memory()->Read8(0xFF40);
 r = emu.memory()->Read8(0xFF42);
 r = emu.memory()->Read8(0xFF43);
 r = emu.memory()->Read8(0xFF45);
 r = emu.memory()->Read8(0xFF47);
 r = emu.memory()->Read8(0xFF48);
 r = emu.memory()->Read8(0xFF49);
 r = emu.memory()->Read8(0xFF4A);
 r = emu.memory()->Read8(0xFF4B);
 r = emu.memory()->Read8(0xFFFF);
 */
 emu.Run();
}


void DisplayWindow2::CreateTextures() {

  graphics_.CreateTexture(256, 256, &pTexture, &out_srv);
  graphics_.CreateTexture(1024, 1024, &vramtex, &vramsrv);

  graphics_.CreateTexture(256, 256, &apu_window.oscilloscope_tex, &apu_window.oscilloscope_srv);
}

void DisplayWindow2::ResetTiming() {
  
}


void DisplayWindow2::GuiGameWindow() {

  auto r = ImGui::Begin("Game");

  //can be in on render but have to think about multithread
  D3D11_MAPPED_SUBRESOURCE sr;
  graphics_.context()->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
  memcpy(sr.pData, emu.lcd_driver()->frame_buffer, 256 * 256 * 4);
  graphics_.context()->Unmap(pTexture, 0);
  //ImGuiWindowFlags flags;
  ImGui::Image((void*)out_srv, ImVec2(160 * 2, 144 * 2), ImVec2(0, 0), ImVec2(160 / 256.0f, 144 / 256.0f));
  ImGui::End();
}

void DisplayWindow2::GuiVRAMWindow() {
  auto vram = emu.memory()->vram();

  auto vram0 = &vram[0];
  auto vram1 = &vram[(1 << 13)];

  D3D11_MAPPED_SUBRESOURCE sr;
  graphics_.context()->Map(vramtex, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
  uint32_t* dest_buffer = (uint32_t*)sr.pData;




  const uint32_t dmg_colors[4] = { 0xFF000000,0xFF545454,0xFFAAAAAA,0xFFFFFFFF };

#define pixel2(bit,y) (((tile[y<<1]>>((7-bit)&0x7))&0x1)+((((tile[(y<<1)+1]>>((7-bit)&0x7))<<1)&0x2)))
  for (int i = 0; i < 2; ++i) {
    for (int tile_index = 0; tile_index < 192 * 2; ++tile_index) {
      uint8_t* tile = &vram[(tile_index << 4) + (i << 13)];
      int offx = (i * 128) + (tile_index % 16) * 8;
      int offy = (tile_index / 16) * 8;

      for (int dy = 0; dy < 8; ++dy) {
        for (int dx = 0; dx < 8; ++dx) {
          dest_buffer[(dx + offx) + (dy + offy) * 1024] = dmg_colors[pixel2(dx, dy)];
        }
      }
    }
  }
#undef pixel2


  //memcpy(sr.pData, emu.lcd_driver()->frame_buffer, 256 * 256 * 4);

  graphics_.context()->Unmap(vramtex, 0);

  ImGui::Begin("VRAM");
  ImGui::Image((void*)vramsrv, ImVec2(256 * 2, 192 * 2), ImVec2(0, 0), ImVec2(256 / 1024.0f, 192 / 1024.0f));
  ImGui::End();
}

void DisplayWindow2::GuiOptionsWindow() {
  auto r = ImGui::Begin("Options");
  if (r) {
    const double m_min = 0.1;
    const double m_max = emulation::gb::gb_gbc_freq_hz;

    r = ImGui::SliderScalar("Machine Frequency", ImGuiDataType_Double, &machine_freq, &m_min, &m_max, "%.2f");
    if (r) {
      emu.set_base_freq_hz(machine_freq);
    }

    ImGui::Text("FPS\t: %02.2f Hz\0", emu.fps());
    ImGui::Text("Freq\t: %0.2f Mhz\0", emu.frequency_mhz());
    ImGui::Text("CPS\t: %llu\0", emu.cycles_per_second());
    ImGui::Text("Cartridge Type: 0x%02x", header.cartridge_type);
    ImGui::Text("Emulation Mode: 0x%02x", emu.mode());
    ImGui::Text("CGB Flag: 0x%02x", header.cgb_flag());

    if (ImGui::Button("Reset")) {
      emu.Reset();
      emu.Run();
    }

    if (ImGui::Button("Stop")) {
      emu.Stop();
    }
    if (ImGui::Button("Run")) {
      emu.Run();
    }

    ImGui::End();
  }
}


void DisplayWindow2::GuiAPUWindow() {

  //ImGui::SetNextWindowPos()
  if (ImGui::Begin("APU")) {




    D3D11_MAPPED_SUBRESOURCE sr;
    graphics_.context()->Map(apu_window.oscilloscope_tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
    if (sr.pData != null) {
      uint32_t* dest_buffer = (uint32_t*)sr.pData;

      for (int y = 0; y < 256; ++y) {
        uint32_t* dest_line = &dest_buffer[y * 256];
        for (int x = 0; x < 256; ++x) {
          *dest_line++ = 0xFF000000;
        }
      }

      auto sound_buf = emu.apu()->output_buffer();

      int32_t wave_left[256] = {0};
      memset(wave_left, 0, 256 * 4);
      uint32_t wave_left_pos = 0;
      int32_t wave_left_sample = 0;
      int32_t wave_right[256] = { 0 };
      memset(wave_right, 0, 256 * 4);
      uint32_t wave_right_pos = 0;
      int32_t wave_right_sample = 0;
      int acc = 0;

      for (int i = 0; i < 4410; ++i) {
        auto l = sound_buf[(i * 2)];
        auto r = sound_buf[(i * 2)+1];
        wave_left_sample += l;
        wave_right_sample += r;
        if (acc++ == 17) {
          wave_left[wave_left_pos++] = wave_left_sample / 17;
          wave_right[wave_right_pos++] = wave_right_sample / 17;
          acc = 0;
        }
      }

      for (int i = 0; i< 256; ++i) {
        auto y = wave_left[i] + 128;
        y = max(y, 0);
        y = min(y, 127);
        uint32_t* dest_line = &dest_buffer[y * 256];
        dest_line[i] = 0xFFFFFFFF;

      }

    }
    graphics_.context()->Unmap(apu_window.oscilloscope_tex, 0);



    ImGui::Image((void*)apu_window.oscilloscope_srv, ImVec2(256,256));
    ImGui::End();
  }

}




void DisplayWindow2::GuiDebuggerWindow() {
  if (ImGui::Begin("Console Output")) {
    //const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
   // static int item_current = 1;
    //ImGui::ListBox("listbox\n(single select)", &item_current, items, IM_ARRAYSIZE(items), 4);
    ImGui::BeginChild("Log");
    ImGui::TextUnformatted(log.begin(), log.end());
    ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
  }
  ImGui::End();

  if (ImGui::Begin("Disassembler")) {
    //const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
   // static int item_current = 1;
    //ImGui::ListBox("listbox\n(single select)", &item_current, items, IM_ARRAYSIZE(items), 4);
    ImGui::BeginChild("ROM0");
    ImGui::ListBox("listbox", &diassembler_index, disassembly);
    ImGui::EndChild();
    
  }
  ImGui::End();


  if (ImGui::Begin("Debugger")) {

    ImGui::Text("PC\t: 0x%04x\0", emu.cpu()->opcode_pc);


    if (ImGui::Button("Reset")) {
      emu.Reset();
      emu.Run();
    }

    if (ImGui::Button("Stop")) {
      emu.Stop();
    }
    if (ImGui::Button("Run")) {
      emu.Run();
    }


    if (ImGui::Button("Step")) {
      emu.cpu()->Step();
    }

    ImGui::End();
  }
}

void DisplayWindow2::Step() {
  //if (emu.state == emulation::gb::EmuState::EmuStateStopped) return;
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  



  //ImGui::ShowDemoWindow();
  
    GuiGameWindow();
    //GuiVRAMWindow();
    GuiOptionsWindow();
    //GuiAPUWindow();
    GuiDebuggerWindow();

  /*
  {
    bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);




    bool show_another_window = false;
    static float f = 0.0f;
    static int counter = 0;

    auto r = ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }
  */

  ImGui::Render();
  graphics_.Clear(graphics::Color(120, 160, 170));
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  graphics_.Render();
}

int DisplayWindow2::OnCreate(WPARAM wParam,LPARAM lParam) {
  return 0;
}

int DisplayWindow2::OnDestroy(WPARAM wParam,LPARAM lParam) {
  //exit_signal_ = true;
  //
  emu.Stop();
  emu.Deinitialize();



  SafeRelease(&apu_window.oscilloscope_srv);
  SafeRelease(&apu_window.oscilloscope_tex);
  SafeRelease(&vramsrv);
  SafeRelease(&vramtex);
  SafeRelease(&out_srv);
  SafeRelease(&pTexture);

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  graphics_.Deinitialize();

  //delete []output;
  //SafeDeleteArray(&dotmatrix_output);
  PostQuitMessage(0);
  return 0;
}

int DisplayWindow2::OnCommand(WPARAM wParam,LPARAM lParam) {

  /*switch (LOWORD(wParam)) {
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
    
      display_mode = ID_VIDEO_STD320X288;
    }

    if (LOWORD(wParam)==ID_VIDEO_EAGLE512X480) {
      CheckMenuItem(menu_,ID_VIDEO_EAGLE512X480,MF_CHECKED);
      CheckMenuItem(menu_,ID_VIDEO_STD320X288,MF_UNCHECKED);
      SetClientSize(640,288*2);
     
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
     
    }
  
  }*/

  return 0;
}

int DisplayWindow2::OnKeyDown(WPARAM wParam,LPARAM lParam) {


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
    emu.cpu()->Step();

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

int DisplayWindow2::OnKeyUp(WPARAM wParam,LPARAM lParam) {


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

int DisplayWindow2::OnDropFiles(WPARAM wParam,LPARAM lParam) {
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

int DisplayWindow2::Render() {
  return 0;
}

int DisplayWindow2::OnResize(WPARAM wparam, LPARAM lparam) {
 
  client_width_ = LOWORD(lparam);
  client_height_ = HIWORD(lparam);
  graphics_.SetDisplaySize(client_width_, client_height_);


  return 0;
}

int DisplayWindow2::OnPaint(WPARAM wparam, LPARAM lparam) {
  if (emu.state == emulation::gb::EmuState::EmuStateStopped) return 0;


  return 0;
}



int DisplayWindow2::PreProcessMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))     {
    return 1;
  }

  if (uMsg == WM_SYSCOMMAND) {
    if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
  }

  return -1;
}



}

