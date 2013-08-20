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
#include "wasapi.h"


namespace audio {
namespace output {

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

WASAPI::WASAPI() : device_enumurator(nullptr),device(nullptr),audio_client(nullptr),
                   render_client(nullptr),pwfx(nullptr),buffer_sample_count(0) {
  numFramesAvailable=0;
  numFramesPadding=0;

	hnsRequestedDuration = (REFERENCE_TIME)(REFTIMES_PER_SEC*0.4);
  hnsActualDuration;
  written_samples;
  memset(&fmt,0,sizeof(fmt));
}

WASAPI::~WASAPI() {

}

int WASAPI::Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits) {
  static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
  static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
  static const IID IID_IAudioClient = __uuidof(IAudioClient);
  static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

  CoInitializeEx(nullptr,COINIT_MULTITHREADED);
  //WAVEFORMATEX* closest_fmt = (WAVEFORMATEX*)CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE));
  //WAVEFORMATEX** pclosest_fmt = &closest_fmt;
  fmt.Format.wFormatTag			= WAVE_FORMAT_EXTENSIBLE;
  fmt.Format.nChannels				= channels;
  fmt.Format.nSamplesPerSec	= sample_rate;
  fmt.Format.wBitsPerSample	= (bits+7)&~7;
  fmt.Format.nBlockAlign			= channels * fmt.Format.wBitsPerSample>>3;
  fmt.Format.nAvgBytesPerSec	= fmt.Format.nBlockAlign * fmt.Format.nSamplesPerSec;
  fmt.Format.cbSize					= 22;
  fmt.Samples.wValidBitsPerSample	= bits;
	fmt.dwChannelMask					= channels==2 ? 3 : 4;	//Select left & right (stereo) or center (mono)
	fmt.SubFormat						= KSDATAFORMAT_SUBTYPE_PCM;
  pwfx = &fmt.Format;
  
  HRESULT hr;
  hr = CoCreateInstance(CLSID_MMDeviceEnumerator,NULL,CLSCTX_ALL,IID_IMMDeviceEnumerator,(void**)&device_enumurator);
  if (FAILED(hr)) return S_FALSE;
  hr = device_enumurator->GetDefaultAudioEndpoint(eRender,eConsole,&device);
  if (FAILED(hr)) {
    SafeRelease(&device_enumurator);
    return S_FALSE;
  } 
  hr = device->Activate(IID_IAudioClient, CLSCTX_ALL,NULL, (void**)&audio_client);
  if (FAILED(hr)) {
    SafeRelease(&device);
    SafeRelease(&device_enumurator);
    return S_FALSE;
  } 
  //hr = audio_client->GetMixFormat(&pwfx);
  //if (FAILED(hr)) return S_FALSE;
  //hr = audio_client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,(WAVEFORMATEX*)&fmt,pclosest_fmt);
  //hr = audio_client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,(WAVEFORMATEX*)&fmt,NULL);
  //CoTaskMemFree(closest_fmt);
  hr = audio_client->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE,0,hnsRequestedDuration,0,pwfx,NULL);
  if (FAILED(hr)) {
    SafeRelease(&audio_client);
    SafeRelease(&device);
    SafeRelease(&device_enumurator);
    return S_FALSE;
  } 

  memcpy(&wave_format_,pwfx,sizeof(wave_format_));

  // Get the actual size of the allocated buffer.
  hr = audio_client->GetBufferSize(&buffer_sample_count);
  if (FAILED(hr)) return S_FALSE;

  hr = audio_client->GetService(IID_IAudioRenderClient,(void**)&render_client);
  if (FAILED(hr)) return S_FALSE;

  hnsActualDuration = REFERENCE_TIME((double)REFTIMES_PER_SEC *
                        buffer_sample_count / fmt.Format.nSamplesPerSec);

  written_samples = 0;

  return S_OK;
}
int WASAPI::Deinitialize() {
  SafeRelease(&render_client);
  SafeRelease(&audio_client);
  SafeRelease(&device);
  SafeRelease(&device_enumurator);
  //CoTaskMemFree(pwfx);
  return S_OK;
}
int WASAPI::Play() {
   return audio_client->Start();

}

int WASAPI::Stop() {
  return audio_client->Stop();
}

uint32_t WASAPI::GetBytesBuffered() {
  return written_samples;
}

void WASAPI::GetCursors(uint32_t& play, uint32_t& write) {
  UINT32* p=nullptr;
  audio_client->GetCurrentPadding(p);
  write = buffer_sample_count - *p;
}

int WASAPI::Write(void* data_pointer, uint32_t size_bytes) {
  BYTE *dest_buf=nullptr;
  uint32_t frames = size_bytes >>2; // div by 2 channels, div by sizeof(short)
  render_client->GetBuffer(frames, &dest_buf);
  if (dest_buf) {
    memcpy(dest_buf,data_pointer,size_bytes);
    written_samples += frames;
  }
  render_client->ReleaseBuffer(frames, 0);
  
  return S_OK;
}

int WASAPI::BeginWrite(uint32_t& samples) {
  UINT32 p;
  auto hr = audio_client->GetCurrentPadding(&p);
  numFramesAvailable = samples = buffer_sample_count - p;
  return S_OK;
}
int WASAPI::EndWrite(void* data_pointer)  {
  return Write(data_pointer,numFramesAvailable<<2);
}  


/*
int WASAPI_GetSampleRate() {
  return pwfx->nSamplesPerSec;
}
*/



}
}