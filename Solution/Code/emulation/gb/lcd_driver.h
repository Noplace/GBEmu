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

class LCDDriver : public Component {
 public:
  uint32_t* frame_buffer;
  LCDDriver(){}
  ~LCDDriver() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Step(double dt);
  uint8_t Read(uint16_t address);
  void    Write(uint16_t address, uint8_t data);
	void RenderBGLine();
	void RenderWindowLine();
	void RenderSpriteLine();
	void RenderLine();
	void RenderAllBGTiles();
	const LCDControlRegister& lcdc() { return lcdc_; }
	const LCDStatusRegister& stat() { return stat_; }
 private:
   uint8_t* colormap;
   LCDControlRegister lcdc_;
   LCDStatusRegister stat_;
   uint8_t scroll_x,scroll_y;
   uint8_t ly,lyc;
   uint8_t wx,wy;
   uint8_t bg_pal[4],obj_pal1[4],obj_pal2[4];
   uint8_t bg_pallete_data;
   uint8_t obj_pallete1_data;
   uint8_t obj_pallete2_data;
   uint64_t counter1,counter2;
   double vsync,hsync;
};

}
}