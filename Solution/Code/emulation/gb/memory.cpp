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

/*
General Memory Map
  0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
  4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
  8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
  A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
  FE00-FE9F   Sprite Attribute Table (OAM)
  FEA0-FEFF   Not Usable
  FF00-FF7F   I/O Ports
  FF80-FFFE   High RAM (HRAM)
  FFFF        Interrupt Enable Register
  */

#include "gb.h"


namespace emulation {
namespace gb {
  
const uint8_t dmgrom[0x100] = {
0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e, 
0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3, 0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 
0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b, 
0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9, 
0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99, 0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 
0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04, 
0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2, 
0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62, 0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 
0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20, 
0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17, 
0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9, 0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 
0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 
0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 
0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c, 
0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20, 
0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};

const uint8_t gbcrom[0x900] = {
0x31,0xFE,0xFF,0x3E,0x02,0xC3,0x7C,0x00,0xD3,0x00,0x98,0xA0,0x12,0xD3,0x00,0x80,0x00,0x40,0x1E,0x53,0xD0,0x00,0x1F,0x42,0x1C,0x00,0x14,0x2A,0x4D,0x19,0x8C,0x7E,0x00,0x7C,0x31,0x6E,0x4A,
0x45,0x52,0x4A,0x00,0x00,0xFF,0x53,0x1F,0x7C,0xFF,0x03,0x1F,0x00,0xFF,0x1F,0xA7,0x00,0xEF,0x1B,0x1F,0x00,0xEF,0x1B,0x00,0x7C,0x00,0x00,0xFF,0x03,0xCE,0xED,0x66,0x66,0xCC,0x0D,0x00,0x0B,
0x03,0x73,0x00,0x83,0x00,0x0C,0x00,0x0D,0x00,0x08,0x11,0x1F,0x88,0x89,0x00,0x0E,0xDC,0xCC,0x6E,0xE6,0xDD,0xDD,0xD9,0x99,0xBB,0xBB,0x67,0x63,0x6E,0x0E,0xEC,0xCC,0xDD,0xDC,0x99,0x9F,0xBB,
0xB9,0x33,0x3E,0x3C,0x42,0xB9,0xA5,0xB9,0xA5,0x42,0x3C,0x58,0x43,0xE0,0x70,0x3E,0xFC,0xE0,0x47,0xCD,0x75,0x02,0xCD,0x00,0x02,0x26,0xD0,0xCD,0x03,0x02,0x21,0x00,0xFE,0x0E,0xA0,0xAF,0x22,
0x0D,0x20,0xFC,0x11,0x04,0x01,0x21,0x10,0x80,0x4C,0x1A,0xE2,0x0C,0xCD,0xC6,0x03,0xCD,0xC7,0x03,0x13,0x7B,0xFE,0x34,0x20,0xF1,0x11,0x72,0x00,0x06,0x08,0x1A,0x13,0x22,0x23,0x05,0x20,0xF9,
0xCD,0xF0,0x03,0x3E,0x01,0xE0,0x4F,0x3E,0x91,0xE0,0x40,0x21,0xB2,0x98,0x06,0x4E,0x0E,0x44,0xCD,0x91,0x02,0xAF,0xE0,0x4F,0x0E,0x80,0x21,0x42,0x00,0x06,0x18,0xF2,0x0C,0xBE,0x20,0xFE,0x23,
0x05,0x20,0xF7,0x21,0x34,0x01,0x06,0x19,0x78,0x86,0x2C,0x05,0x20,0xFB,0x86,0x20,0xFE,0xCD,0x1C,0x03,0x18,0x02,0x00,0x00,0xCD,0xD0,0x05,0xAF,0xE0,0x70,0x3E,0x11,0xE0,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x00,0x80,0xAF,0x22,0xCB,0x6C,0x28,0xFB,0xC9,0x2A,0x12,0x13,0x0D,0x20,0xFA,0xC9,0xE5,0x21,0x0F,0xFF,0xCB,0x86,0xCB,0x46,0x28,0xFC,0xE1,0xC9,0x11,0x00,0xFF,0x21,0x03,0xD0,0x0E,0x0F,0x3E,0x30,0x12,0x3E,0x20,0x12,0x1A,0x2F,0xA1,0xCB,0x37,0x47,0x3E,0x10,0x12,0x1A,0x2F,0xA1,0xB0,0x4F,0x7E,0xA9,0xE6,0xF0,0x47,0x2A,0xA9,0xA1,0xB0,0x32,0x47,0x79,0x77,0x3E,0x30,0x12,0xC9,0x3E,0x80,0xE0,0x68,0xE0,0x6A,0x0E,0x6B,0x2A,0xE2,0x05,0x20,0xFB,0x4A,0x09,0x43,0x0E,0x69,0x2A,
0xE2,0x05,0x20,0xFB,0xC9,0xC5,0xD5,0xE5,0x21,0x00,0xD8,0x06,0x01,0x16,0x3F,0x1E,0x40,0xCD,0x4A,0x02,0xE1,0xD1,0xC1,0xC9,0x3E,0x80,0xE0,0x26,0xE0,0x11,0x3E,0xF3,0xE0,0x12,0xE0,0x25,0x3E,0x77,0xE0,0x24,0x21,0x30,0xFF,0xAF,0x0E,0x10,0x22,0x2F,0x0D,0x20,0xFB,0xC9,0xCD,0x11,0x02,0xCD,0x62,0x02,0x79,0xFE,0x38,0x20,0x14,0xE5,0xAF,0xE0,0x4F,0x21,0xA7,0x99,0x3E,0x38,0x22,0x3C,0xFE,0x3F,0x20,0xFA,0x3E,0x01,0xE0,0x4F,0xE1,0xC5,0xE5,0x21,0x43,0x01,0xCB,0x7E,0xCC,0x89,0x05,0xE1,0xC1,0xCD,0x11,0x02,0x79,0xD6,0x30,0xD2,0x06,0x03,0x79,0xFE,0x01,0xCA,0x06,0x03,0x7D,0xFE,0xD1,0x28,0x21,0xC5,0x06,0x03,0x0E,0x01,0x16,0x03,0x7E,0xE6,0xF8,0xB1,0x22,0x15,0x20,0xF8,0x0C,
0x79,0xFE,0x06,0x20,0xF0,0x11,0x11,0x00,0x19,0x05,0x20,0xE7,0x11,0xA1,0xFF,0x19,0xC1,0x04,0x78,0x1E,0x83,0xFE,0x62,0x28,0x06,0x1E,0xC1,0xFE,0x64,0x20,0x07,0x7B,0xE0,0x13,0x3E,0x87,0xE0,0x14,0xFA,0x02,0xD0,0xFE,0x00,0x28,0x0A,0x3D,0xEA,0x02,0xD0,0x79,0xFE,0x01,0xCA,0x91,0x02,0x0D,0xC2,0x91,0x02,0xC9,0x0E,0x26,0xCD,0x4A,0x03,0xCD,0x11,0x02,0xCD,0x62,0x02,0x0D,0x20,0xF4,0xCD,0x11,0x02,0x3E,0x01,0xE0,0x4F,0xCD,0x3E,0x03,0xCD,0x41,0x03,0xAF,0xE0,0x4F,0xCD,0x3E,0x03,0xC9,0x21,0x08,0x00,0x11,0x51,0xFF,0x0E,0x05,0xCD,0x0A,0x02,0xC9,0xC5,0xD5,0xE5,0x21,0x40,0xD8,0x0E,0x20,0x7E,0xE6,0x1F,0xFE,0x1F,0x28,0x01,0x3C,0x57,0x2A,0x07,0x07,0x07,0xE6,0x07,0x47,0x3A,
0x07,0x07,0x07,0xE6,0x18,0xB0,0xFE,0x1F,0x28,0x01,0x3C,0x0F,0x0F,0x0F,0x47,0xE6,0xE0,0xB2,0x22,0x78,0xE6,0x03,0x5F,0x7E,0x0F,0x0F,0xE6,0x1F,0xFE,0x1F,0x28,0x01,0x3C,0x07,0x07,0xB3,0x22,0x0D,0x20,0xC7,0xE1,0xD1,0xC1,0xC9,0x0E,0x00,0x1A,0xE6,0xF0,0xCB,0x49,0x28,0x02,0xCB,0x37,0x47,0x23,0x7E,0xB0,0x22,0x1A,0xE6,0x0F,0xCB,0x49,0x20,0x02,0xCB,0x37,0x47,0x23,0x7E,0xB0,0x22,0x13,0xCB,0x41,0x28,0x0D,0xD5,0x11,0xF8,0xFF,0xCB,0x49,0x28,0x03,0x11,0x08,0x00,0x19,0xD1,0x0C,0x79,0xFE,0x18,0x20,0xCC,0xC9,0x47,0xD5,0x16,0x04,
0x58,0xCB,0x10,0x17,0xCB,0x13,0x17,0x15,0x20,0xF6,0xD1,0x22,0x23,0x22,0x23,0xC9,0x3E,0x19,0xEA,0x10,0x99,0x21,0x2F,0x99,0x0E,0x0C,0x3D,0x28,0x08,0x32,0x0D,0x20,0xF9,0x2E,0x0F,0x18,0xF3,0xC9,0x3E,0x01,0xE0,0x4F,0xCD,0x00,0x02,0x11,0x07,0x06,0x21,0x80,0x80,0x0E,0xC0,0x1A,0x22,0x23,0x22,0x23,0x13,0x0D,0x20,0xF7,0x11,0x04,0x01,0xCD,0x8F,0x03,0x01,0xA8,0xFF,0x09,0xCD,0x8F,0x03,0x01,0xF8,0xFF,0x09,0x11,0x72,0x00,0x0E,0x08,0x23,0x1A,0x22,0x13,0x0D,0x20,0xF9,0x21,0xC2,0x98,0x06,0x08,0x3E,0x08,0x0E,0x10,0x22,0x0D,0x20,0xFC,0x11,0x10,0x00,0x19,0x05,0x20,0xF3,0xAF,0xE0,0x4F,0x21,0xC2,0x98,0x3E,0x08,0x22,0x3C,0xFE,0x18,0x20,0x02,0x2E,0xE2,0xFE,0x28,0x20,0x03,0x21,0x02,0x99,0xFE,0x38,0x20,0xED,0x21,0xD8,0x08,0x11,0x40,0xD8,0x06,0x08,0x3E,0xFF,0x12,0x13,0x12,0x13,0x0E,0x02,0xCD,0x0A,0x02,0x3E,0x00,0x12,0x13,0x12,0x13,0x13,0x13,0x05,0x20,0xEA,0xCD,0x62,0x02,0x21,0x4B,0x01,0x7E,0xFE,0x33,0x20,0x0B,0x2E,0x44,0x1E,0x30,0x2A,0xBB,0x20,0x49,0x1C,0x18,0x04,0x2E,0x4B,0x1E,0x01,0x2A,0xBB,0x20,0x3E,0x2E,
0x34,0x01,0x10,0x00,0x2A,0x80,0x47,0x0D,0x20,0xFA,0xEA,0x00,0xD0,0x21,0xC7,0x06,0x0E,0x00,0x2A,0xB8,0x28,0x08,0x0C,0x79,0xFE,0x4F,0x20,0xF6,0x18,0x1F,0x79,0xD6,0x41,0x38,0x1C,0x21,0x16,0x07,0x16,0x00,0x5F,0x19,0xFA,0x37,0x01,0x57,0x7E,0xBA,0x28,0x0D,0x11,0x0E,0x00,0x19,0x79,0x83,0x4F,0xD6,
0x5E,0x38,0xED,0x0E,0x00,0x21,0x33,0x07,0x06,0x00,0x09,0x7E,0xE6,0x1F,0xEA,0x08,0xD0,0x7E,0xE6,0xE0,0x07,0x07,0x07,0xEA,0x0B,0xD0,0xCD,0xE9,0x04,0xC9,0x11,0x91,0x07,0x21,0x00,0xD9,0xFA,0x0B,0xD0,0x47,0x0E,0x1E,0xCB,0x40,0x20,0x02,0x13,0x13,0x1A,0x22,0x20,0x02,0x1B,0x1B,0xCB,0x48,0x20,0x02,0x13,0x13,0x1A,0x22,0x13,0x13,0x20,0x02,0x1B,0x1B,0xCB,0x50,0x28,0x05,0x1B,0x2B,0x1A,0x22,0x13,0x1A,0x22,0x13,0x0D,0x20,0xD7,0x21,0x00,0xD9,0x11,0x00,0xDA,0xCD,0x64,0x05,0xC9,0x21,0x12,0x00,0xFA,0x05,0xD0,0x07,0x07,0x06,0x00,0x4F,0x09,0x11,0x40,0xD8,0x06,0x08,0xE5,0x0E,0x02,0xCD,0x0A,0x02,0x13,0x13,0x13,0x13,0x13,0x13,0xE1,0x05,0x20,0xF0,0x11,0x42,0xD8,0x0E,0x02,0xCD,0x0A,0x02,0x11,0x4A,0xD8,0x0E,0x02,0xCD,0x0A,0x02,0x2B,0x2B,0x11,0x44,0xD8,0x0E,0x02,0xCD,0x0A,0x02,0xC9,0x0E,0x60,0x2A,0xE5,0xC5,0x21,0xE8,0x07,0x06,0x00,0x4F,0x09,0x0E,0x08,0xCD,0x0A,0x02,0xC1,0xE1,0x0D,0x20,0xEC,0xC9,0xFA,0x08,0xD0,0x11,0x18,0x00,0x3C,0x3D,0x28,0x03,0x19,0x20,0xFA,0xC9,0xCD,0x1D,0x02,0x78,0xE6,0xFF,0x28,0x0F,0x21,
0xE4,0x08,0x06,0x00,0x2A,0xB9,0x28,0x08,0x04,0x78,0xFE,0x0C,0x20,0xF6,0x18,0x2D,0x78,0xEA,0x05,0xD0,0x3E,0x1E,0xEA,0x02,0xD0,0x11,0x0B,0x00,0x19,0x56,0x7A,0xE6,0x1F,0x5F,0x21,0x08,0xD0,0x3A,0x22,0x7B,0x77,0x7A,0xE6,0xE0,0x07,0x07,0x07,0x5F,0x21,0x0B,0xD0,0x3A,0x22,0x7B,0x77,0xCD,0xE9,0x04,0xCD,0x28,0x05,0xC9,0xCD,0x11,0x02,0xFA,0x43,0x01,0xCB,0x7F,0x28,0x04,0xE0,0x4C,0x18,0x28,0x3E,0x04,0xE0,0x4C,0x3E,0x01,0xE0,0x6C,0x21,0x00,0xDA,0xCD,0x7B,0x05,0x06,0x10,0x16,0x00,0x1E,0x08,0xCD,0x4A,0x02,0x21,0x7A,0x00,0xFA,0x00,0xD0,0x47,0x0E,0x02,0x2A,0xB8,0xCC,0xDA,0x03,0x0D,0x20,0xF8,0xC9,0x01,0x0F,0x3F,0x7E,0xFF,0xFF,0xC0,0x00,0xC0,0xF0,0xF1,0x03,0x7C,0xFC,0xFE,0xFE,0x03,0x07,0x07,0x0F,0xE0,0xE0,0xF0,0xF0,0x1E,0x3E,0x7E,0xFE,0x0F,0x0F,0x1F,0x1F,0xFF,0xFF,0x00,0x00,0x01,0x01,0x01,
0x03,0xFF,0xFF,0xE1,0xE0,0xC0,0xF0,0xF9,0xFB,0x1F,0x7F,0xF8,0xE0,0xF3,0xFD,0x3E,0x1E,0xE0,0xF0,0xF9,0x7F,0x3E,0x7C,0xF8,0xE0,0xF8,0xF0,0xF0,0xF8,0x00,0x00,0x7F,0x7F,0x07,0x0F,0x9F,0xBF,0x9E,0x1F,0xFF,0xFF,0x0F,0x1E,0x3E,0x3C,0xF1,0xFB,0x7F,0x7F,0xFE,0xDE,0xDF,0x9F,0x1F,0x3F,0x3E,0x3C,0xF8,0xF8,0x00,0x00,0x03,0x03,0x07,0x07,0xFF,0xFF,0xC1,0xC0,0xF3,0xE7,0xF7,0xF3,0xC0,0xC0,0xC0,0xC0,0x1F,0x1F,0x1E,0x3E,0x3F,0x1F,0x3E,0x3E,0x80,0x00,0x00,0x00,0x7C,0x1F,0x07,0x00,0x0F,0xFF,0xFE,0x00,0x7C,0xF8,0xF0,0x00,0x1F,0x0F,0x0F,0x00,0x7C,0xF8,0xF8,0x00,0x3F,0x3E,0x1C,0x00,0x0F,0x0F,0x0F,0x00,0x7C,0xFF,0xFF,0x00,0x00,0xF8,0xF8,0x00,0x07,0x0F,0x0F,0x00,0x81,0xFF,0xFF,0x00,0xF3,0xE1,0x80,0x00,0xE0,0xFF,0x7F,0x00,0xFC,0xF0,0xC0,0x00,
0x3E,0x7C,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x88,0x16,0x36,0xD1,0xDB,0xF2,0x3C,0x8C,0x92,0x3D,0x5C,0x58,0xC9,0x3E,0x70,0x1D,0x59,0x69,0x19,0x35,0xA8,0x14,0xAA,0x75,0x95,0x99,0x34,0x6F,
0x15,0xFF,0x97,0x4B,0x90,0x17,0x10,0x39,0xF7,0xF6,0xA2,0x49,0x4E,0x43,0x68,0xE0,0x8B,0xF0,0xCE,0x0C,0x29,0xE8,0xB7,0x86,0x9A,0x52,0x01,0x9D,0x71,0x9C,0xBD,0x5D,0x6D,0x67,0x3F,0x6B,0xB3,0x46,0x28,0xA5,0xC6,0xD3,0x27,0x61,0x18,0x66,0x6A,0xBF,0x0D,0xF4,0x42,0x45,0x46,0x41,0x41,0x52,0x42,0x45,0x4B,0x45,0x4B,0x20,0x52,0x2D,0x55,
0x52,0x41,0x52,0x20,0x49,0x4E,0x41,0x49,0x4C,0x49,0x43,0x45,0x20,0x52,0x7C,0x08,0x12,0xA3,0xA2,0x07,0x87,0x4B,0x20,0x12,0x65,0xA8,0x16,0xA9,0x86,0xB1,0x68,0xA0,0x87,0x66,0x12,0xA1,0x30,0x3C,0x12,0x85,0x12,0x64,0x1B,0x07,0x06,0x6F,0x6E,0x6E,0xAE,0xAF,0x6F,0xB2,0xAF,0xB2,0xA8,0xAB,0x6F,0xAF,0x86,0xAE,0xA2,0xA2,0x12,0xAF,0x13,0x12,0xA1,0x6E,0xAF,0xAF,0xAD,0x06,0x4C,0x6E,0xAF,0xAF,0x12,0x7C,0xAC,0xA8,0x6A,0x6E,0x13,0xA0,0x2D,0xA8,0x2B,0xAC,0x64,0xAC,0x6D,0x87,0xBC,0x60,0xB4,0x13,0x72,0x7C,0xB5,0xAE,0xAE,0x7C,0x7C,0x65,0xA2,0x6C,0x64,0x85,0x80,0xB0,0x40,0x88,0x20,0x68,0xDE,0x00,0x70,0xDE,0x20,0x78,0x20,0x20,0x38,0x20,0xB0,0x90,0x20,0xB0,0xA0,0xE0,0xB0,0xC0,0x98,0xB6,0x48,0x80,0xE0,0x50,0x1E,0x1E,0x58,0x20,0xB8,0xE0,0x88,0xB0,0x10,0x20,0x00,0x10,0x20,0xE0,0x18,0xE0,0x18,0x00,0x18,0xE0,0x20,0xA8,0xE0,0x20,0x18,0xE0,0x00,0x20,0x18,
0xD8,0xC8,0x18,0xE0,0x00,0xE0,0x40,0x28,0x28,0x28,0x18,0xE0,0x60,0x20,0x18,0xE0,0x00,0x00,0x08,0xE0,0x18,0x30,0xD0,0xD0,0xD0,0x20,0xE0,0xE8,0xFF,0x7F,0xBF,0x32,0xD0,0x00,0x00,0x00,0x9F,0x63,0x79,0x42,0xB0,0x15,0xCB,0x04,0xFF,0x7F,0x31,0x6E,0x4A,0x45,0x00,0x00,0xFF,0x7F,0xEF,0x1B,0x00,0x02,0x00,0x00,0xFF,0x7F,0x1F,0x42,0xF2,0x1C,0x00,0x00,0xFF,0x7F,0x94,0x52,0x4A,0x29,0x00,0x00,0xFF,0x7F,0xFF,0x03,0x2F,0x01,0x00,0x00,0xFF,0x7F,0xEF,0x03,0xD6,0x01,0x00,0x00,0xFF,0x7F,0xB5,0x42,0xC8,0x3D,0x00,0x00,0x74,0x7E,0xFF,0x03,0x80,0x01,0x00,0x00,0xFF,0x67,0xAC,0x77,0x13,0x1A,0x6B,0x2D,0xD6,0x7E,0xFF,0x4B,0x75,0x21,0x00,0x00,0xFF,0x53,0x5F,0x4A,0x52,0x7E,0x00,0x00,0xFF,0x4F,0xD2,0x7E,0x4C,0x3A,0xE0,0x1C,0xED,0x03,0xFF,0x7F,0x5F,0x25,0x00,0x00,0x6A,0x03,0x1F,0x02,0xFF,0x03,0xFF,0x7F,0xFF,0x7F,0xDF,0x01,0x12,0x01,0x00,0x00,0x1F,0x23,0x5F,0x03,0xF2,0x00,0x09,0x00,0xFF,0x7F,0xEA,0x03,0x1F,0x01,0x00,0x00,0x9F,0x29,0x1A,0x00,0x0C,0x00,0x00,0x00,0xFF,0x7F,0x7F,0x02,0x1F,0x00,0x00,0x00,0xFF,0x7F,0xE0,
0x03,0x06,0x02,0x20,0x01,0xFF,0x7F,0xEB,0x7E,0x1F,0x00,0x00,0x7C,0xFF,0x7F,0xFF,0x3F,0x00,0x7E,0x1F,0x00,0xFF,0x7F,0xFF,0x03,0x1F,0x00,0x00,0x00,0xFF,0x03,0x1F,0x00,0x0C,0x00,0x00,0x00,0xFF,0x7F,0x3F,0x03,0x93,0x01,0x00,0x00,0x00,0x00,0x00,0x42,0x7F,0x03,0xFF,0x7F,0xFF,0x7F,0x8C,0x7E,0x00,0x7C,0x00,0x00,0xFF,0x7F,0xEF,0x1B,0x80,0x61,0x00,0x00,0xFF,0x7F,0x00,0x7C,0xE0,0x03,0x1F,0x7C,0x1F,0x00,0xFF,0x03,0x40,0x41,0x42,0x20,0x21,0x22,0x80,0x81,0x82,0x10,0x11,0x12,0x12,0xB0,0x79,0xB8,0xAD,0x16,0x17,0x07,0xBA,0x05,0x7C,0x13,0x00,0x00,0x00,0x00
};

/*const uint8_t dmgrom2[0x100] = {
0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e, 0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3, 0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b, 0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9, 0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99, 0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04, 0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2, 0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62, 0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20, 0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17, 0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9, 0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c, 0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20, 0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};*/

void Memory::Initialize(Emu* emu) {
  Component::Initialize(emu);
  vram_ = new uint8_t[0x4000];
  wram_ = new uint8_t[0x8000];
  memset(vram_,0,0x4000);
  wram_select = 1;
  vram_select = 0;
  Reset();
}

void Memory::Deinitialize() {
  SafeDeleteArray(&wram_);
  SafeDeleteArray(&vram_);
}

void Memory::Reset() {
  ZeroMemory(ioports_,sizeof(ioports_));
  memset(joypadflags,0,sizeof(joypadflags));
  ioports_[0] = 0x0F;
  last_address = 0;
   /*ioports_[0x05] = 0x00; // TIMA
   ioports_[0x06] = 0x00; // TMA
   ioports_[0x07] = 0x00; // TAC
   ioports_[0x10] = 0x80; // NR10
   ioports_[0x11] = 0xBF; // NR11
   ioports_[0x12] = 0xF3; // NR12
   ioports_[0x14] = 0xBF; // NR14
   ioports_[0x16] = 0x3F; // NR21
   ioports_[0x17] = 0x00; // NR22
   ioports_[0x19] = 0xBF; // NR24
   ioports_[0x1A] = 0x7F; // NR30
   ioports_[0x1B] = 0xFF; // NR31
   ioports_[0x1C] = 0x9F; // NR32
   ioports_[0x1E] = 0xBF; // NR33
   ioports_[0x20] = 0xFF; // NR41
   ioports_[0x21] = 0x00; // NR42
   ioports_[0x22] = 0x00; // NR43
   ioports_[0x23] = 0xBF; // NR30
   ioports_[0x24] = 0x77; // NR50
   ioports_[0x25] = 0xF3; // NR51
   ioports_[0x26] = 0xF1; //NR52
   ioports_[0x40] = 0x91; // LCDC
   ioports_[0x42] = 0x00; // SCY
   ioports_[0x43] = 0x00; // SCX
   ioports_[0x45] = 0x00; // LYC
   ioports_[0x47] = 0xFC; // BGP
   ioports_[0x48] = 0xFF; // OBP0
   ioports_[0x49] = 0xFF; // OBP1
   ioports_[0x4A] = 0x00; // WY
   ioports_[0x4B] = 0x00; // WX
   ioports_[0xFF] = 0x00; // IE*/
}


uint8_t* Memory::GetMemoryPointer(uint16_t address) {

  if (address >= 0x0000 && address <= 0x3FFF) {
    if (ioports_[0x50] == 0 && address < 0x100) { 
      return nullptr;//&dmgrom[address&0xFF];
    } else {
      return emu_->cartridge()->GetMemoryPointer(address);
    }
  } else if (address >= 0x4000 && address <= 0x7FFF) {
    return emu_->cartridge()->GetMemoryPointer(address);
  } else if (address >= 0x8000 && address <= 0x9FFF) {
    return &vram_[(address&0x1FFF)|(vram_select<<13)];
  } else if (address >= 0xA000 && address <= 0xBFFF) {
    return emu_->cartridge()->GetMemoryPointer(address);
  } else if (address >= 0xC000 && address <= 0xCFFF) {
    return &wram_[address&0x0FFF];
  } else if (address >= 0xD000 && address <= 0xDFFF) {
    return &wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xE000 && address <= 0xFDFF) {
    if ((address&0xF000)==0xE000)
      return &wram_[address&0x0FFF];
    else if ((address&0xF000)==0xF000)
      return &wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xFE00 && address <= 0xFE9F) {
    return &oam_[address-0xFE00];
  } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    return nullptr;
  } else if (address >= 0xFF00 && address <= 0xFF7F) {
    return nullptr;
  } else if (address >= 0xFF80 && address <= 0xFFFE) {
    return &hram_[address-0xFF80];
  } else if (address == 0xFFFF) {
    return nullptr;
  }

