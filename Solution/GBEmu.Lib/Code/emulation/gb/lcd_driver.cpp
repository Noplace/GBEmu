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
#include "gb.h"

/*
note: I have made two cgb sprite palletes for sake of easier code for the mono game -> color conversion
*/

namespace emulation {
namespace gb {
  


#define pixel(bit) (((tile[y<<1]>>(bit&0x7))&0x1)+((((tile[(y<<1)+1]>>(bit&0x7))<<1)&0x2)))
#define pixel2(bit,y) (((tile[y<<1]>>(bit&0x7))&0x1)+((((tile[(y<<1)+1]>>(bit&0x7))<<1)&0x2)))


struct DMGSprite {
  uint8_t y, x, tileindex;

  union {
    struct {
      uint8_t unused : 4;
      uint8_t pal : 1;
      uint8_t xflip : 1;
      uint8_t yflip : 1;
      uint8_t priority : 1;
      /* Bit7   OBJ-to-BG Priority (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
        (Used for both BG and Window. BG color 0 is always behind OBJ)
Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
*/
    };
    uint8_t raw;
  }attr;

};

struct GBCSprite {
  uint8_t y, x, tileindex;

  union {
    struct {
      uint8_t cgbpalnum : 3;
      uint8_t vrambank : 1;
      uint8_t pal : 1;
      uint8_t xflip : 1;
      uint8_t yflip : 1;
      uint8_t priority : 1;
      /* Bit7   OBJ-to-BG Priority (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
        (Used for both BG and Window. BG color 0 is always behind OBJ)
Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
*/
    };
    uint8_t raw;
  }attr;

};

static inline uint32_t bgr555torgb(uint32_t src) {
  uint8_t r = (src&0x1F)<<3;
  uint8_t g = ((src>>5)&0x1F)<<3;
  uint8_t b = ((src>>10)&0x1F)<<3;
  return 0xff000000|(r<<16)|(g<<8)|(b);
}

static inline uint32_t rgbtobgr555(uint32_t src) {
  uint8_t r = ((src>>16)&0xFF)>>3;
  uint8_t g = ((src>>8)&0xFF)>>3;
  uint8_t b = ((src)&0xFF)>>3;
  return (b<<10)|(g<<5)|(r);
}


//const uint32_t pal32[4] = {0xffffffff,0xffAAAAAA,0xff545454,0xff000000};
//const uint32_t dmg_colors[4] = {0x00000000,0x54000000,0xAA000000,0xFF000000};
const uint32_t dmg_colors[4] = { 0x00000000,0x54000000,0xAA000000,0xFF000000 };



void LCDDriver::Initialize(Emu* emu) {
  Component::Initialize(emu);
  frame_buffer = new uint32_t[256*256];
  colormap = new ColorMapLine[256*256];
  memset(frame_buffer,0,256*256*4);
  memset(colormap,0,256*256*4);
  lcdscreenmode_ = LCDScreenModeProgressive;
  memset(&hdma,0,sizeof(hdma));
  hdma.ff55 = 0xFF;
  memset(&cgb_bgpal,0xFF,sizeof(cgb_bgpal));
  memset(&cgb_sppal1,0,sizeof(cgb_sppal1));
  memset(&cgb_sppal2,0,sizeof(cgb_sppal2));
  enable_bg_ = true;
  enable_window_ = true; 
  enable_sprite_ = true;
  //auto ioports = emu_->memory()->ioports();
  //lcdc_ = ((LCDControlRegister*)&ioports[0x40]);
  //stat_ = ((LCDStatusRegister*)&ioports[0x41]);

  /*
  //hack, to show something during boot in cgb mode
  for (int i=0;i<64;i+=8) {
    cgb_bgpal[i+0] = rgbtobgr555(pal32[0]);
    cgb_bgpal[i+1] = rgbtobgr555(pal32[0])>>8;
    cgb_bgpal[i+2] = rgbtobgr555(pal32[1]);
    cgb_bgpal[i+3] = rgbtobgr555(pal32[1])>>8;
    cgb_bgpal[i+4] = rgbtobgr555(pal32[2]);
    cgb_bgpal[i+5] = rgbtobgr555(pal32[2])>>8;
    cgb_bgpal[i+6] = rgbtobgr555(pal32[3]);
    cgb_bgpal[i+7] = rgbtobgr555(pal32[3])>>8;
  }*/
  Reset();
}


struct cgb_pal_config {
  uint32_t bg[4];
  uint32_t obj1[4];
  uint32_t obj2[4];
};
 
cgb_pal_config cgbp_default = {
  {0xFFFFFFFF	, 0xFF7BFF31	, 0xFF0063C5	, 0xFF000000},
  {0xFFFFFFFF	, 0xFFFF8484	, 0xFF943A3A	, 0xFF000000},
  {0xFFFFFFFF	, 0xFFFF8484	, 0xFF943A3A	, 0xFF000000},
};

cgb_pal_config cgbp_supermario_0x46 = {
  {0xB5B5FF	, 0xFFFF94	, 0xAD5A42	, 0x000000},
  {0x000000	, 0xFFFFFF	, 0xFF8484,	 0x943A3A},
  {0x000000	, 0xFFFFFF	, 0xFF8484	, 0x943A3A},
};
/*
BG	 
OBJ0	 
OBJ1	 
*/
void LCDDriver::UpdateCGBPalBasedonGBPal(int paltype) {
  if (emu_->mode() != EmuMode::EmuModeGBC) return; //we should use a seperate flag to force mono to color regardless of gb mode/type
  if (emu_->cartridge()->header->cgb_flag() != 0x0) return;
  return;
  uint8_t* src;
  uint8_t* dest;
  uint32_t* palcol;
  cgb_pal_config* pal_config;
  switch (emu_->cartridge()->hash) {
    case 0x46:pal_config= &cgbp_supermario_0x46; break;
    default:pal_config= &cgbp_default; break;
  }
  switch (paltype) {
    case 0:src = bg_pal; dest=cgb_bgpal; palcol =pal_config->bg;  break;
    case 1:src = obj_pal1;dest=cgb_sppal1;palcol =pal_config->obj1; break;
    case 2:src = obj_pal2;dest=cgb_sppal2;palcol =pal_config->obj2; break;
  }
  for (int i=0;i<64;i+=8) {
    dest[i+0] = rgbtobgr555(palcol[src[0]]);
    dest[i+1] = rgbtobgr555(palcol[src[0]])>>8;
    dest[i+2] = rgbtobgr555(palcol[src[1]]);
    dest[i+3] = rgbtobgr555(palcol[src[1]])>>8;
    dest[i+4] = rgbtobgr555(palcol[src[2]]);
    dest[i+5] = rgbtobgr555(palcol[src[2]])>>8;
    dest[i+6] = rgbtobgr555(palcol[src[3]]);
    dest[i+7] = rgbtobgr555(palcol[src[3]])>>8;
  }
}

void LCDDriver::Deinitialize() {
  delete [] colormap;
  delete [] frame_buffer;  
}

void LCDDriver::Reset() {
  memset(&spritedma,0,sizeof(spritedma));
  evenodd = 0;
  lcdc_.raw = 0;
  stat_.raw = 2;
  ly = 0;
  lyc = 0;
  scroll_x = scroll_y = 0;
  wx = wy = 0;
  screen_counter_ = scanline_dots_ = 0;
  dot_delay_ = 0;
  vsync = 0;
  hsync = 0;
  sprite_bug_counter = 0;
  mode3_extra_cycles_ = 0;
  cgb_bgpal_index= cgb_bgpal_rindex= cgb_bgpal_windex=cgb_bgpal_data=cgb_sppal_index=cgb_sppal_data=0;
  dma_reg_ = 0;
  int48signal = 0;
  int40signal = 0;
}
/*todo variable time for mode 3 and 0
mode 3:
between 4194304*0.00004137=173.51835648 and //172
4194304*0.00007069=296.49534976 //296
done,check
*/

void LCDDriver::DoHDMA() {
  if (hdma.mode == 1 && hdma.active == 1) {
    uint16_t offset = 0;
    

    auto src = hdma.src.raw & 0xFFF0;
    auto dest = 0x8000 | (hdma.dest.raw & 0x1FF0);
    // memcpy(dest,src,hdma.length);

    for (int i = 0; i < 0x10; ++i) {
      auto data = emu_->memory()->Read8(src + i);
      emu_->memory()->Write8(dest + i, data);
    }

 
    hdma.length -= 0x10;
    hdma.src.raw += 0x10;
    hdma.dest.raw += 0x10;
    --hdma.ff55;
    if (hdma.ff55 == 0xFF)
      hdma.active = 0;
  }

}



void LCDDriver::Tick() {
  ++scanline_dots_; //line clock
  ++screen_counter_;//screen clock

  stat_.coincidence = lyc == ly;


  //dma 
  if (spritedma.start == true) { //launched

  }


  static auto mode0_counter = 0;
  static auto mode1_counter = 0;
  static auto mode2_counter = 0;
  static auto mode3_counter = 0;

  //check this
  if (screen_counter_ > 1) {
    if (scanline_dots_ == 1) {
      if ((stat_.coincidence_inr && stat_.coincidence)) { //test
        if (int48signal == 0) {
          emu_->memory()->interrupt_flag() |= 2;
          int48signal = 1;
        }
      }
    }
  }



  switch (stat_.mode) {
  case 2://searching oam
    
    ++mode2_counter;
    
      if (stat_.oam_int && int48signal == 0) { //|| (stat_.coincidence_inr && stat_.coincidence) lyc interrupts occurs at start of mode 2
          emu_->memory()->interrupt_flag() |= 2;
          int48signal = 1;
      }



    if (scanline_dots_ == 80)
      stat_.mode = 3;
    break;
  case 3://trnsfer data to LCD
    ++mode3_counter;
    {
      static auto flag = lcdc_.bgdisplay;
      if (flag != lcdc_.bgdisplay) {
        //OutputDebugString("bg display changed during mode3\n");
        flag = lcdc_.bgdisplay;
      }
    }

    if (scanline_dots_ >= 80 && scanline_dots_ < 160) {
      if (sprite_bug_counter != 0) { //80 is 20 cycles if 114 is one scanline total cycles
        if (lcdc_.lcd_enable == 1)
          emu_->memory()->oam()[8 + (rand() % 152)] = rand() & 0xFF;
        --sprite_bug_counter;
      }
    }


    if (scanline_dots_ == 81+8) {// account for 8 extra cycles
      sprite_bug_counter = 0;
      pixel_counter_ = 0;
      //RenderLine(0, 160);//will update mode3_extra_cycles_
      BeginRenderLine();
     
    }

    
    if (scanline_dots_ >= 89 && pixel_counter_ < 160) { //0 to 159
      //RenderLine(0, 160);

      if (dot_delay_ == 0) {
        if (render_line == true) {
          
          if (emu_->mode() == EmuMode::EmuModeGBC) {
            RenderCGBBGPixel(cmline);
            RenderCGBWindowPixel(cmline);
            RenderCGBSpritePixel(cmline);
          } else {
            RenderDMGBGPixel(cmline);
            RenderDMGWindowPixel(cmline);
            RenderDMGSpritePixel(cmline);
          }
        }
        ++pixel_counter_;
      } else {
        --dot_delay_;
      }

    }

    if (pixel_counter_ == 160) {
      EndRenderLine();
      stat_.mode = 0;
      pixel_counter_ = 0;
    }


    /*
    if (scanline_dots_ >= 81 && scanline_dots_ <= 241) {
      //sprite_bug_counter = 0;
      //pixel_counter_ = 0;
      RenderLine(0, 160);//will update mode3_extra_cycles_
    }

    if (scanline_dots_ == 252 + mode3_extra_cycles_) {
      stat_.mode = 0;

    }*/

  break;
  case 0://hblank
    if (mode0_counter == 0)
      DoHDMA();
    ++mode0_counter;
    /*if (sprite_bug_counter != 0 && lcdc_.lcd_enable == 1) {
      emu_->memory()->oam()[8+(rand()%152)] = rand()&0xFF;
      --sprite_bug_counter;
    }*/
    //if (scanline_dots_ == 252 + mode3_extra_cycles_ + 1) {
    if (stat_.hblank_int && int48signal == 0) {
      emu_->memory()->interrupt_flag() |= 2;
      int48signal = 1;
    }
    //}

    if (scanline_dots_ == 456) {

      //DoHDMA();
      if (ly == 143) {
        stat_.mode = 1;//vblank period
      }
      else {
        stat_.mode = 2;

      }
    }
    break;
  case 1: { //vblank
    ++mode1_counter;
    if (ly == 144 && scanline_dots_ == 1) {
      emu_->memory()->interrupt_flag() |= 1;
    }
    if ((stat_.vblank_int) && int48signal == 0) { //maybe with stat_.oam_int|| as per doc
      emu_->memory()->interrupt_flag() |= 2;
      int48signal = 1;
    }

    break;
  }
  }


  //static auto mode0_lines = 0, mode1_lines = 0, mode2_lines = 0, mode3_lines = 0;

  if (scanline_dots_ == 5) {
    int48signal = 0;
  }

  if (scanline_dots_ == 456) {


    

    //char debugstr[255];
    //auto total = mode0_counter + mode1_counter + mode2_counter + mode3_counter;
    //sprintf_s(debugstr, "cycles for line %d , %d %d %d %d = %d\n", ly,mode0_counter, mode1_counter, mode2_counter, mode3_counter, total);
    //OutputDebugString(debugstr);
    mode0_counter = mode1_counter = mode2_counter = mode3_counter = 0;

    if (ly++ == 153 && scanline_dots_ == 456) {



      //if (lcdc_.lcd_enable)
      emu_->Render();
      evenodd = !evenodd;
      stat_.mode = 2;
      ly = 0;
      screen_counter_ = 0;
      scanline_dots_ = 0;
      
    }
    scanline_dots_ = 0;
    int40signal = 0;
    int48signal = 0; //needed for demotronic to work correctly with lyc interrupt
  }



}


uint8_t LCDDriver::Read(uint16_t address) {

  switch (address) {
    case 0xFF40:
      return lcdc_.raw;
    case 0xFF41:
      return 0x80|stat_.raw;
    case 0xFF42:
      return scroll_y;
    case 0xFF43:
      return scroll_x;
    case 0xFF44:
      return ly;
    case 0xFF45:
      return lyc;
    case 0xFF46:
      return dma_reg_;
    case 0xFF47:
      return bg_pallete_data;
    case 0xFF48:
      return obj_pallete1_data;
    case 0xFF49:
      return obj_pallete2_data;
    case 0xFF4A:
      return wy;
    case 0xFF4B:
      return wx;
    case 0xFF4C:
      return lcdmode_;
    case 0xFF51:
      return 0xFF;//hdma.src.high;
    case 0xFF52:
      return 0xFF;// hdma.src.low;
    case 0xFF53:
      return 0xFF;// hdma.dest.high;
    case 0xFF54:
      return 0xFF;// hdma.dest.low;
    case 0xFF55:
      return hdma.ff55;

    case 0xFF68:
      return cgb_bgpal_index;
    case 0xFF69:
      return  cgb_bgpal[cgb_bgpal_rindex];
    case 0xFF6A:
      return  cgb_sppal_index;
    case 0xFF6B:
      return cgb_sppal_data;
    case 0xFF6C:
      return pallock_;
  }
  return 0;
}

void LCDDriver::Write(uint16_t address, uint8_t data) {
  switch (address) {
    case 0xFF40:{
      int change = (lcdc_.raw & 0x80) ^ (data & 0x80);
      lcdc_.raw = data;
      if (change) {
        if (lcdc_.lcd_enable) {
          screen_counter_ = 0;
          scanline_dots_ = 7;//4-7 work ,, check
          sprite_bug_counter = 0;
        } else {
          int48signal = 0;
          screen_counter_ = 0;
          scanline_dots_ = 0;
        }

        ly = 0;
        stat_.mode = 2;
      }
      break;
    }
    case 0xFF41:
      stat_.raw = (data & ~0x7) | (stat_.raw&0x7);
      break;
    case 0xFF42:
      scroll_y = data;
      break;
    case 0xFF43:
      scroll_x = data;
      break;
      break;
    case 0xFF44:
      ly = 0;
      break;
    case 0xFF45:
      lyc = data;
      stat_.coincidence = 0;
      break;
    case 0xFF46: {
      dma_reg_ = data;

      //char str[125];
      //sprintf_s(str,"sprite dma during mode %d\n",stat_.mode);
      //sprintf_s(str, "dma initiated value %02x \n", data);
      //OutputDebugString(str);
      
      //test : put the dma routine in memory tick 
      /*uint16_t srcaddr = data<<8;
      auto dest = emu_->memory()->oam();
      for (int i=0;i<160;++i)
        *dest++ = emu_->memory()->ClockedRead8(srcaddr++);
        */
      
      memset(&emu_->memory()->dma_request, 0, sizeof(emu_->memory()->dma_request));
      emu_->memory()->dma_request.dest = emu_->memory()->oam();
      emu_->memory()->dma_request.source_address = data << 8;
      emu_->memory()->dma_request.transfer_counter = 161;
      


      break;
    }
    case 0xFF47:
      bg_pallete_data = data;
      bg_pal[0] = data&0x3;
      bg_pal[1] = (data>>2)&0x3;
      bg_pal[2] = (data>>4)&0x3;
      bg_pal[3] = (data>>6)&0x3;
      //UpdateCGBPalBasedonGBPal(0);
      break;
    case 0xFF48:
      obj_pallete1_data = data;
      obj_pal1[0] = data&0x3;
      obj_pal1[1] = (data>>2)&0x3;
      obj_pal1[2] = (data>>4)&0x3;
      obj_pal1[3] = (data>>6)&0x3;
      //UpdateCGBPalBasedonGBPal(1);
      break;
    case 0xFF49:
      obj_pallete2_data = data;
      obj_pal2[0] = data&0x3;
      obj_pal2[1] = (data>>2)&0x3;
      obj_pal2[2] = (data>>4)&0x3;
      obj_pal2[3] = (data>>6)&0x3;
      //UpdateCGBPalBasedonGBPal(2);
      break;
    case 0xFF4A:
      wy = data;
      break;
    case 0xFF4B:
      wx = data;
      break;
    case 0xFF4C:
      lcdmode_ = data;
      break;
    case 0xFF51:
      hdma.src.high = data;
      break;
    case 0xFF52:
      hdma.src.low = data;
      break;
    case 0xFF53:
      hdma.dest.high = data;
      break;
    case 0xFF54:
      hdma.dest.low = data;
      break;
    case 0xFF55:
      if (emu_->mode() == EmuMode::EmuModeGBC) {
        hdma.length = ((data&0x7F)+1)<<4;

        if (hdma.active) {
          if (data & 0x80) {
            hdma.ff55 = data & 0x7F;
          } else {
            hdma.ff55 = 0xFF;
            hdma.active = 0;
          }
        } else {
          hdma.mode = (data&0x80)>>7;
          if (hdma.mode == 0) { //GDMA
            auto src = hdma.src.raw&0xFFF0;
            auto dest = 0x8000|(hdma.dest.raw&0x1FF0);
           // memcpy(dest,src,hdma.length);

            for (int i = 0; i < hdma.length; ++i) {
             auto data= emu_->memory()->Read8(src+i);
              emu_->memory()->Write8(dest+i,data);
            }
           

            hdma.dest.raw += hdma.length;
            hdma.src.raw += hdma.length;
            OutputDebugString("done gdma\n");
          } else { //HDMA
            hdma.active = 1;
            hdma.ff55 = data & 0x7F;
            OutputDebugString("start hdma\n");
            //check
            //if (stat_.mode == 0)
           //   DoHDMA();
          }

        }

        
        
      }
      break;


    case 0xFF68:
      cgb_bgpal_index = data;
      cgb_bgpal_rindex = cgb_bgpal_index & 0x3F;
      cgb_bgpal_windex = cgb_bgpal_index & 0x3F;
      break;
    case 0xFF69:
      cgb_bgpal_data = data;
      cgb_bgpal[cgb_bgpal_windex&0x3F] = data;
      if (cgb_bgpal_index&0x80)
        ++cgb_bgpal_windex;
      break;
    case 0xFF6A:
      cgb_sppal_index = data;
      break;
    case 0xFF6B:
      cgb_sppal_data = data;
      cgb_sppal1[cgb_sppal_index&0x3F] = cgb_sppal2[cgb_sppal_index&0x3F] = data;
      if (cgb_sppal_index&0x80)
        ++cgb_sppal_index;
      break;
    case 0xFF6C:
      pallock_ = data;
      break;
  }
}


void LCDDriver::RenderCGBBGPixel(ColorMapLine* cmline) {
  uint8_t mapoffset = ((ly + scroll_y) & 0xFF) >> 3; //was & 0xFF, check
  //static uint8_t lineoffset =  ((scroll_x >> 3)) & 0x1F;
  //static auto y =  (ly + scroll_y) & 7;
  //static auto x =  scroll_x & 7;
  const auto vram = emu_->memory()->vram();
  const auto tiledata = &vram[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
  const auto tilemap = &vram[lcdc_.bg_tile_map == 0 ? 0x1800 : 0x1C00];
  const auto tileattr = &vram[lcdc_.bg_tile_map == 0 ? 0x3800 : 0x3C00];
  auto incx = [&]() {
    ++bgparams.x;
    if (bgparams.x == 8) {
      bgparams.x = 0;
      bgparams.lineoffset = (bgparams.lineoffset + 1) & 0x1F;

    }
  };

  if (pixel_counter_ == 0 && scroll_x % 8 != 0) {

    //++dot_delay_; //causes issues, in demotronic so far, check others
    //return;
  }

  if (pixel_counter_ == 0) {
    bgparams.lineoffset = ((scroll_x >> 3)) & 0x1F;
    bgparams.y = (ly + scroll_y) & 7;
    bgparams.x = scroll_x & 7;
  }

  //int i = scanline_counter_ - 81;
  //char debugstr[128];
 // sprintf_s(debugstr, "scan line %d %d %d\n", i,x,scroll_x);
  //OutputDebugString(debugstr);
  {
    auto attr = tileattr[(mapoffset << 5) + bgparams.lineoffset];
    auto palindex = attr & 0x7;

    auto tileindex = tilemap[(mapoffset << 5) + bgparams.lineoffset];
    if ((lcdc_.tile_data == 0)) {
      if (tileindex < 128) tileindex += 128;
      else tileindex -= 128;
    }
    uint8_t* tile = &tiledata[(tileindex << 4) | ((attr & 0x8) << 10)];
    if (attr & 0x40)
      bgparams.y = 7 - bgparams.y;
    uint8_t bgcolor;
    bgcolor = (attr & 0x20) ? pixel2((bgparams.x), bgparams.y) : pixel2((7 - bgparams.x), bgparams.y);

    incx();


    if (enable_bg_ == true) {
      cmline[pixel_counter_].pixel = ((cgb_bgpal[(palindex << 3) | (bgcolor << 1) | 1] << 8) | cgb_bgpal[(palindex << 3) | (bgcolor << 1)]) & 0x7FFF;
    }
  

    cmline[pixel_counter_].orgcol = bgcolor;
    cmline[pixel_counter_].bgpriority = lcdc_.bgdisplay;
    cmline[pixel_counter_].bgoveroam = (attr & 0x80) == 0x80;

  }
}

//check
void LCDDriver::RenderCGBWindowPixel(ColorMapLine* cmline) {
  if (lcdc_.window_enable == 1) {
    auto mapoffset = ((ly - wy) & 0xFF) >> 3;
    static auto lineoffset = 0;
    static auto y = (ly - wy) & 7;
    static auto x = 0;
    const auto vram = emu_->memory()->vram();
    const auto tiledata = &vram[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
    const auto tilemap = &vram[lcdc_.window_tile_map == 0 ? 0x1800 : 0x1C00];
    const auto tileattr = &vram[lcdc_.window_tile_map == 0 ? 0x3800 : 0x3C00];


    auto incx = []() {
      ++x;
      if (x == 8) {
        x = 0;
        lineoffset = (lineoffset + 1) & 0x1F;
      }
    };

    if (pixel_counter_ == (wx - 7))  {
      mapoffset = ((ly - wy) & 0xFF) >> 3;
      lineoffset = 0;
      y = (ly - wy) & 7;
      x = 0;
    }

    if ((ly >= wy) && (wx >= 7 && wx <= 166)) {

      //for (int i = (wx - 7); i < 160; ++i)
      if (pixel_counter_>= (wx - 7) && pixel_counter_ < 160)  {
        auto attr = tileattr[(mapoffset << 5) + lineoffset];
        auto palindex = attr & 0x7;

        auto tileindex = tilemap[(mapoffset << 5) + lineoffset];
        if (lcdc_.tile_data == 0) {
          if (tileindex < 128) tileindex += 128;
          else tileindex -= 128;
        }
        uint8_t* tile = &tiledata[(tileindex << 4) | ((attr & 0x8) << 10)];
        if (attr & 0x20)
          y = 7 - y;
        uint8_t bgcolor = pixel((7 - x));
        if (attr & 0x10)
          bgcolor = pixel((x));
        incx();
        
        if (enable_window_ == true) {
          cmline[pixel_counter_].pixel = ((cgb_bgpal[(palindex << 3) + (bgcolor << 1) + 1] << 8) | cgb_bgpal[(palindex << 3) + (bgcolor << 1)]) & 0x7FFF;
        }
        cmline[pixel_counter_].orgcol = bgcolor;
        cmline[pixel_counter_].bgpriority = lcdc_.bgdisplay;
        
          cmline[pixel_counter_].bgoveroam = (attr & 0x80) == 0x80;//check
        cmline[pixel_counter_].windowoveroam = (attr & 0x80) == 0x80;//check

      }

    }
  }
}



void LCDDriver::RenderCGBSpritePixel(ColorMapLine* cmline) {
  auto vram = emu_->memory()->vram();
  auto sprites = (GBCSprite*)emu_->memory()->oam();
  uint8_t sprite_count = 10;
  uint8_t* tiledata = &vram[0];


  if (lcdc_.sprite_enable == 1) {
    
    //&&sprite_count!=0
    for (int j = 0; j < 40; ++j) {
      
      int16_t spritey = sprites[j].y - 16;
      int16_t spritex = sprites[j].x - 8;
      if (ly >= (spritey) && ly < (spritey + (8 << lcdc_.sprite_size))) { //same line
        if (lcdc_.sprite_size) {
          sprites[j].tileindex &= ~0x01;
        }
        const uint8_t* tile = &tiledata[(sprites[j].tileindex << 4) | (sprites[j].attr.vrambank << 13)];
        uint8_t y = ly - spritey;

        if (sprites[j].attr.yflip)
          y = ((7 << lcdc_.sprite_size)) - y;

        //for (int x = 0; x < 8; ++x) 
        if (pixel_counter_ >= spritex && pixel_counter_ < (spritex + 8)) {
          int x = pixel_counter_ - spritex;//offset
          uint8_t p = 0;
          if (sprites[j].attr.xflip) {
            p = pixel((x));
          }
          else {
            p = pixel((7 - x));
          }
          uint8_t* pal = sprites[j].attr.pal == 0 ? cgb_sppal1 : cgb_sppal2;

          auto& cmp = cmline[pixel_counter_];

          if ( (  ((cmp.bgoveroam == false) && (cmp.bgpriority == true)) || (cmp.bgpriority == false))
            && (sprites[j].attr.priority == 0 || (cmp.orgcol == 0)) && (p != 0) && ((x + spritex) >= 0)) {

            if (enable_sprite_ == true) {
              cmp.pixel =  ((pal[(sprites[j].attr.cgbpalnum << 3) + (p << 1) + 1] << 8) | pal[(sprites[j].attr.cgbpalnum << 3) + (p << 1)]) & 0x7FFF;
             // cmp.pixel = cmp.bgoveroam * 0xFF00 + lcdc_.bgdisplay * 0xFF;
            }

            if (x == 0) {
              dot_delay_ += 3;//avg per sprite based on my calc
            }
          }

        }

        --sprite_count;
        
      }
    }

  }
}


void LCDDriver::RenderDMGBGPixel(ColorMapLine* cmline) {
  auto mapoffset = ((ly + scroll_y) & 0xFF) >> 3;
  static auto lineoffset = ((scroll_x >> 3)) & 0x1F;
  static auto y = (ly + scroll_y) & 7;
  static auto x = scroll_x & 7;
  const auto vram = emu_->memory()->vram();


  auto incx = []() {
    ++x;
    if (x == 8) {
      x = 0;
      lineoffset = (lineoffset + 1) & 0x1F;
    }
  };

  if (pixel_counter_ == 0) {
    lineoffset = ((scroll_x >> 3)) & 0x1F;
    y = (ly + scroll_y) & 7;
    x = scroll_x & 7;
  }

  if (lcdc_.bgdisplay == 0) {
    //for (int i=0;i<160;++i) {
    cmline[pixel_counter_].pixel = 0;
    //}
  }
  else {
    uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
    uint8_t* bgtilemap = &vram[lcdc_.bg_tile_map == 0 ? 0x1800 : 0x1C00];

    //for (int i = 0; i < 160; ++i) {
    auto tileindex = bgtilemap[(mapoffset << 5) + lineoffset];
    if (lcdc_.tile_data == 0) {
      if (tileindex < 128) tileindex += 128;
      else tileindex -= 128;
    }
    uint8_t* tile = &tiledata[(tileindex << 4)];
    uint8_t bgcolor = pixel((7 - x));
    incx();

    if (enable_bg_) {
      cmline[pixel_counter_].pixel = bg_pal[bgcolor];
    }
    else {
      cmline[pixel_counter_].pixel = 0;
    }

    cmline[pixel_counter_].orgcol = bgcolor;

  }
}



void LCDDriver::RenderDMGWindowPixel(ColorMapLine* cmline) {
  if (lcdc_.window_enable == 1) {
    auto mapoffset = ((ly - wy) & 0xFF) >> 3;
    static auto lineoffset = 0;
    static auto y = (ly - wy) & 7;
    static auto x = 0;

    auto incx = []() {
      ++x;
      if (x == 8) {
        x = 0;
        lineoffset = (lineoffset + 1) & 0x1F;
      }
    };

    if (pixel_counter_ == 0) {
      lineoffset = 0;
      y = (ly - wy) & 7;
      x = 0;
    }

    if ((ly >= wy) && (wx >= 7 && wx <= 166)) {
      auto vram = emu_->memory()->vram();
      uint8_t* tiledata = &vram[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
      uint8_t* tilemap = &vram[lcdc_.window_tile_map == 0 ? 0x1800 : 0x1C00];
      if (pixel_counter_ >= (wx - 7) && pixel_counter_ < 160) {
        auto tileindex = tilemap[(mapoffset << 5) + lineoffset];
        if (lcdc_.tile_data == 0) {
          if (tileindex < 128) tileindex += 128;
          else tileindex -= 128;
        }
        uint8_t* tile = &tiledata[(tileindex << 4)];
        uint8_t bgcolor = pixel((7 - x));
        incx();
        if (enable_window_) {
          cmline[pixel_counter_].pixel = bg_pal[bgcolor];
        }
        cmline[pixel_counter_].orgcol = bgcolor;
      }
    }
  }
}

void LCDDriver::RenderDMGSpritePixel(ColorMapLine* cmline) {
  uint8_t sprite_count = 10;

 
  if (lcdc_.sprite_enable == 1) {
    uint8_t* tiledata = &emu_->memory()->vram()[0x0000];
    DMGSprite* sprites = (DMGSprite*)emu_->memory()->oam();
    //&&sprite_count!=0
    for (int j = 0; j < 40; ++j) {
      int16_t spritey = sprites[j].y - 16;
      int16_t spritex = sprites[j].x - 8;
      if (ly >= (spritey) && ly < (spritey + (8 << lcdc_.sprite_size))) { //same line
        if (lcdc_.sprite_size)
          sprites[j].tileindex &= ~0x01;
        uint8_t* tile = &tiledata[(sprites[j].tileindex << 4)];
        uint8_t y = ly - spritey;
        uint8_t* pal = sprites[j].attr.pal == 0 ? obj_pal1 : obj_pal2;
        if (sprites[j].attr.yflip)
          y = ((7 << lcdc_.sprite_size)) - y;

        if (pixel_counter_ >= spritex && pixel_counter_ < (spritex + 8)) {
          int x = pixel_counter_ - spritex;//offset
          uint8_t p = 0;
          if (sprites[j].attr.xflip) {
            p = pixel((x));
          }
          else {
            p = pixel((7 - x));
          }

          if (((p != 0) && ((sprites[j].attr.priority == 0) || (cmline[x + spritex].orgcol == 0))) &&
            (x + spritex) >= 0) {

            if (enable_sprite_ == true) {
              cmline[x + spritex].pixel = pal[p];
            }
            if (x == 0) {
              dot_delay_ += 3;//avg per sprite based on my calc
            }
          }
        }

        --sprite_count;
       
      }
    }

  }
}


void LCDDriver::BeginRenderLine() {
  render_line = false;
  if (ly > 143) {
    
    return;
  }

  cmline = &colormap[ly << 8];
   

  if (((lcdscreenmode_ == LCDScreenModeInterlace) && ((evenodd == 0 && !(ly % 2)) || (evenodd == 1 && (ly % 2))))
    || (lcdscreenmode_ == LCDScreenModeProgressive)) {

    //if (emu_->mode() == EmuMode::EmuModeGBC) {
      render_line = true;
    //}
  }
}

void LCDDriver::EndRenderLine() {
  render_line = false;
  auto fbline = &frame_buffer[ly << 8];


  if (emu_->mode() == EmuMode::EmuModeGBC) {
    for (int i = 0; i < 160; ++i) { //256px per line
      *fbline++ = bgr555torgb(cmline->pixel);
      ++cmline;
    }
  } else {
    for (int i = 0; i < 160; ++i) {//256px per line
      *fbline++ = dmg_colors[cmline->pixel];
      ++cmline;
    }
  }

}


}
}