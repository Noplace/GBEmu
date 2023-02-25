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
#pragma once
//#include "../../utilities/windows/windows.h"
//#include <GBEmu/Solution/Emu.Core/Code/utilities/types.h>
#include "..\..\..\..\Emu.Core\Code\utilities\types.h"
#include <objbase.h>
#pragma warning(push)
#pragma warning(disable : 4201)
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#pragma warning(pop)
//#include <GBEmu/Solution/Emu.Core/Code/audio/output/interface.h>
#include "..\..\..\..\Emu.Core\Code\audio\output\interface.h"

namespace audio {
namespace output {

class WASAPI : public Interface {
 public:
  WASAPI();
  ~WASAPI();
  int Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits);
  int Deinitialize();
  int Play();
  int Stop();
  uint32_t GetBytesBuffered();
  void GetCursors(uint32_t& play, uint32_t& write);
  int Write(void* data_pointer, uint32_t size_bytes);
  int BeginWrite(uint32_t& samples);
  int EndWrite(void* data_pointer) ;  
  void Sync() {}
 protected:
  IMMDeviceEnumerator* device_enumurator;
  IMMDevice *device;
  IAudioClient* audio_client;
  IAudioRenderClient* render_client;
  WAVEFORMATEX *pwfx;
  WAVEFORMATEXTENSIBLE fmt;
  REFERENCE_TIME hnsRequestedDuration;
  REFERENCE_TIME hnsActualDuration;
  UINT32 buffer_sample_count;
  UINT32 numFramesAvailable;
  UINT32 numFramesPadding;
  UINT32 written_samples;




};

}
}