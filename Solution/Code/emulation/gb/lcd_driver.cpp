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
const uint32_t dmg_colors[4] = {0x00000000,0x54000000,0xAA000000,0xFF000000};



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
  if (emu_->mode() != EmuModeGBC) return; //we should use a seperate flag to force mono to color regardless of gb mode/type
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
  screen_counter_ = scanline_counter_ = 0;
  vsync = 0;
  hsync = 0;
  sprite_bug_counter = 0;
  mode3_extra_cycles_ = 0;
  cgb_bgpal_index=cgb_bgpal_data=cgb_sppal_index=cgb_sppal_data=0;
  int48signal = 0;
}
/*todo variable time for mode 3 and 0
mode 3:
between 4194304*0.00004137=173.51835648 and //172
4194304*0.00007069=296.49534976 //296
*/

void LCDDriver::DoHDMA() {
  if (hdma.mode == 1 && hdma.active == 1) {
    uint16_t offset = 0;

    auto src = emu_->memory()->GetMemoryPointer(hdma.src.raw&0xFFF0);
    auto dest = emu_->memory()->GetMemoryPointer(0x8000|(hdma.dest.raw&0x1FF0));
    memcpy(dest,src,0x10);
    hdma.length -= 0x10;
    hdma.src.raw += 0x10;
    hdma.dest.raw += 0x10;
    --hdma.ff55;
    if (hdma.ff55 == 0xFF)
      hdma.active = 0;
  }

}

void LCDDriver::Tick() {
  ++scanline_counter_; //line clock
  ++screen_counter_;//screen clock

  stat_.coincidence = lyc == ly;


  //dma 
  if (spritedma.start == true) { //launched

  }




  switch (stat_.mode) {
    case 2:
      if (scanline_counter_==1) {
        if (stat_.oam_int || (stat_.coincidence_inr && stat_.coincidence)) { //lyc interrupts occurs at start of mode 2
          if (int48signal == 0) {
            emu_->memory()->interrupt_flag() |= 2;
            int48signal = 1;
          }
        }
      
      }
      if (scanline_counter_==80)
        stat_.mode = 3;
      break;
    case 3:
      {
        static auto flag = lcdc_.bgdisplay;
        if (flag != lcdc_.bgdisplay) {
          OutputDebugString("bg display changed during mode3\n");
          flag = lcdc_.bgdisplay;
        }
      }

      if (sprite_bug_counter != 0) {
        if (lcdc_.lcd_enable == 1 )
          emu_->memory()->oam()[8+(rand()%152)] = rand()&0xFF; 
        --sprite_bug_counter;
      }

      if (scanline_counter_ == 81) {
        RenderLine(0,160);//will update mode3_extra_cycles_
      }

      if (scanline_counter_ == 252+mode3_extra_cycles_) {
        stat_.mode = 0;
       
      }
      break;
    case 0:
      /*if (sprite_bug_counter != 0 && lcdc_.lcd_enable == 1) {
        emu_->memory()->oam()[8+(rand()%152)] = rand()&0xFF; 
        --sprite_bug_counter;
      }*/
      if (scanline_counter_ == 252+mode3_extra_cycles_+1) {
        if (stat_.hblank_int) {
          if (int48signal == 0) {
            emu_->memory()->interrupt_flag() |= 2;
            int48signal = 1;
          }
        }
      }

      if (scanline_counter_==456) {
        
        //DoHDMA();
        if (ly == 143) {
          stat_.mode = 1;//vblank period
        } else {
          stat_.mode = 2;

        }
      }
      break;
    case 1:{
      if (ly == 144 && scanline_counter_ == 1) {
        emu_->memory()->interrupt_flag() |= 1;
        if (stat_.vblank_int) { //either here or in mode 1 all the time//doubt it
          if (int48signal == 0) {
            emu_->memory()->interrupt_flag() |= 2;
            int48signal = 1;
          }
        }
      }
      break;
    }
  }

  
  if (scanline_counter_ == 456) {
    if (ly++ == 153 && scanline_counter_ == 456) {
      //if (lcdc_.lcd_enable)
      emu_->Render();
      evenodd = !evenodd;
      stat_.mode = 2;
      ly = 0;
      screen_counter_ = 0;
      scanline_counter_ = 0;
    } 
    scanline_counter_ = 0;
    int48signal = 0;
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
      return hdma.src.high;
    case 0xFF52:
      return hdma.src.low;
    case 0xFF53:
      return hdma.dest.high;
    case 0xFF54:
      return hdma.dest.low;
    case 0xFF55:
      return hdma.ff55;

    case 0xFF68:
      return cgb_bgpal_index;
    case 0xFF69:
      return  cgb_bgpal_data;
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
          scanline_counter_ = 4;//4-7 work
        } else {
          screen_counter_ = 0;
          scanline_counter_ = 0;
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
      uint16_t srcaddr = data<<8;
      //char str[25];
      //sprintf(str,"sprite dma during mode %d\n",stat_.mode);
      //OutputDebugString(str);
      auto dest = emu_->memory()->oam();
      for (int i=0;i<160;++i)
        *dest++ = emu_->memory()->ClockedRead8(srcaddr++);
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
      if (emu_->mode() == EmuModeGBC) {
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
            auto src = emu_->memory()->GetMemoryPointer(hdma.src.raw&0xFFF0);
            auto dest = emu_->memory()->GetMemoryPointer(0x8000|(hdma.dest.raw&0x1FF0));
            memcpy(dest,src,hdma.length);
            hdma.dest.raw += hdma.length;
            hdma.src.raw += hdma.length;
          } else { //HDMA
            hdma.active = 1;
            hdma.ff55 = data & 0x7F;
            if (stat_.mode == 0)
              DoHDMA();
          }

        }

        
        
      }
      break;


    case 0xFF68:
      cgb_bgpal_index = data;
      break;
    case 0xFF69:
      cgb_bgpal_data = data;
      cgb_bgpal[cgb_bgpal_index&0x3F] = data;
      if (cgb_bgpal_index&0x80)
        ++cgb_bgpal_index;
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

