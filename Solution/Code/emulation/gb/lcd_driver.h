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

namespace emulation {
namespace gb {

union LCDControlRegister {
  struct {
    uint8_t bgdisplay:1;
    uint8_t sprite_enable:1;
    uint8_t sprite_size:1;
    uint8_t bg_tile_map:1;
    uint8_t tile_data:1;
    uint8_t window_enable:1;
    uint8_t window_tile_map:1;
    uint8_t lcd_enable:1;

  };
  uint8_t raw;
};

union LCDStatusRegister {
  struct {
    uint8_t mode:2;
    uint8_t coincidence:1;
    uint8_t hblank_int:1;
    uint8_t vblank_int:1;
    uint8_t oam_int:1;
    uint8_t coincidence_inr:1;
    uint8_t _unused:1;

  };
  uint8_t raw;
};

enum LCDScreenMode {LCDScreenModeInterlace,LCDScreenModeProgressive};

class LCDDriver : public Component {
 public:
  uint8_t sprite_bug_counter;
  uint32_t* frame_buffer;
  LCDDriver(){}
  ~LCDDriver() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  void Tick();
  uint8_t Read(uint16_t address);
  void    Write(uint16_t address, uint8_t data);
  void RenderBGLine(uint16_t* cmline);
  void RenderWindowLine(uint16_t* cmline);
  void RenderSpriteLine(uint16_t* cmline);

  void RenderCGBBGLine(uint16_t* cmline);
  void RenderCGBWindowLine(uint16_t* cmline);
  void RenderCGBSpriteLine(uint16_t* cmline);
  void RenderLine();
  void RenderAllBGTiles();
  const LCDControlRegister& lcdc() { return lcdc_; }
  const LCDStatusRegister& stat() { return stat_; }
 private:
   struct {
     uint8_t* oam;
     bool start;
   }spritedma;
   struct {
     union {
      struct {
        uint8_t low;
        uint8_t high;
      };
      uint16_t raw;
     }src;
     union {
      struct {
        uint8_t low;
        uint8_t high;
      };
      uint16_t raw;
     }dest;
    uint16_t length;
    uint8_t mode;
    uint8_t active() {
      return length == 0; //0 is active
    }
   }hdma;
   uint16_t* colormap;
   LCDControlRegister lcdc_;
   LCDStatusRegister stat_;
   uint8_t scroll_x,scroll_y;
   uint8_t ly,lyc;
   uint8_t wx,wy;
   uint8_t bg_pal[4],obj_pal1[4],obj_pal2[4];
   uint8_t bg_pallete_data;
   uint8_t obj_pallete1_data;
   uint8_t obj_pallete2_data;
   uint8_t cgb_bgpal_index,cgb_bgpal_data,cgb_sppal_index,cgb_sppal_data;
   uint8_t cgb_bgpal[64];
   uint8_t cgb_sppal[64];
   uint64_t mode3_extra_cycles;
   uint64_t counter1,counter2;
   LCDScreenMode lcdscreenmode_;
   bool evenodd;
   double vsync,hsync;
};

}
}