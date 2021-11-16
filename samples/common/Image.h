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

#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <wincodec.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class Image
{
public:
    Image();
    void load(const std::filesystem::path& FilePath);
    uint32_t width() { return m_width; }
    uint32_t height() { return m_height; }
    uint32_t bpp() { return m_bpp; }
    uint32_t rowPitch() { return m_rowPitch; }
    uint32_t imageSize() { return m_imageSize; }
    uint8_t* data() { return m_data.data(); }
protected:
    void LoadWIC(const std::wstring& filename);
private:    
    uint32_t m_width, m_height;
    uint32_t m_bpp;
    std::vector<uint8_t> m_data;
    uint32_t m_rowPitch;
    uint32_t m_imageSize;
    ComPtr<IWICImagingFactory> m_WICFactory;
};