#define pixel(bit) (((tile[y<<1]>>(bit&0x7))&0x1)+((((tile[(y<<1)+1]>>(bit&0x7))<<1)&0x2)))

void LCDDriver::RenderAllBGTiles() {
  auto mapoffset = ((ly)&0xFF) >> 3;
  auto y = (ly) & 7;
  auto x = 0;
  auto readTile = [&](){
      uint8_t* bgtilemap = &emu_->memory()->vram()[lcdc_.bg_tile_map ==0?0x1800:0x1C00];
      auto tileindex = bgtilemap[(mapoffset<<5)];
      if(lcdc_.tile_data == 1) {
        int8_t d = tileindex;
        d+=128;
        tileindex = d;
      }
      return tileindex;
  };

  if (lcdc_.bgdisplay == 0) {
    for (int i=0;i<256;++i) {
      colormap[((ly)<<8)+(i)].pixel = 0;
    }
  } else {
    uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data ==0?0x0800:0x0000];
    for (int i=0;i<256;++i) {
      auto tileindex = (i>>3)+(ly>>3)*32;
      uint8_t* tile = &tiledata[(tileindex<<4)];
      
      colormap[((ly)<<8)+i].pixel = bg_pal[pixel((7-x))];

      ++x;
      if (x==8) {
        x = 0;
      }
    }
  }
}

void LCDDriver::RenderBGLine(int x0,int x1,ColorMapLine* cmline) {
  auto mapoffset = ((ly+scroll_y)&0xFF) >> 3;
  auto lineoffset = ((scroll_x>>3))&0x1F;
  auto y = (ly + scroll_y) & 7;
  auto x = scroll_x & 7;
  auto vram = emu_->memory()->vram();


  auto incx = [&x,&lineoffset](){
      ++x;
      if (x==8) {
        x = 0;
        lineoffset = (lineoffset+1)&0x1F;
      }
  };

  if (lcdc_.bgdisplay == 0) {
    for (int i=0;i<160;++i) {
      cmline[i].pixel = 0;
    }
  } else {
    uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data ==0?0x0800:0x0000];
    uint8_t* bgtilemap = &vram[lcdc_.bg_tile_map ==0?0x1800:0x1C00];

    for (int i=0;i<160;++i) {
      auto tileindex = bgtilemap[(mapoffset<<5) + lineoffset];
      if(lcdc_.tile_data == 0) {
        if(tileindex < 128) tileindex += 128;
        else tileindex -= 128;
      }
      uint8_t* tile = &tiledata[(tileindex<<4)];
      uint8_t bgcolor = pixel((7-x));
      incx();
      cmline[i].pixel = bg_pal[bgcolor];
      cmline[i].orgcol = bgcolor;
    }
  }
}

