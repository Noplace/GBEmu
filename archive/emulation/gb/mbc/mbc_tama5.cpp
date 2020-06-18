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

//based on gest source code

namespace emulation {
namespace gb {

struct rtc_clock
{
   int s; // seconds counter
   int m; // minutes counter
   int h; // hour counter
   int d; // day counter
   int control; // carry bit, halt flag
   int cur_register;
   time_t last_time;
};

class MBCTAMA5 : public MemoryBankController {
 public:
  void Initialize(Cartridge* cartridge) {
    MemoryBankController::Initialize(cartridge);
    eram_ = new uint8_t[0x2000];
    rom_bank_number = 1;
    ram_bank_number = 0;
    tama_flag = mbc_low = mbc_high = 0;

    tama_val4 = tama_val5 = tama_val6 = tama_val7 = 0;
    tama_count = 0;
    tama_time = 0;
    tama_month = 0;
    tama_change_clock = 0;
    memset(&rtc,0,sizeof(rtc));

    battery_ = true;
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
      return &(cartridge->rom()[address]);
    } else if (address >= 0x4000 && address <= 0x7FFF) {
      return &(cartridge->rom()[(address&0x3FFF)+(rom_bank_number<<14)]);
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        return &eram_[(address&0x1FFF)];
    }
    return nullptr;
  }

void update_tama_RTC()
{
   time_t now = time(0);
   time_t diff = now-rtc.last_time;
   if(diff > 0)
   {
    rtc.s += diff % 60;
    if(rtc.s > 59) 
    {
      rtc.s -= 60;
      rtc.m++;
    }

    diff /= 60;

    rtc.m += diff % 60;
    if(rtc.m > 59) 
    {
      rtc.m -= 60;
      rtc.h++;
    }

    diff /= 60;

    rtc.h += diff % 24;
    if(rtc.h > 24) 
    {
      rtc.h -= 24;
      rtc.d++;
    }
    diff /= 24;

    rtc.d += (int)diff;
    if(rtc.d > 31) 
    {
      rtc.d -= 31;
      tama_month++;
      if(tama_month > 12)
         tama_month -= 12;
    }
  }
  rtc.last_time = now;
}

  uint8_t Read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x3FFF) {
      return cartridge->rom()[address];
    } else if (address >= 0x4000 && address <= 0x7FFF) {
      return cartridge->rom()[(address&0x3FFF)+(rom_bank_number<<14)];
    } else if (address >= 0xA000 && address <= 0xBFFF) {
      
      if (address == 0xA000) {
        switch (tama_flag) {
        case 0x0A:
          return 1;
        case 0x0D:{

          uint8_t read = 0;
            if(tama_val4 == 1)
            {           
               if(tama_val6 == 0x04)
               {
                  if(tama_val7 == 0x06) // get minutes higher nibble
                  {
                     read = rtc.m;
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;   
                     if(read > 0x4f)
                        read += 6; 
                     if(read == 0x5A)
                        read = 0;                                                             
                     read = (read&0xf0)>>4;
                  }
                  else if(tama_val7 == 0x07) { // get hours higher nibble
                     read = rtc.h;
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;   
                                                        
                     read = (read&0xf0)>>4;                  
                  }
               } 
            } else
            { // read memory ?
               read = eram_[((tama_val6<<4)|tama_val7)];
            } 
            return read;
          break;
        }
        case 0x0C: // RTC registers
         {
            update_tama_RTC();
            byte read = 0;
            switch(tama_val4)
            {
            case 0:
               if(rtc.s == 0)
                  read = 0;
               else
                  read = rtc.s - (tama_time+1); // return 0 if second has passed
               tama_time = rtc.s;
            break;
            case 1:   
               if(tama_val6 == 0x04)
               {              
                  if(tama_val7 == 0x06) // get minutes lower nibble 
                  {
                     read = rtc.m;
                     
                     //change into correct format
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;     
                     if(read > 0x4f)
                        read += 6;
                     if(read == 0x5A)
                        read = 0;
                                                                  
                     read &= 0x0f;
                  }
                  else 
                  if(tama_val7 == 0x07) // get hours lower nibble
                  {
                     read = rtc.h;
                     
                     //change into correct format
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;     
                        
                     read &= 0x0f;                                          
                  }
               } 
            break;
            case 7: // day low
               read = rtc.d&0xF;
            break;
            case 8: // day high
               read = ((rtc.d&0xf0)>>4);
            break;
            case 9: // month
               read = tama_month;
            break;
            case 0xA: // ? (year maybe?)
               read = 0;            
            break;
            case 0xF: // ? (year maybe?)
               read = 0;            
            break;
            }
            return read;
         }

        }
      }
      
      
      
    }
    return 0;
  }
  void Write(uint16_t address, uint8_t data) {

    if (address < 0x8000)
      return;

    if (address >= 0xA000 && address <= 0xBFFF) {
      if (address == 0xA000) {
        switch (tama_flag) {
          case 0:
            mbc_low = data&0x0F;
            rom_bank_number = mbc_low|(mbc_high<<4);
       
            break;
          case 1:
            mbc_high = data & 0x01;
            rom_bank_number = mbc_low|(mbc_high<<4);
       
            break;

          case 4:
            tama_val4 = data;
            break;
          case 5:
            tama_val5 = data;
            ++tama_count;
            if(tama_count==1 && data == 0) tama_change_clock |= 2;
            if(tama_count==2 && data == 1) tama_change_clock |= 1;
            if(tama_change_clock == 3) rtc.last_time = time(0);

            eram_[(tama_val6<<4)+tama_val7] = tama_val4|(data<<4);
            
            //which time counter is changed?
            if(tama_count==6 && tama_change_clock==3)
            {
               tama_month = data;
            } else
            if(tama_count==7 && tama_change_clock==3)
            {
               tama_month += data*10;
            } else
            if(tama_count==8 && tama_change_clock==3)
            {
               rtc.d = data;
            } else
            if(tama_count==9 && tama_change_clock==3)
            {
               rtc.d += data*10;
            } else
            if(tama_count==10 && tama_change_clock==3)
            {
               rtc.m = data*10;
            } else
            if(tama_count==11 && tama_change_clock==3)
            {
               rtc.h = data*10;
            } else
            if(tama_change_clock==3 && tama_count >= 13)
            {
               tama_count = 0;
               tama_change_clock = 0;
            } else
            if(tama_change_clock!=3 && tama_count >= 2)
            {
               tama_count=0;
               tama_change_clock = 0;
            }
            break;
          case 6:
            tama_val6 = data;
            break;
          case 7:
            tama_val7 = data;
            break;
        }

      } else if (address == 0xA001) {
        tama_flag = data;
      } else {
          eram_[(address&0x1FFF)] = data;
      }
    }




  }
  uint8_t tama_flag,tama_time,mbc_low,mbc_high,tama_val4,tama_val5,tama_val6,tama_val7;
   int tama_count;
   int tama_month;
   int tama_change_clock;
   rtc_clock rtc;
};


}
}