#pragma once

namespace emulation {
namespace gb {

enum JoypadKeys { JoypadRight=0,JoypadLeft=1,JoypadUp=2,JoypadDown=3,JoypadA=4,JoypadB=5,JoypadSelect=6,JoypadStart=7};

class Memory : public Component {
 public:
  Memory(){}
  ~Memory() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  void Reset();
  uint8_t Read8(uint16_t address);
  void    Write8(uint16_t address, uint8_t data);
	uint8_t* vram() { return vram_; }
  uint8_t* oam() { return oam_; }
  uint8_t* ioports() { return ioports_; }
  uint8_t& interrupt_enable() { return interrupt_enable_register_; }
  uint8_t& interrupt_flag() {  return ioports_[0x0F]; }
	uint16_t Read16(uint16_t address) {
		uint16_t nn;
		nn = Read8(address);
		nn |= Read8(address+1)<<8;
		return nn;
	}
	void JoypadPress(JoypadKeys key);
	void JoypadRelease(JoypadKeys key) {
		//ioports_[0] |= key;
		joypadflags[key] = false;
	}
	void Tick();
 private:
  const uint8_t* rom_;
  uint8_t* vram_;
  uint8_t* wram1_;
  uint8_t* wram2_;
  uint8_t oam_[160];
  uint8_t ioports_[128];
  uint8_t hram_[127];
  uint8_t interrupt_enable_register_;
  uint16_t last_address;
	bool joypadflags[8];
};

}
}