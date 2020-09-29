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
    uint8_t r = (src & 0x1F) << 3;
    uint8_t g = ((src >> 5) & 0x1F) << 3;
    uint8_t b = ((src >> 10) & 0x1F) << 3;
    return 0xff000000 | (r << 16) | (g << 8) | (b);
  }

  static inline uint32_t rgbtobgr555(uint32_t src) {
    uint8_t r = ((src >> 16) & 0xFF) >> 3;
    uint8_t g = ((src >> 8) & 0xFF) >> 3;
    uint8_t b = ((src) & 0xFF) >> 3;
    return (b << 10) | (g << 5) | (r);
  }


  //const uint32_t pal32[4] = {0xffffffff,0xffAAAAAA,0xff545454,0xff000000};
  //const uint32_t dmg_colors[4] = {0x00000000,0x54000000,0xAA000000,0xFF000000};
  const uint32_t dmg_colors[4] = { 0x00000000,0x54000000,0xAA000000,0xFF000000 };



  /*
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
            //OutputDebugString("bg display changed during mode3\n");
            flag = lcdc_.bgdisplay;
          }
        }

        if (scanline_counter_ >= 80 && scanline_counter_ <= 160) {
          if (sprite_bug_counter != 0) { //80 is 20 cycles if 114 is one scanline total cycles
            if (lcdc_.lcd_enable == 1)
              emu_->memory()->oam()[8 + (rand() % 152)] = rand() & 0xFF;
            --sprite_bug_counter;
          }
        }


        if (scanline_counter_ == 81 ) {
          sprite_bug_counter = 0;
          pixel_counter_ = 0;
          //RenderLine(0, 160);//will update mode3_extra_cycles_
        }

        if (scanline_counter_ >= 81 && scanline_counter_ <= 241) {
          //sprite_bug_counter = 0;
          //pixel_counter_ = 0;
          RenderLine(0,160);//will update mode3_extra_cycles_
        }

        if (scanline_counter_ == 252+mode3_extra_cycles_) {
          stat_.mode = 0;

        }
        break;
      case 0:
        //if (sprite_bug_counter != 0 && lcdc_.lcd_enable == 1) {
        //  emu_->memory()->oam()[8+(rand()%152)] = rand()&0xFF;
        //  --sprite_bug_counter;
        //}
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



  }*/


void LCDDriver::RenderAllBGTiles() {
  auto mapoffset = ((ly) & 0xFF) >> 3;
  auto y = (ly) & 7;
  auto x = 0;
  auto readTile = [&]() {
    uint8_t* bgtilemap = &emu_->memory()->vram()[lcdc_.bg_tile_map == 0 ? 0x1800 : 0x1C00];
    auto tileindex = bgtilemap[(mapoffset << 5)];
    if (lcdc_.tile_data == 1) {
      int8_t d = tileindex;
      d += 128;
      tileindex = d;
    }
    return tileindex;
  };

  if (lcdc_.bgdisplay == 0) {
    for (int i = 0; i < 256; ++i) {
      colormap[((ly) << 8) + (i)].pixel = 0;
    }
  } else {
    uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
    for (int i = 0; i < 256; ++i) {
      auto tileindex = (i >> 3) + (ly >> 3) * 32;
      uint8_t* tile = &tiledata[(tileindex << 4)];

      colormap[((ly) << 8) + i].pixel = bg_pal[pixel((7 - x))];

      ++x;
      if (x == 8) {
        x = 0;
      }
    }
  }
}

