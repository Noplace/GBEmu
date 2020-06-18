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


namespace emulation {
namespace gb {

  //eram_enable only to allow writing to ram

  enum MBCPocketCameraMode { MBCPocketCameraModeRAM=0, MBCPocketCameraModeCamera=1};
class MBCPocketCamera : public MemoryBankController {
 public:
   void Initialize(Cartridge* cartridge) {
     MemoryBankController::Initialize(cartridge);
     eram_enable = 0;
     eram_enable = 0;
     rom_bank_number = 1;
     ram_bank_number = 0;
     mode = 0;

     memset(camera_io, 0, sizeof(camera_io));
     switch (cartridge->header->cartridge_type) {
      case 0x0F:battery_ = true; break;
      case 0x10:battery_ = true; break;
      case 0x13:battery_ = true; break;
     }

     if (battery_)
       cartridge->LoadRam();
    
   }

   void Deinitialize() {
     if (battery_)
       cartridge->SaveRam();
     MemoryBankController::Deinitialize();
   }

   uint8_t* GetMemoryPointer(uint16_t address) {
     if (address >= 0x0000 && address <= 0x3FFF) {
       return &rom_[address];
     }
     else if (address >= 0x4000 && address <= 0x7FFF) {
       return &rom_[(address & 0x3FFF) | ((rom_bank_number) << 14)];
     }
     else if (address >= 0xA000 && address <= 0xBFFF) {

       if ((eram_enable & 0x0A) == 0x0A && mode == MBCPocketCameraModeRAM && eram_size)
         return &eram_[(address & 0x1FFF) | (ram_bank_number << 13)];


       return 0;
     }
     return 0;
   }
   uint8_t Read(uint16_t address) {
     if (address >= 0x0000 && address <= 0x3FFF) {
       return rom_[address];
     }
     else if (address >= 0x4000 && address <= 0x7FFF) {
       return rom_[(address & 0x3FFF) | ((rom_bank_number) << 14)];
     }
     else if (address >= 0xA000 && address <= 0xBFFF) {

       //(eram_enable & 0x0A) == 0x0A && 
       if (mode == MBCPocketCameraModeRAM && eram_size) {
         return eram_[(address & 0x1FFF) | (ram_bank_number << 13)];
       }

       if (mode == MBCPocketCameraModeCamera) {
         //char ds[25];
         //sprintf(ds, "read camera %x = %x\n", address, camera_io[address & 0x7F]);
         //OutputDebugString(ds);
         return camera_io[address & 0x7F];

       }


       //if ((rtc_enable & 0x0A) == 0x0A && mode == 1)
      //   return rtc[rtc_select];

       return 0;
     }
     return 0;
   }
   void Write(uint16_t address, uint8_t data) {
     if (address >= 0x0000 && address <= 0x1FFF) {
       eram_enable = data;
      
     }
     else if (address >= 0x2000 && address <= 0x3FFF) {
       rom_bank_number = data & 0x3F;
     }
     else if (address >= 0x4000 && address <= 0x5FFF) {
       if (data >= 0x00 && data <= 0x0F) {
         mode = MBCPocketCameraModeRAM;
         ram_bank_number = data;
       }
       else if (data >> 4 == 1) {
         mode = MBCPocketCameraModeCamera;
         //camera io
         
       }
     }
     else if (address >= 0x6000 && address <= 0x7FFF) {
     

     }
     else if (address >= 0xA000 && address <= 0xBFFF) {
       
       if ((eram_enable & 0x0A) == 0x0A && mode == MBCPocketCameraModeRAM && eram_size) {
         eram_[(address & 0x1FFF) | (ram_bank_number << 13)] = data;
       }

       if (mode == MBCPocketCameraModeCamera) {
         ///char ds[25];
         //sprintf(ds, "write camera %x = %x\n", address, data);
         //OutputDebugString(ds);

         if (address == 0xA000)
         {
           camera_io[address & 0x7F] = data&0x03;
           if (camera_io[0] & 1)
             start_capture = true;
         }
         else
         {
           camera_io[address & 0x7F] = data;
         }
       }


      
     }
   }

   void Step(double dt) {
     /*
     N_bit = ([A001] & BIT(7)) ? 0 : 512
exposure = ([A002]<<8) | [A003]
CYCLES = 32446 + N_bit + 16 * exposure
     */

     auto n_bit = camera_io[1] & 0x80 ? 0 : 512;
     auto exposure = (camera_io[2] << 8) | camera_io[3];
     auto CYCLES = 32446 + n_bit + 16 * exposure;

     auto time_needed = ( cartridge->emu()->base_freq_hz() / 1048576.0 ) * 1000.0;

     if (start_capture = true)  {

       camera_io[0] &= ~0x01;

       start_capture = false;
    }
   }

   uint8_t mode;
   bool start_capture = false;
   uint8_t camera_io[0x80];
};

}
}