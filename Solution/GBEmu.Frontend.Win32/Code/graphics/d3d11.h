/*****************************************************************************************************************
* Copyright (c) 2020 Khalid Ali Al-Kooheji                                                                       *
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


#include <d3d11.h>
#include <GBEmu/Solution/Emu.Core/Code/utilities/types.h>
#include "graphics.h"

namespace graphics {

  class D3D11 : public Graphics {
  public:
    D3D11() : device_(nullptr), context_(nullptr), swapchain_(nullptr), rendertargetview_(nullptr) {

    }
    virtual ~D3D11() {

    }
    void Initialize(HWND window_handle, int width, int height);
    void Deinitialize();
    void SetDisplaySize(int width, int height);
    void Clear(RGBQUAD color);
    void Render();

    void CreateTexture(uint32_t width, uint32_t height, ID3D11Texture2D** tex, ID3D11ShaderResourceView** srv);
   

    ID3D11Device* device() { return device_; }
    ID3D11DeviceContext* context() { return context_; }
    IDXGISwapChain* swapchain() { return swapchain_; }
    ID3D11RenderTargetView* rendertargetview() { return rendertargetview_; }
  protected:
    ID3D11Device* device_ ;
    ID3D11DeviceContext* context_ ;
    IDXGISwapChain* swapchain_ ;
    ID3D11RenderTargetView* rendertargetview_ ;

    void CreateRenderTarget();
    bool init_;
  };

}