void LCDDriver::RenderWindowLine(int x0,int x1,ColorMapLine* cmline) {
  if (lcdc_.window_enable == 1) {
    auto mapoffset = ((ly-wy)&0xFF) >> 3;
    auto lineoffset = 0;
    auto y = (ly-wy) & 7;
    auto x = 0;

    auto incx = [&x,&lineoffset](){
        ++x;
        if (x==8) {
          x = 0;
          lineoffset = (lineoffset+1)&0x1F;
        }
    };

    if ((ly >= wy)&&(wx>=7&&wx<=166)) { 
      auto vram = emu_->memory()->vram();
      uint8_t* tiledata = &vram[lcdc_.tile_data ==0?0x0800:0x0000];
      uint8_t* tilemap = &vram[lcdc_.window_tile_map ==0?0x1800:0x1C00];
      for (int i=(wx-7);i<160;++i) {
        auto tileindex = tilemap[(mapoffset<<5) + lineoffset];
        if(lcdc_.tile_data == 0) {
          if(tileindex < 128) tileindex += 128;
          else tileindex -= 128;
        }
        uint8_t* tile = &tiledata[(tileindex<<4)];
        uint8_t bgcolor = pixel((7-x));
        incx();
        cmline[i].pixel = bg_pal[bgcolor];
        cmline[i].orgcol = bgcolor;
      }
    }
  }
}

