// The MIT License(MIT)
// 
// Copyright(c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files(the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "Image.h"
#include "DXUtilities.h"

Image::Image() 
{
    DX::ThrowIfFailed(CoInitialize(nullptr));
    DX::ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&m_WICFactory));
}

void Image::load(const std::filesystem::path& FilePath)
{
    if (FilePath.extension() == ".png")
        LoadWIC(FilePath.wstring());
}

void Image::LoadWIC(const std::wstring& filename)
{
    wchar_t* wfilename = _wcsdup(filename.c_str());
    ComPtr<IWICBitmapDecoder> decoder;
    DX::ThrowIfFailed(m_WICFactory->CreateDecoderFromFilename(wfilename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder));
    ComPtr<IWICBitmapFrameDecode> frame;
    DX::ThrowIfFailed(decoder->GetFrame(0, &frame));
    DX::ThrowIfFailed(frame->GetSize(&m_width, &m_height));
    // get bpp
    WICPixelFormatGUID pixelFormat;
    DX::ThrowIfFailed(frame->GetPixelFormat(&pixelFormat));
    ComPtr<IWICBitmapSource> pConvertedFrame;
    if (pixelFormat != GUID_WICPixelFormat32bppRGBA)
    {
        DX::ThrowIfFailed(WICConvertBitmapSource(GUID_WICPixelFormat32bppRGBA, frame.Get(), &pConvertedFrame));
    }
    else
    {
        pConvertedFrame = frame;
    }
    DX::ThrowIfFailed(pConvertedFrame->GetPixelFormat(&pixelFormat));
    ComPtr<IWICComponentInfo> cinfo;
    DX::ThrowIfFailed(m_WICFactory->CreateComponentInfo(pixelFormat, &cinfo));
    ComPtr<IWICPixelFormatInfo> pfinfo;
    DX::ThrowIfFailed(cinfo->QueryInterface(__uuidof(IWICPixelFormatInfo), &pfinfo));
    DX::ThrowIfFailed(pfinfo->GetBitsPerPixel(&m_bpp));
    m_rowPitch = (m_width * m_bpp + 7) / 8;
    m_imageSize = m_rowPitch * m_height;
    m_data.resize(m_imageSize);
    DX::ThrowIfFailed(pConvertedFrame->CopyPixels(0, m_rowPitch, m_imageSize, m_data.data()));
}