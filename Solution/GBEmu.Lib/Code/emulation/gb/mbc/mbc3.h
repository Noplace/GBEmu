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
        enum MBC3Mode { MBC3ModeRAM = 0, MBC3ModeRTC = 1 };
        class MBC3 : public MemoryBankController {
        public:
            void Initialize(Cartridge* cartridge) {
                MemoryBankController::Initialize(cartridge);
                rom_bank_number = 1;
                ram_bank_number = 0;
                mode = MBC3ModeRAM;

                ram_rtc_enable = false;

                rtc_select = 0;

                timecounter = 0;
                rtc_timer = 0;
                memset(rtc, 0, sizeof(rtc));
                switch (cartridge->header->cartridge_type) {
                case 0x0F:battery_ = true; break;
                case 0x10:battery_ = true; break;
                case 0x13:battery_ = true; break;
                }
                if (battery_)
                    cartridge->LoadRam();

                latch_seq = 0;
                time_day = 0;
                time_hour = 0;
                time_minute = 0;
                    time_second = 0;
                    time_control = 0;
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
                    return &rom_[(address & 0x3FFF) | ((rom_bank_number % max_rom_banks) << 14)];
                }
                else if (address >= 0xA000 && address <= 0xBFFF) {


                    if (ram_rtc_enable == true && mode == MBC3ModeRAM && eram_size)
                        return &eram_[(address & 0x1FFF) | (ram_bank_number << 13)];

                    if (ram_rtc_enable == true && mode == MBC3ModeRTC)
                        return &rtc[rtc_select];

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

                    if (ram_rtc_enable == true && mode == MBC3ModeRAM && eram_size) {
                        
                        //sprintf_s(logstr, "MBC 3 read @ $%04x = $%02x (RAM)\n", address, eram_[(address & 0x1FFF) | (ram_bank_number << 13)]);
                        //cartridge->emu()->log_output(logstr);

                        return eram_[(address & 0x1FFF) | (ram_bank_number << 13)];
                    } else  if (ram_rtc_enable == true && mode == MBC3ModeRTC) {
                        //sprintf_s(logstr, "MBC 3 read @ %d = $%02x (RTC)\n", rtc_select, rtc[rtc_select]);
                        //cartridge->emu()->log_output(logstr);
                        //return rtc[rtc_select];
                      
                        switch (rtc_select) {
                            case 0: return rtc[rtc_select] & 0x3F; break;
                            case 1: return rtc[rtc_select] & 0x3F; break;
                            case 2: return rtc[rtc_select] & 0x1F; break;
                            case 3: return rtc[rtc_select] & 0xFF; break;
                            case 4: return rtc[rtc_select] & 0xC1; break;
                        }

                    }

                    sprintf_s(logstr, "MBC 3 read @  $%04x (Unknown)\n", address);
                    cartridge->emu()->log_output(logstr);
                    return 0xFF;
                }
                sprintf_s(logstr, "MBC 3 unknown read @ %04x\n", address);
                cartridge->emu()->log_output(logstr);
                return 0;
            }

            void Write(uint16_t address, uint8_t data) {

                if (address >= 0x0000 && address <= 0x1FFF) {

                    if ((data & 0x0F) == 0x0A) {
                        ram_rtc_enable = true;
                    }
                    else {
                        ram_rtc_enable = false;
                    }

                    //sprintf_s(logstr, "MBC 3 0x0000-0x1FFF Write: 0x%x\n", data);
                   // cartridge->emu()->log_output(logstr);

                }
                else if (address >= 0x2000 && address <= 0x3FFF) {
                    rom_bank_number = data & 0x7F;
                    if (rom_bank_number == 0)
                        rom_bank_number = 1;
                    rom_bank_number = rom_bank_number & (max_rom_banks - 1);
                    //sprintf_s(logstr, "MBC 3 rom_bank_number  0x%x\n", rom_bank_number);
                    //cartridge->emu()->log_output(logstr);
                }
                else if (address >= 0x4000 && address <= 0x5FFF) {

                    if (data >= 0 && data <= 3) {
                        mode = MBC3ModeRAM;
                        // if ((data & 0x3) < max_ram_banks) {
                        ram_bank_number = data;
                        ram_bank_number = ram_bank_number & (max_ram_banks - 1);
                        //}



                        //sprintf_s(logstr, "MBC 3 0x4000-0x5FFF Write: MBC3ModeRAM ram bank 0x%x\n", ram_bank_number);
                        //cartridge->emu()->log_output(logstr);
                    }
                    else if (data >= 0x08 && data <= 0x0C) {
                        mode = MBC3ModeRTC;
                        rtc_select = data - 0x08;

                        //sprintf_s(logstr, "MBC 3 0x4000-0x5FFF Write: MBC3ModeRTC rtc_select 0x%x\n", rtc_select);
                        //cartridge->emu()->log_output(logstr);
                    }
                    else {
                        sprintf_s(logstr, "MBC 3 0x4000-0x5FFF Write: Unknown 0x%x\n", data);
                        cartridge->emu()->log_output(logstr);
                    }



                }
                else if (address >= 0x6000 && address <= 0x7FFF) {
                    if (data == 0x00 && latch_seq == 0) {
                        latch_seq = 1;
                    }
                    else if (data == 0x01 && latch_seq == 1) {
                        
                        //sprintf_s(logstr, "MBC 3 0x6000-0x7FFF Latch Write: $%02x\n", data);
                        //cartridge->emu()->log_output(logstr);

                        //auto n = rtc_timer;
                       // auto day = n / (24 * 3600);
                        //n = n % (24 * 3600);
                        //auto hour = n / 3600;
                        //n %= 3600;
                        //auto minutes = n / 60;
                        //n %= 60;
                        //auto seconds = n;


                        //if (ram_rtc_enable == true) {
                        rtc[0] = (uint8_t)time_second;//rtc_timer % 60;
                        rtc[1] = (uint8_t)time_minute;//(rtc_timer/ 60) % 60;
                        rtc[2] = (uint8_t)time_hour;//(rtc_timer / 60 / 60) % 24;

                       // auto days = time_day;//rtc_timer / 60 / 60 / 24;

                        rtc[3] = (uint8_t)(time_day & 0xFF);
                        
                       // rtc[4] &= 0xC0; //~0x01;
                        rtc[4] = time_control;
                        rtc[4] |= (time_day >> 8) & 0x1;
                       //// if (!(rtc[4]&0x80)) //carry does not reset once set
                        //    rtc[4] |= (days >> 2) & 0x80;


                        //}





                        latch_seq = 0;
                    }


                }
                else if (address >= 0xA000 && address <= 0xBFFF) {
                    if (ram_rtc_enable == true && mode == MBC3ModeRAM && eram_size) {
                        eram_[(address & 0x1FFF) | (ram_bank_number << 13)] = data;
                        //sprintf_s(logstr, "MBC 3 write @ $%04x = $%02x (RAM)\n", address, data);
                        //cartridge->emu()->log_output(logstr);
                    }
                    else if (ram_rtc_enable == true && mode == MBC3ModeRTC) {
                        //sprintf_s(logstr, "MBC 3 write @ %d = $%02x (RTC)\n", rtc_select, data);
                        //cartridge->emu()->log_output(logstr);
                        //rtc[rtc_select] = data;
                        /*switch (rtc_select) {
                            case 0: rtc[rtc_select] = data & 0x3F; break;
                            case 1: rtc[rtc_select] = data & 0x3F; break;
                            case 2: rtc[rtc_select] = data & 0x1F; break;
                            case 3: rtc[rtc_select] = data & 0xFF; break;
                            case 4: rtc[rtc_select] = data & 0xC1; break;
                        }*/
                        
                        switch (rtc_select) {
                        case 0: time_second = rtc[0] = data & 0x3F; timecounter = 0; break;
                        case 1: time_minute = rtc[1]= data & 0x3F; break;
                        case 2: time_hour = rtc[2]=data & 0x1F; break;
                        case 3: time_day = rtc[3] = data & 0xFF; break;
                        case 4: time_control = rtc[4] = data & 0xC1;  break;
                        }

                        //normal way, commented for testing
                        //reset counter properly
                        //auto rtc_value = rtc[0] + rtc[1] * 60 + rtc[2] * 3600 + (((rtc[4] & 0x80) << 2) | ((rtc[4]&0x1)<<8)|rtc[3]) * 24 * 3600;
                        //set_rtc_time(rtc_value);
                        //timecounter = 0;//affects subtiming rom test
                        
                        





               
                    }
                    else {
                        sprintf_s(logstr, "MBC 3 write @ $%04x = $%02x (Unknown)\n", address, data);
                        cartridge->emu()->log_output(logstr);
                    }

                    
                }
            }

            void Step(double dt) {
              if ((time_control & 0x40) == 0x40) return;//halt
                timecounter += dt;
                if (timecounter >= 1000.0) { // 1sec ////32768Hz, for original gameboy
                    timecounter -= 1000.0;


                   
                    if (++time_second == 60) { //sec
                        if (++time_minute == 60) { //min
                            if (++time_hour == 24) { //hour
                                if (++time_day == 0x100) { //day
                                  
                                    if (time_control & 1)
                                    {
                                        time_control |= 0x80;//overflow
                                        time_control = time_control & ~0x01;
                                    }
                                    else
                                    {
                                        time_control |= 1;
                                    }

                                   
                                    
                                    time_day = 0;
                                }
                                time_hour = 0;
                            }
                            time_minute = 0;
                        }
                        time_second = 0;
                    }


                    /* normal way
                    if ((rtc[4] & 0x40) == 0x40) return;//halt
                    //++rtc_timer;
                    set_rtc_time(rtc_timer + 1);
           */

                    /* if (++rtc[0] == 60) { //sec
                       if (++rtc[1] == 60) { //min
                         if (++rtc[2] == 24) { //hour
                           if (++rtc[3] == 0xFF) { //day
                             if (rtc[4]&1)
                               rtc[4] |= 0x80;
                             else
                               rtc[4] |= 1;
                             rtc[3] = 0;
                           }
                           rtc[2] = 0;
                         }
                         rtc[1] = 0;
                       }
                       rtc[0] = 0;
                     }*/
                    
                }
            }

            void set_rtc_time(uint64_t value) {
                rtc_timer = value;
                auto n = rtc_timer;
                time_day = n / (24 * 3600);
                n = n % (24 * 3600);
                time_hour = n / 3600;
                n %= 3600;
                time_minute = n / 60;
                n %= 60;
                time_second = n;

                //sprintf_s(logstr, "rtc_timer new value = %d time = %d,%d,%d,%d\n", rtc_timer, time_day, time_hour, time_minute, time_second);
                //cartridge->emu()->log_output(logstr);
            }

            bool ram_rtc_enable;
            uint8_t mode;
            uint8_t rtc[5];
            uint64_t rtc_timer;
            uint64_t time_day, time_hour, time_minute, time_second, time_control;
            uint8_t rtc_select;
            uint8_t latch_seq;
            double timecounter;
        };

    }
}