void LCDDriver::RenderSpriteLine(int x0,int x1,ColorMapLine* cmline) {
  uint8_t sprite_count = 10;
  struct Sprite{
    uint8_t y,x,tileindex;

    union {
      struct {
        uint8_t unused:4;
        uint8_t pal:1;
        uint8_t xflip:1;
        uint8_t yflip:1;
        uint8_t priority:1;
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

  } ;
  mode3_extra_cycles_ = 0;
  if (lcdc_.sprite_enable == 1) {
    uint8_t* tiledata = &emu_->memory()->vram()[0x0000];
    Sprite* sprites = (Sprite*)emu_->memory()->oam();
    //&&sprite_count!=0
    for (int j=0;j<40;++j) {
       int16_t spritey = sprites[j].y-16;
       int16_t spritex = sprites[j].x-8;
       if ( ly >= (spritey) && ly < (spritey+(8<<lcdc_.sprite_size))) { //same line
         if (lcdc_.sprite_size)
           sprites[j].tileindex &= ~0x01;
          uint8_t* tile = &tiledata[(sprites[j].tileindex<<4)];
          uint8_t y = ly-spritey;
          uint8_t* pal = sprites[j].attr.pal == 0?obj_pal1:obj_pal2;
          if (sprites[j].attr.yflip)
            y = ((7<<lcdc_.sprite_size)) - y;
           
          for (int x=0;x<8;++x) {
            uint8_t p = 0;
            if (sprites[j].attr.xflip) {
              p = pixel((x));
            } else {
              p = pixel((7-x));
            }
            
            if (((p!=0)&&((sprites[j].attr.priority==0)||(cmline[x+spritex].orgcol == 0)))&&
              (x+spritex)>=0)
              cmline[x+spritex].pixel = pal[p];
          }

        --sprite_count;
        mode3_extra_cycles_ += 12;
       }
     }

  }
}



void LCDDriver::RenderCGBBGLine(int x0,int x1,ColorMapLine* cmline) {


  uint8_t mapoffset = ((ly+scroll_y)&0xFF) >> 3;
  uint8_t lineoffset = ((scroll_x>>3))&0x1F;
  auto y = (ly + scroll_y) & 7;
  auto x = scroll_x & 7;
  auto vram = emu_->memory()->vram();


  auto incx = [&x,&lineoffset](){
      ++x;
      if (x==8) {
        x = 0;
        lineoffset = (lineoffset+1)&0x1F;
        
      }
  };

  uint8_t* tiledata = &vram[lcdc_.tile_data ==0?0x0800:0x0000];
  uint8_t* tilemap = &vram[lcdc_.bg_tile_map ==0?0x1800:0x1C00];
  uint8_t* tileattr =  &vram[lcdc_.bg_tile_map ==0?0x3800:0x3C00];

  for (int i=0;i<160;++i) {
    auto attr = tileattr[(mapoffset<<5) + lineoffset];
    auto palindex = attr&0x7;
 
    auto tileindex = tilemap[(mapoffset<<5) + lineoffset];
    if((lcdc_.tile_data == 0)) {
      if(tileindex < 128) tileindex += 128;
      else tileindex -= 128;
    }
    uint8_t* tile = &tiledata[(tileindex<<4)|((attr&0x8)<<10)];
    if (attr&0x40)
      y = 7-y;
    uint8_t bgcolor;
    bgcolor = (attr&0x20)?pixel((x)):pixel((7-x));
     
    incx();

    cmline[i].pixel = ((cgb_bgpal[(palindex<<3)|(bgcolor<<1)|1]<<8)|cgb_bgpal[(palindex<<3)|(bgcolor<<1)])&0x7FFF;
    cmline[i].orgcol = bgcolor;
    cmline[i].bgpriority = lcdc_.bgdisplay;
    cmline[i].bgoveroam = (attr&0x80)==0x80;
  }
  
}


void LCDDriver::RenderCGBWindowLine(int x0,int x1,ColorMapLine* cmline) {
  if (lcdc_.window_enable == 1) {
    auto mapoffset = ((ly-wy)&0xFF) >> 3;
    auto lineoffset = 0;
    auto y = (ly-wy) & 7;
    auto x = 0;
    auto vram = emu_->memory()->vram();
    auto incx = [&x,&lineoffset](){
        ++x;
        if (x==8) {
          x = 0;
          lineoffset = (lineoffset+1)&0x1F;
        }
    };

    if ((ly >= wy)&&(wx>=7&&wx<=166)) { 
      uint8_t* tiledata = &vram[lcdc_.tile_data ==0?0x0800:0x0000];
      uint8_t* tilemap = &vram[lcdc_.window_tile_map ==0?0x1800:0x1C00];
      uint8_t* tileattr =  &vram[lcdc_.window_tile_map ==0?0x3800:0x3C00];

      for (int i=(wx-7);i<160;++i) {
        auto attr = tileattr[(mapoffset<<5) + lineoffset];
        auto palindex = attr&0x7;
      

        auto tileindex = tilemap[(mapoffset<<5) + lineoffset];
        if(lcdc_.tile_data == 0) {
          if(tileindex < 128) tileindex += 128;
          else tileindex -= 128;
        }
        uint8_t* tile = &tiledata[(tileindex<<4)|((attr&0x8)<<10)];
        if (attr&0x20)
          y = 7-y;
        uint8_t bgcolor = pixel((7-x));
        if (attr&0x10)
          bgcolor = pixel((x));
        incx();
        cmline[i].pixel = ((cgb_bgpal[(palindex<<3)+(bgcolor<<1)+1]<<8)|cgb_bgpal[(palindex<<3)+(bgcolor<<1)])&0x7FFF;
        cmline[i].orgcol = bgcolor;


        cmline[i].bgpriority = lcdc_.bgdisplay;
        cmline[i].bgoveroam = (attr&0x80)==0x80;
    
      }

    }
  }
}


void LCDDriver::RenderCGBSpriteLine(int x0,int x1,ColorMapLine* cmline) {
  uint8_t sprite_count = 10;
  struct Sprite{
    uint8_t y,x,tileindex;

    union {
      struct {
        uint8_t cgbpalnum:3;
        uint8_t vrambank:1;
        uint8_t pal:1;
        uint8_t xflip:1;
        uint8_t yflip:1;
        uint8_t priority:1;
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

  } ;
  mode3_extra_cycles_ = 0;
  if (lcdc_.sprite_enable == 1) {
    auto vram = emu_->memory()->vram();
    Sprite* sprites = (Sprite*)emu_->memory()->oam();
    //&&sprite_count!=0
    for (int j=0;j<40;++j) {
      uint8_t* tiledata = &vram[0];
     
       int16_t spritey = sprites[j].y-16;
       int16_t spritex = sprites[j].x-8;
       if ( ly >= (spritey) && ly < (spritey+(8<<lcdc_.sprite_size))) { //same line
         if (lcdc_.sprite_size)
           sprites[j].tileindex &= ~0x01;
          uint8_t* tile = &tiledata[(sprites[j].tileindex<<4)|(sprites[j].attr.vrambank<<13)];
          uint8_t y = ly-spritey;
          
          if (sprites[j].attr.yflip)
            y = ((7<<lcdc_.sprite_size)) - y;
           
          for (int x=0;x<8;++x) {
            uint8_t p = 0;
            if (sprites[j].attr.xflip) {
              p = pixel((x));
            } else {
              p = pixel((7-x));
            }
            uint8_t* pal = sprites[j].attr.pal == 0?cgb_sppal1:cgb_sppal2;
             
            auto& cmp = cmline[x+spritex];
            
           
            //cgb mode
           /* if ( ((cmp&0x8000)==0) || //global priority
              
              (((cmp&0x10000)==0) && //bg not taking priority
              (sprites[j].attr.priority==0||((cmp&0x60000)==0))) //either sprite priority or bg col 0
              
              &&(p!=0)&&((x+spritex)>=0)
              )
              cmp = ((pal[(sprites[j].attr.cgbpalnum<<3)+(p<<1)+1]<<8)|pal[(sprites[j].attr.cgbpalnum<<3)+(p<<1)])&0x7FFF;
            */
            
              if (
                (((cmp.bgoveroam==false)&&(cmp.bgpriority==true))||(cmp.bgpriority==false))
                
                &&(sprites[j].attr.priority==0||(cmp.orgcol==0)) &&(p!=0)&&((x+spritex)>=0)  )
                cmp.pixel = ((pal[(sprites[j].attr.cgbpalnum<<3)+(p<<1)+1]<<8)|pal[(sprites[j].attr.cgbpalnum<<3)+(p<<1)])&0x7FFF;
          }

        --sprite_count;
        mode3_extra_cycles_ += 12;
       }
     }

  }
}


void LCDDriver::RenderLine(int x0,int x1) {
  if (ly > 143)
    return; 
  auto cmline = &colormap[ly<<8];

  //either display whole time if progressive, or based on ly if interlace
  if (((lcdscreenmode_==LCDScreenModeInterlace)&&((evenodd == 0 && !(ly%2))||(evenodd == 1 && (ly%2))))
    ||(lcdscreenmode_==LCDScreenModeProgressive)) {

    if (emu_->mode() == EmuModeGBC) {
      RenderCGBBGLine(x0,x1,cmline);
      RenderCGBWindowLine(x0,x1,cmline);
      RenderCGBSpriteLine(x0,x1,cmline);
      auto fbline = &frame_buffer[ly<<8];
        for (int i=0;i<160;++i) { //256px per line
         *fbline++ = bgr555torgb(cmline->pixel);
         ++cmline;
        }
    } else {
      
      //RenderAllBGTiles();
      RenderBGLine(x0,x1,cmline);
      RenderWindowLine(x0,x1,cmline);
      RenderSpriteLine(x0,x1,cmline);

        auto fbline = &frame_buffer[ly<<8];
        for (int i=0;i<160;++i) {//256px per line
          *fbline++ = dmg_colors[cmline->pixel];
          ++cmline;
        }
    }

  }

}

}
}