void LCDDriver::RenderBGLine(int x0, int x1, ColorMapLine* cmline) {
  auto mapoffset = ((ly + scroll_y) & 0xFF) >> 3;
  auto lineoffset = ((scroll_x >> 3)) & 0x1F;
  auto y = (ly + scroll_y) & 7;
  auto x = scroll_x & 7;
  auto vram = emu_->memory()->vram();


  auto incx = [&x, &lineoffset]() {
    ++x;
    if (x == 8) {
      x = 0;
      lineoffset = (lineoffset + 1) & 0x1F;
    }
  };

  if (lcdc_.bgdisplay == 0) {
    for (int i = 0; i < 160; ++i) {
      cmline[i].pixel = 0;
    }
  } else {
    uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
    uint8_t* bgtilemap = &vram[lcdc_.bg_tile_map == 0 ? 0x1800 : 0x1C00];

    for (int i = 0; i < 160; ++i) {
      auto tileindex = bgtilemap[(mapoffset << 5) + lineoffset];
      if (lcdc_.tile_data == 0) {
        if (tileindex < 128) tileindex += 128;
        else tileindex -= 128;
      }
      uint8_t* tile = &tiledata[(tileindex << 4)];
      uint8_t bgcolor = pixel((7 - x));
      incx();
      cmline[i].pixel = bg_pal[bgcolor];
      cmline[i].orgcol = bgcolor;
    }
  }
}

void LCDDriver::RenderWindowLine(int x0, int x1, ColorMapLine* cmline) {
  if (lcdc_.window_enable == 1) {
    auto mapoffset = ((ly - wy) & 0xFF) >> 3;
    auto lineoffset = 0;
    auto y = (ly - wy) & 7;
    auto x = 0;

    auto incx = [&x, &lineoffset]() {
      ++x;
      if (x == 8) {
        x = 0;
        lineoffset = (lineoffset + 1) & 0x1F;
      }
    };

    if ((ly >= wy) && (wx >= 7 && wx <= 166)) {
      auto vram = emu_->memory()->vram();
      uint8_t* tiledata = &vram[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
      uint8_t* tilemap = &vram[lcdc_.window_tile_map == 0 ? 0x1800 : 0x1C00];
      for (int i = (wx - 7); i < 160; ++i) {
        auto tileindex = tilemap[(mapoffset << 5) + lineoffset];
        if (lcdc_.tile_data == 0) {
          if (tileindex < 128) tileindex += 128;
          else tileindex -= 128;
        }
        uint8_t* tile = &tiledata[(tileindex << 4)];
        uint8_t bgcolor = pixel((7 - x));
        incx();
        cmline[i].pixel = bg_pal[bgcolor];
        cmline[i].orgcol = bgcolor;
      }
    }
  }
}


void LCDDriver::RenderSpriteLine(int x0, int x1, ColorMapLine* cmline) {
  uint8_t sprite_count = 10;

  mode3_extra_cycles_ = 0;
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

        for (int x = 0; x < 8; ++x) {
          uint8_t p = 0;
          if (sprites[j].attr.xflip) {
            p = pixel((x));
          } else {
            p = pixel((7 - x));
          }

          if (((p != 0) && ((sprites[j].attr.priority == 0) || (cmline[x + spritex].orgcol == 0))) &&
            (x + spritex) >= 0)
            cmline[x + spritex].pixel = pal[p];
        }

        --sprite_count;
        mode3_extra_cycles_ += 12;
      }
    }

  }
}


