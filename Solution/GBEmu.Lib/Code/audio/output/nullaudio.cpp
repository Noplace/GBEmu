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
#include "nullaudio.h"


namespace audio {
namespace output {

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

NullAudio::NullAudio() {
}

NullAudio::~NullAudio() {

}

int NullAudio::Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits) {

  return S_OK;
}
int NullAudio::Deinitialize() {

  return S_OK;
}
int NullAudio::Play() {
  return S_OK;

}

int NullAudio::Stop() {
  return S_OK;
}

uint32_t NullAudio::GetBytesBuffered() {
  return 0;;
}

void NullAudio::GetCursors(uint32_t& play, uint32_t& write) {

}

int NullAudio::Write(void* data_pointer, uint32_t size_bytes) {
  
  
  return S_OK;
}

int NullAudio::BeginWrite(uint32_t& samples) {
 
  return S_OK;
}
int NullAudio::EndWrite(void* data_pointer)  {
  return 0;
}  


/*
int WASAPI_GetSampleRate() {
  return pwfx->nSamplesPerSec;
}
*/



}
}