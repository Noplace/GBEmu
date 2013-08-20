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
#include "xaudio.h"

//#pragma comment(lib, "xmcore.lib")
#ifdef _DEBUG
#pragma comment(lib, "XAudio2.lib")
#else
#pragma comment(lib, "XAudio2.lib")
#endif

namespace audio {
namespace output {

XAudio::XAudio() : window_handle_(nullptr), buffer_size_(0) {
  pXAudio2 = nullptr;
  pMasteringVoice = nullptr;
  pSourceVoice = nullptr;
}

XAudio::~XAudio() {

}
  
int XAudio::Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits) {
  HRESULT hr;
  CoInitializeEx( NULL, COINIT_MULTITHREADED );
  UINT32 flags = 0;
  #ifdef _DEBUG
    //flags |= XAUDIO2_DEBUG_ENGINE;
  #endif
		
  if  (FAILED(hr = XAudio2Create(&pXAudio2,flags)))  {
    Deinitialize();
    return S_FALSE;
  }

  if (FAILED( hr = pXAudio2->CreateMasteringVoice( &pMasteringVoice ) ) )  {
    Deinitialize();
    return S_FALSE;
  }
  

	ZeroMemory( &wave_format_, sizeof(WAVEFORMATEX) ); 
	wave_format_.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wave_format_.nChannels       =  channels; 
	wave_format_.nSamplesPerSec  = sample_rate; 
	wave_format_.wBitsPerSample  = (WORD) bits; 
	wave_format_.nBlockAlign     = (WORD) ((wave_format_.wBitsPerSample >> 3) * wave_format_.nChannels);
	wave_format_.nAvgBytesPerSec = (DWORD) (wave_format_.nSamplesPerSec * wave_format_.nBlockAlign);
  wave_format_.cbSize = 0;
  if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wave_format_, 0, 1.0f, &voiceContext))) {
    Deinitialize();
    return S_FALSE;
  }
  
  pSourceVoice->Start( 0, 0 );
  //float cv[] = { 0.0f , 0.1f };
  //auto result = pSourceVoice->SetChannelVolumes(2,cv);
  return S_OK;
}

int XAudio::Deinitialize() {
  pSourceVoice->Stop( 0 );
  pSourceVoice->DestroyVoice();
  pMasteringVoice->DestroyVoice();
  SafeRelease(&pXAudio2);
  CoUninitialize();
  return S_OK;
}

uint32_t XAudio::GetBytesBuffered() {
 return 0;
}

int XAudio::Write(void* data_pointer, uint32_t size_bytes) {
  
  XAUDIO2_VOICE_STATE state;
  for(; ; )  {
      pSourceVoice->GetState( &state );
      if( state.BuffersQueued == 0  )
          break;
      WaitForSingleObject(voiceContext.hBufferEndEvent, INFINITE);
  }

 // pSourceVoice->GetState( &state );
 //     if( state.BuffersQueued )
  //       return S_FALSE;

  XAUDIO2_BUFFER buf = {0};
  buf.AudioBytes = size_bytes;
  
 // BYTE* buffer_data = new BYTE[size_bytes];
  //memcpy(buffer_data,data_pointer,size_bytes);
  //buf.pContext = buffer_data;
  buf.pAudioData = (const BYTE*)data_pointer;//buffer_data;
  
  buf.LoopCount = XAUDIO2_LOOP_INFINITE;
  //if( currentPosition >= waveLength )
  //    buf.Flags = XAUDIO2_END_OF_STREAM;

  pSourceVoice->SubmitSourceBuffer( &buf );
  return S_OK;
}

}
}