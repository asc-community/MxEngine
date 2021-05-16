// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace MxEngine
{
    class Image
    {
        uint8_t* data;
        size_t width;
        size_t height;
        uint8_t channels;
        bool isFloatingPoint;

        void Free();
    public:
        Image();
        Image(uint8_t* data, size_t width, size_t height, size_t channels, bool isFloatingPoint);
        ~Image();
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
        Image(Image&&) noexcept;
        Image& operator=(Image&&) noexcept;

        uint8_t* GetRawData() const;
        size_t GetWidth() const;
        size_t GetHeight() const;
        size_t GetChannelCount() const;
        size_t GetChannelSize() const;
        size_t GetPixelSize() const;
        size_t GetTotalByteSize() const;
        bool IsFloatingPoint() const;

        void SetPixelByte(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void SetPixelFloat(size_t x, size_t y, float r, float g, float b, float a = 1.0f);
        std::array<uint8_t, 4> GetPixelByte(size_t x, size_t y) const;
        std::array<float, 4> GetPixelFloat(size_t x, size_t y) const;
    };
}