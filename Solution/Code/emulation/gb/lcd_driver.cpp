#include "gb.h"


namespace emulation {
namespace gb {

const uint32_t pal32[4] = {0xffffffff,0xffAAAAAA,0xff545454,0xff000000};

void LCDDriver::Initialize(Emu* emu) {
  Component::Initialize(emu);
  frame_buffer = new uint32_t[256*256];
  lcdc_.raw = 0;
  stat_.raw = 2;
  ly = 0;
  lyc = 0;
  scroll_x = scroll_y = 0;
  wx = wy = 0;
  counter1 = counter2 = 0;
  vsync = 0;
  hsync = 0;
  colormap = new uint8_t[256*256];
}

void LCDDriver::Deinitialize() {
  auto ioports = emu_->memory()->ioports();
  delete [] colormap;
  delete [] frame_buffer;  
}

void LCDDriver::Step(double dt) {
	stat_.coincidence = lyc == ly;
  if (stat_.coincidence_inr && stat_.coincidence)
    emu_->memory()->interrupt_flag() |= 2;

	switch (stat_.mode) {
		case 2:
      if (stat_.oam_int)
        emu_->memory()->interrupt_flag() |= 2;
			if (counter2==80)
				stat_.mode = 3;
			break;
		case 3:
		  if (counter2 == 282) {
	  		RenderLine();
				stat_.mode = 0;
				if (ly == 143) 
					stat_.mode = 1;//vblank period
			}
			break;
		case 0:
      if (stat_.hblank_int)
        emu_->memory()->interrupt_flag() |= 2;

			if (counter2==456)
				stat_.mode = 2;
			break;
		case 1:
        if (stat_.vblank_int)
          emu_->memory()->interrupt_flag() |= 2;
				emu_->memory()->interrupt_flag() |= 1;
			break;
	}

	
  if (counter2 == 456) {
		if (ly == 153 && counter2 == 456) {
      for (int i=0;i<256*256;++i)
        frame_buffer[i] = pal32[colormap[i]];
			emu_->on_render();
			stat_.mode = 0;
			ly = 0;
			counter1 = 0;
			counter2 = 0;
		} else
			++ly;
		counter2 = 0;
	}

  ++counter2; //line clock
  ++counter1;//screen clock
}

uint8_t LCDDriver::Read(uint16_t address) {

  switch (address) {
    case 0xFF40:
      return lcdc_.raw;
    case 0xFF41:
      return stat_.raw;
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
  }
	return 0;
}

void LCDDriver::Write(uint16_t address, uint8_t data) {
  switch (address) {
    case 0xFF40:
      lcdc_.raw = data;
      break;
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
      break;
    case 0xFF46: {
      uint16_t srcaddr = data<<8;
      auto dest = emu_->memory()->oam();
      for (int i=0;i<160;++i)
        *dest++ = emu_->memory()->Read8(srcaddr++);
      break;
    }
    case 0xFF47:
      bg_pallete_data = data;
      bg_pal[0] = data&0x3;
      bg_pal[1] = (data>>2)&0x3;
      bg_pal[2] = (data>>4)&0x3;
      bg_pal[3] = (data>>6)&0x3;
      break;
    case 0xFF48:
      obj_pallete1_data = data;
      obj_pal1[0] = data&0x3;
      obj_pal1[1] = (data>>2)&0x3;
      obj_pal1[2] = (data>>4)&0x3;
      obj_pal1[3] = (data>>6)&0x3;
      break;
    case 0xFF49:
      obj_pallete2_data = data;
      obj_pal2[0] = data&0x3;
      obj_pal2[1] = (data>>2)&0x3;
      obj_pal2[2] = (data>>4)&0x3;
      obj_pal2[3] = (data>>6)&0x3;
      break;
    case 0xFF4A:
      wy = data;
      break;
    case 0xFF4B:
      wx = data;
      break;
  }
}

#define pixel(bit) ((tile[y<<1]>>(bit))&0x1+(((tile[(y<<1)+1]>>(bit))<<1)&0x2))

void LCDDriver::RenderAllBGTiles() {
	auto mapoffset = ((ly)&0xFF) >> 3;
	auto y = (ly) & 7;
	auto x = 0;
	auto readTile = [&](){
		  uint8_t* bgtilemap = &emu_->memory()->vram()[lcdc_.bg_tile_map ==0?0x1800:0x1C00];
			auto tileindex = bgtilemap[(mapoffset<<5)];
			if(lcdc_.tile_data == 0) {
				int8_t d = tileindex;
				d+=128;
				tileindex = d;
			}
			return tileindex;
	};

	if (lcdc_.bgdisplay == 0) {
		for (int i=0;i<256;++i) {
			colormap[((ly)<<8)+(i)] = 0;
		}
	} else {
		uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data ==0?0x0800:0x0000];
		for (int i=0;i<256;++i) {
			auto tileindex = (i>>3)+(ly>>3)*32;
			uint8_t* tile = &tiledata[(tileindex<<4)];
			
			colormap[((ly)<<8)+i] = bg_pal[pixel((7-x))];

			++x;
			if (x==8) {
				x = 0;
			}
		}
	}
}

void LCDDriver::RenderBGLine() {
	auto mapoffset = ((ly+scroll_y)&0xFF) >> 3;
	auto lineoffset = ((scroll_x>>3))&0x1F;
	auto y = (ly + scroll_y) & 7;
	auto x = scroll_x & 7;
	auto readTile = [&](){
		  uint8_t* bgtilemap = &emu_->memory()->vram()[lcdc_.bg_tile_map ==0?0x1800:0x1C00];
			auto tileindex = bgtilemap[(mapoffset<<5) + lineoffset];
			if(lcdc_.tile_data == 0) { //1 && tileindex < 128) {
        //tileindex += 256;
        //tileindex &= 0xFF;
				int8_t d = tileindex;
				d+=128;
				tileindex = d;
			}
			return tileindex;
	};

	if (lcdc_.bgdisplay == 0) {
		for (int i=0;i<256;++i) {
			colormap[((ly)<<8)+(i)] = 0;
		}
	} else {
		uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data ==0?0x0800:0x0000];
		for (int i=0;i<256;++i) {
			auto tileindex = readTile();
			uint8_t* tile = &tiledata[(tileindex<<4)];
			
      uint8_t bgcolor = pixel((7-x));
			
			++x;
			if (x==8) {
				x = 0;
				lineoffset = (lineoffset+1)&0x1F;
      }
      colormap[((ly)<<8)+i] = bg_pal[bgcolor];
		}
	}
}