void LCDDriver::RenderCGBBGLine(int x0, int x1, ColorMapLine* cmline) {

  //if (scanline_counter_ > 240)
  //  return;

  uint8_t mapoffset = ((ly + scroll_y) & 0xFF) >> 3;
  uint8_t lineoffset = ((scroll_x >> 3)) & 0x1F;
  auto y = (ly + scroll_y) & 7;
  auto x = scroll_x & 7;

  if (scanline_dots_ == 81) {
    // mapoffset = ((ly + scroll_y) & 0xFF) >> 3;
     // lineoffset = ((scroll_x >> 3)) & 0x1F;
     // y = (ly + scroll_y) & 7;
     // x = scroll_x & 7;
  }
  auto vram = emu_->memory()->vram();


  auto incx = [&x, &lineoffset]() {
    ++x;
    if (x == 8) {
      x = 0;
      lineoffset = (lineoffset + 1) & 0x1F;

    }
  };

  uint8_t* tiledata = &vram[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
  uint8_t* tilemap = &vram[lcdc_.bg_tile_map == 0 ? 0x1800 : 0x1C00];
  uint8_t* tileattr = &vram[lcdc_.bg_tile_map == 0 ? 0x3800 : 0x3C00];

  //int i = scanline_counter_ - 81;
  //char debugstr[128];
 // sprintf_s(debugstr, "scan line %d %d %d\n", i,x,scroll_x);
  //OutputDebugString(debugstr);
  for (int i = 0; i < 160; ++i) {
    auto attr = tileattr[(mapoffset << 5) + lineoffset];
    auto palindex = attr & 0x7;

    auto tileindex = tilemap[(mapoffset << 5) + lineoffset];
    if ((lcdc_.tile_data == 0)) {
      if (tileindex < 128) tileindex += 128;
      else tileindex -= 128;
    }
    uint8_t* tile = &tiledata[(tileindex << 4) | ((attr & 0x8) << 10)];
    if (attr & 0x40)
      y = 7 - y;
    uint8_t bgcolor;
    bgcolor = (attr & 0x20) ? pixel((x)) : pixel((7 - x));

    incx();

    if (i < 160) {
      cmline[i].pixel = ((cgb_bgpal[(palindex << 3) | (bgcolor << 1) | 1] << 8) | cgb_bgpal[(palindex << 3) | (bgcolor << 1)]) & 0x7FFF;
      cmline[i].orgcol = bgcolor;
      cmline[i].bgpriority = lcdc_.bgdisplay;
      cmline[i].bgoveroam = (attr & 0x80) == 0x80;
    }
  }

}


void LCDDriver::RenderCGBWindowLine(int x0, int x1, ColorMapLine* cmline) {
  if (lcdc_.window_enable == 1) {
    auto mapoffset = ((ly - wy) & 0xFF) >> 3;
    auto lineoffset = 0;
    auto y = (ly - wy) & 7;
    auto x = 0;
    auto vram = emu_->memory()->vram();
    auto incx = [&x, &lineoffset]() {
      ++x;
      if (x == 8) {
        x = 0;
        lineoffset = (lineoffset + 1) & 0x1F;
      }
    };

    if ((ly >= wy) && (wx >= 7 && wx <= 166)) {
      uint8_t* tiledata = &vram[lcdc_.tile_data == 0 ? 0x0800 : 0x0000];
      uint8_t* tilemap = &vram[lcdc_.window_tile_map == 0 ? 0x1800 : 0x1C00];
      uint8_t* tileattr = &vram[lcdc_.window_tile_map == 0 ? 0x3800 : 0x3C00];

      for (int i = (wx - 7); i < 160; ++i) {
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
        cmline[i].pixel = ((cgb_bgpal[(palindex << 3) + (bgcolor << 1) + 1] << 8) | cgb_bgpal[(palindex << 3) + (bgcolor << 1)]) & 0x7FFF;
        cmline[i].orgcol = bgcolor;


        cmline[i].bgpriority = lcdc_.bgdisplay;
        cmline[i].bgoveroam = (attr & 0x80) == 0x80;

      }

    }
  }
}


void LCDDriver::RenderCGBSpriteLine(int x0, int x1, ColorMapLine* cmline) {
  uint8_t sprite_count = 10;

  mode3_extra_cycles_ = 0;
  if (lcdc_.sprite_enable == 1) {
    auto vram = emu_->memory()->vram();
    GBCSprite* sprites = (GBCSprite*)emu_->memory()->oam();
    //&&sprite_count!=0
    for (int j = 0; j < 40; ++j) {
      uint8_t* tiledata = &vram[0];

      int16_t spritey = sprites[j].y - 16;
      int16_t spritex = sprites[j].x - 8;
      if (ly >= (spritey) && ly < (spritey + (8 << lcdc_.sprite_size))) { //same line
        if (lcdc_.sprite_size)
          sprites[j].tileindex &= ~0x01;
        uint8_t* tile = &tiledata[(sprites[j].tileindex << 4) | (sprites[j].attr.vrambank << 13)];
        uint8_t y = ly - spritey;

        if (sprites[j].attr.yflip)
          y = ((7 << lcdc_.sprite_size)) - y;

        for (int x = 0; x < 8; ++x) {
          uint8_t p = 0;
          if (sprites[j].attr.xflip) {
            p = pixel((x));
          } else {
            p = pixel((7 - x));
          }
          uint8_t* pal = sprites[j].attr.pal == 0 ? cgb_sppal1 : cgb_sppal2;

          auto& cmp = cmline[x + spritex];


          //cgb mode
         /* if ( ((cmp&0x8000)==0) || //global priority

            (((cmp&0x10000)==0) && //bg not taking priority
            (sprites[j].attr.priority==0||((cmp&0x60000)==0))) //either sprite priority or bg col 0

            &&(p!=0)&&((x+spritex)>=0)
            )
            cmp = ((pal[(sprites[j].attr.cgbpalnum<<3)+(p<<1)+1]<<8)|pal[(sprites[j].attr.cgbpalnum<<3)+(p<<1)])&0x7FFF;
          */

          if (
            (((cmp.bgoveroam == false) && (cmp.bgpriority == true)) || (cmp.bgpriority == false))

            && (sprites[j].attr.priority == 0 || (cmp.orgcol == 0)) && (p != 0) && ((x + spritex) >= 0))
            cmp.pixel = ((pal[(sprites[j].attr.cgbpalnum << 3) + (p << 1) + 1] << 8) | pal[(sprites[j].attr.cgbpalnum << 3) + (p << 1)]) & 0x7FFF;
        }

        --sprite_count;
        mode3_extra_cycles_ += 12;
      }
    }

  }
}





//not using x0,x1
void LCDDriver::RenderLine(int x0, int x1) {
  if (ly > 143)
    return;
  auto cmline = &colormap[ly << 8];

  //either display whole time if progressive, or based on ly if interlace
  if (((lcdscreenmode_ == LCDScreenModeInterlace) && ((evenodd == 0 && !(ly % 2)) || (evenodd == 1 && (ly % 2))))
    || (lcdscreenmode_ == LCDScreenModeProgressive)) {

    if (emu_->mode() == EmuMode::EmuModeGBC) {

      RenderCGBBGLine(x0,x1,cmline);
      RenderCGBWindowLine(x0, x1, cmline); 
      RenderCGBSpriteLine(x0,x1,cmline);

     /* if (dot_delay_ == 0) {
        RenderCGBBGPixel(cmline);
        RenderCGBWindowPixel(cmline);
        RenderCGBSpritePixel(cmline);

        ++pixel_counter_;
      } else {
        --dot_delay_;
      }*/


      if (pixel_counter_ == 160) {
        auto fbline = &frame_buffer[ly << 8];
        for (int i = 0; i < 160; ++i) { //256px per line
          *fbline++ = bgr555torgb(cmline->pixel);
          ++cmline;
        }
      }


    } else {


      /*if (dot_delay_ == 0) {
        RenderDMGBGPixel(cmline);
        RenderDMGWindowPixel(cmline);
        RenderDMGSpritePixel(cmline);

        ++pixel_counter_;
      } else {
        --dot_delay_;
      }*/


      //RenderAllBGTiles();
      RenderBGLine(x0,x1,cmline);
      RenderWindowLine(x0,x1,cmline);
      RenderSpriteLine(x0,x1,cmline);
      if (pixel_counter_ == 160) {
        auto fbline = &frame_buffer[ly << 8];
        for (int i = 0; i < 160; ++i) {//256px per line
          *fbline++ = dmg_colors[cmline->pixel];
          ++cmline;
        }
      }
    }

  }


}

}
}