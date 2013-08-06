#include "gb.h"


namespace emulation {
namespace gb {

const uint32_t colormap[4] = {0xffffffff,0xffAAAAAA,0xff545454,0xff000000};

void LCDDriver::Initialize(Emu* emu) {
  Component::Initialize(emu);
  frame_buffer = new uint32_t[256*256];
  lcdc_.raw = 0;
  stat_.raw = 0;
  ly = 0;
  lyc = 0;
  scroll_x = scroll_y = 0;
  wx = wy = 0;
  counter1 = counter2 = 0;
  vsync = 0;
  hsync = 0;
}

void LCDDriver::Deinitialize() {
  auto ioports = emu_->memory()->ioports();
  delete [] frame_buffer;  
}

void LCDDriver::Step(double dt) {
	stat_.coincidence = lyc == ly;
  

	switch (stat_.mode) {
		case 0:
			if (counter2==204)
				stat_.mode = 2;
			break;
		case 2:
			if (counter2==284)
				stat_.mode = 3;
			break;
		case 3:
		  if (counter2 == 456) {
	  		RenderLine();
				stat_.mode = 0;
				if (ly == 143) 
					stat_.mode = 1;//vblank period
			}
			break;
		case 1:
			emu_->memory()->interrupt_flag() |= 1;
			break;
	}

	
  if (counter2 == 456) {
		if (ly == 153 && counter2 == 456) {
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

	/*hsync += dt;
  if (hsync >= 0.10871928680147858230050010871929) {
    hsync = 0;
  }

  vsync += dt;
  if (vsync >= 16.742005692281935375858027791729) {

		
    vsync = 0;
    
		
  }*/
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
        dest[i] = emu_->memory()->Read8(srcaddr++);
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


void LCDDriver::RenderBGLine() {
	auto mapoffset = ((ly+scroll_y)&0xFF) >> 3;
	auto lineoffset = ((scroll_x>>3))&0x1F;
	auto y = (ly + scroll_y) & 7;
	auto x = scroll_x & 7;
	auto readTile = [&](){
		 uint8_t* bgtilemap = &emu_->memory()->vram()[lcdc_.bg_tile_map ==0?0x1800:0x1C00];
			auto tileindex = bgtilemap[(mapoffset<<5) + lineoffset];
			if(lcdc_.tile_data == 0) {
				int8_t d = tileindex;
				d+=128;
				tileindex = d;
			}
			return tileindex;
	};

	if (lcdc_.bgdisplay == 0) {
		for (int i=0;i<256;++i) {
			frame_buffer[((ly)<<8)+(i)] = colormap[0];
		}
	} else {
		uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data ==0?0x0800:0x0000];
		for (int i=0;i<256;++i) {
			auto tileindex = readTile();
			uint8_t* tile = &tiledata[(tileindex<<4)];
			#define pixel(bit) ((tile[y<<1]>>(bit))&0x1+((tile[(y<<1)+1]>>(bit))<<1))
			frame_buffer[((ly)<<8)+i] = colormap[bg_pal[pixel((7-x))]];
			#undef pixel
			++x;
			if (x==8) {
				x = 0;
				lineoffset = (lineoffset+1)&0x1F;
			}
		}
	}
}

void LCDDriver::RenderWindowLine() {
	if (lcdc_.window_enable == 1) {
		uint8_t* tiledata = &emu_->memory()->vram()[lcdc_.tile_data ==0?0x0800:0x0000];
		uint8_t* tilemap = &emu_->memory()->vram()[lcdc_.window_tile_map ==0?0x1800:0x1C00];
		for (int i=0;i<32;++i) {
			auto mapoffset = ((ly)&0xFF) >> 3;
			auto lineoffset = (i)&0x1F;
			auto tileindex = tilemap[(mapoffset<<5) + lineoffset];
			auto y = (ly) & 7;
			auto x = 0;
			if(lcdc_.tile_data == 0) {
				int8_t d = tileindex;
				d+=128;
				tileindex = d;
			}
			uint8_t* tile = &tiledata[(tileindex<<4)];

			#define pixel(bit) ((tile[y<<1]>>bit)&0x1+((tile[(y<<1)+1]>>bit)<<1))

			frame_buffer[((ly)<<8)+((i<<3)+0)] = colormap[bg_pal[pixel(7)]];
			frame_buffer[((ly)<<8)+((i<<3)+1)] = colormap[bg_pal[pixel(6)]];
			frame_buffer[((ly)<<8)+((i<<3)+2)] = colormap[bg_pal[pixel(5)]];
			frame_buffer[((ly)<<8)+((i<<3)+3)] = colormap[bg_pal[pixel(4)]];
			
			
			frame_buffer[((ly)<<8)+((i<<3)+4)] = colormap[bg_pal[pixel(3)]];
			frame_buffer[((ly)<<8)+((i<<3)+5)] = colormap[bg_pal[pixel(2)]];
			frame_buffer[((ly)<<8)+((i<<3)+6)] = colormap[bg_pal[pixel(1)]];
			frame_buffer[((ly)<<8)+((i<<3)+7)] = colormap[bg_pal[pixel(0)]];

			#undef pixel
		}
	}
}

void LCDDriver::RenderSpriteLine() {
	if (lcdc_.sprite_enable == 1) {
		int a =1;
	}
}

void LCDDriver::RenderLine() {
	RenderBGLine();
	RenderWindowLine();
	RenderSpriteLine();
}

}
}