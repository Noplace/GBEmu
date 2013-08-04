#pragma once

namespace emulation {
namespace gb {

class Memory : public Component {
 public:
  Memory(){}
  ~Memory() {}
  void Initialize(Emu* emu);
  void Deinitialize();
  uint8_t Read8(uint16_t address);
  void    Write8(uint16_t address, uint8_t data);
  uint8_t* oam() { return oam_; }
  uint8_t* ioports() { return ioports_; }
  uint8_t& interrupt_enable() { return interrupt_enable_register_; }
  uint8_t& interrupt_flag() {  return ioports_[0x0F]; }
 private:
  const uint8_t* rom_;
  uint8_t* vram_;
  uint8_t* wram1_;
  uint8_t* wram2_;
  uint8_t oam_[160];
  uint8_t ioports_[128];
  uint8_t hram_[127];
  uint8_t interrupt_enable_register_;
};

}
}