void LCDDriver::RenderWindowLine() {
	if (lcdc_.window_enable == 1) {
	int  a = 1;

	}
}

void LCDDriver::RenderSpriteLine() {
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
	if (lcdc_.sprite_enable == 1) {
		uint8_t* tiledata = &emu_->memory()->vram()[0x0000];
    Sprite* sprites = (Sprite*)emu_->memory()->oam();

    for (int j=0;j<40;++j) {
       uint8_t spritey = sprites[j].y-16;
       uint8_t spritex = sprites[j].y-8;
       if ( ly >= (spritey) && ly < (spritey+(8<<lcdc_.sprite_size))) { //same line
         if (lcdc_.sprite_size)
           sprites[j].tileindex &= ~0x01;
          uint8_t* tile = &tiledata[(sprites[j].tileindex<<4)];
          uint8_t y = ly-spritey;
          uint8_t* pal = sprites[j].attr.pal == 0?obj_pal1:obj_pal2;
          if (sprites[j].attr.yflip)
            y = ((8<<lcdc_.sprite_size)) - y;
			     
			    for (int x=0;x<8;++x) {
            uint8_t p = 0;
            if (sprites[j].attr.xflip) {
              p = pixel((x));
            } else {
              p = pixel((7-x));
            }
            if (p != 0)
            colormap[((ly)<<8)+x+spritex] = pal[p];
          }

			
       }
     }

    for (int i=0;i<256;++i) {
			
  
      /*auto tileindex = readTile();
			uint8_t* tile = &tiledata[(tileindex<<4)];
			#define pixel(bit) ((tile[y<<1]>>(bit))&0x1+(((tile[(y<<1)+1]>>(bit))<<1)&0x2))
			frame_buffer[((ly)<<8)+i] = colormap[bg_pal[pixel((7-x))]];
			#undef pixel
			++x;
			if (x==8) {
				x = 0;
				lineoffset = (lineoffset+1)&0x1F;
			}*/
		}

	}
}

void LCDDriver::RenderLine() {
  //RenderAllBGTiles();
	RenderBGLine();
	//RenderWindowLine();
	//RenderSpriteLine();
}

}
}