  return nullptr;
}

uint8_t Memory::Read8(uint16_t address) {
  uint8_t result = 0;
  
  if (address >= 0x0000 && address <= 0x3FFF) {
    if (emu_->mode() == EmuModeGB && ioports_[0x50] == 0 && address < 0x100) { 
      result = dmgrom[address];
    } else if (emu_->mode() == EmuModeGBC &&  ioports_[0x50] == 0 && (address < 0x100 || (address >= 0x200 && address <= 0x8FF))) { 
        result = gbcrom[address];
      //char str[255];
      //sprintf(str,"read gbc rom at 0x%04x\n",address);
      //OutputDebugString(str);
    } else {
      result = emu_->cartridge()->Read(address);
    }



  } else if (address >= 0x4000 && address <= 0x7FFF) {
    result = emu_->cartridge()->Read(address);
  } else if (address >= 0x8000 && address <= 0x9FFF) {

    if ((emu_->lcd_driver()->stat().mode == 3)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
      result = 0xFF;
    else
      result = vram_[(address&0x1FFF)|(vram_select<<13)];
  } else if (address >= 0xA000 && address <= 0xBFFF) {
    result = emu_->cartridge()->Read(address);//emu_->cartridge()->eram()[address&0x1FFF];
  } else if (address >= 0xC000 && address <= 0xCFFF) {
    result = wram_[address&0x0FFF];
  } else if (address >= 0xD000 && address <= 0xDFFF) {
    result = wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xE000 && address <= 0xFDFF) {
    if ((address&0xF000)==0xE000)
     result = wram_[address&0x0FFF];
    else if ((address&0xF000)==0xF000)
     result =  wram_[(address&0x0FFF)+(0x1000*wram_select)];
  } else if (address >= 0xFE00 && address <= 0xFE9F) {

    if ((emu_->lcd_driver()->stat().mode&0x2)&&(emu_->lcd_driver()->lcdc().lcd_enable==1))
      result = 0xFF;
    else
      result = oam_[address-0xFE00];
  } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    int a = 1;
  } else if (address >= 0xFF00 && address <= 0xFF7F) {
    if (address >= 0xFF10 && address <= 0xFF3F) {
      result = emu_->apu()->Read(address);
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
      result = emu_->lcd_driver()->Read(address);
    } else if (address >= 0xFF51 && address <= 0xFF55) { //HDMA
      result = emu_->lcd_driver()->Read(address);
    } else if (address >= 0xFF68 && address <= 0xFF6C) { //color pallete
      result = emu_->lcd_driver()->Read(address);
    } else if (address >= 0xFF04 && address <= 0xFF07) {
      result = emu_->timer()->Read(address);
    } else if (address == 0xFF0F) {
      result = 0xE0|ioports_[address&0xFF];
    } else {
      result = ioports_[address&0xFF];
    }
  } else if (address >= 0xFF80 && address <= 0xFFFE) {
    result = hram_[address-0xFF80];
  } else if (address == 0xFFFF) {
    result = interrupt_enable_register_;
  }

  return result;
}

void Memory::Write8(uint16_t address, uint8_t data) {
  
  if (address >= 0x0000 && address <= 0x3FFF) {
    emu_->cartridge()->Write(address,data);
  } else if (address >= 0x4000 && address <= 0x7FFF) {
    emu_->cartridge()->Write(address,data);
  } else if (address >= 0x8000 && address <= 0x9FFF) {
    if ((emu_->lcd_driver()->stat().mode == 3)&&(emu_->lcd_driver()->lcdc().lcd_enable==1)) {
    } else {
      vram_[(address&0x1FFF)|(vram_select<<13)] = data;
    }
  } else if (address >= 0xA000 && address <= 0xBFFF) {
    emu_->cartridge()->Write(address,data);
  } else if (address >= 0xC000 && address <= 0xCFFF) {
     wram_[(address&0x0FFF)] = data;
  } else if (address >= 0xD000 && address <= 0xDFFF) {
     wram_[(address&0x0FFF)+(0x1000*wram_select)] = data;
  } else if (address >= 0xE000 && address <= 0xFDFF) {
    if ((address&0xF000)==0xE000)
     wram_[(address&0x0FFF)] = data;
    if ((address&0xF000)==0xF000)
     wram_[(address&0x0FFF)+(0x1000*wram_select)] = data;
  } else if (address >= 0xFE00 && address <= 0xFE9F) {
    if ((emu_->lcd_driver()->stat().mode&0x2)&&(emu_->lcd_driver()->lcdc().lcd_enable==1)) {
    } else {
     oam_[address-0xFE00] = data;
    }
  } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    //int a = 1;
     int a = 1;
  } else if (address >= 0xFF00 && address <= 0xFF7F) {
    
    if (address == 0xFF00) {
      ioports_[0]=data & ~0x0F;
    } else if (address == 0xFF01) {
      char str[255];
      sprintf_s(str,"%c",data);
      OutputDebugString(str);
    } else if (address >= 0xFF04 && address <= 0xFF07) {
      emu_->timer()->Write(address,data);
    } else if (address >= 0xFF10 && address <= 0xFF3F) {
      emu_->apu()->Write(address,data);
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
      emu_->lcd_driver()->Write(address,data);
    } else if ((address&0xFF) == 0x4F) { //FF4F - VBK - CGB Mode Only - VRAM Bank
      ioports_[address&0xFF]=data;
      vram_select = data&0x1;
    } else if (address >= 0xFF51 && address <= 0xFF55) { //HDMA
      emu_->lcd_driver()->Write(address,data);
    } else if (address >= 0xFF68 && address <= 0xFF6C) { //color pallete
      emu_->lcd_driver()->Write(address,data);
    } else if ((address&0xFF) == 0x70) { //FF70 - SVBK - CGB Mode Only - WRAM Bank
      ioports_[address&0xFF]=data;
      wram_select = data&0x3;
      if (wram_select == 0) 
        wram_select = 1;
    } else {
      ioports_[address&0xFF]=data;
    }
  } else if (address >= 0xFF80 && address <= 0xFFFE) {
    hram_[address-0xFF80] = data;
  } else if (address == 0xFFFF) {
    interrupt_enable_register_ = data;
  }
  
}

uint8_t Memory::ClockedRead8(uint16_t address) {
  emu_->MachineTick();
  return Read8(address);
}

void Memory::ClockedWrite8(uint16_t address, uint8_t data) {
  emu_->MachineTick();
  Write8(address,data);
}

void Memory::Tick() {
  ioports_[0] |= 0x0F;

  if ((ioports_[0] & 0x10)==0) {
    for (int i=0;i<4;++i) {
      if (joypadflags[i]==true)//pressed
        ioports_[0] &= ~(1<<i);
      else
        ioports_[0] |= (1<<i);
    }
  }

  if ((ioports_[0] & 0x20)==0) {
    for (int i=0;i<4;++i) {
      if (joypadflags[i+4]==true)//pressed
        ioports_[0] &= ~(1<<i);
      else
        ioports_[0] |= (1<<i);
    }
  } 

  if ((ioports_[0]&0x0F) != 0x0F) 
    interrupt_flag() |= 16;

}

void Memory::JoypadPress(JoypadKeys key) {
  joypadflags[key] = true;
  emu_->cpu()->Wake();
  //ioports_[0] &= ~key;
}

}
}