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

#include "Image.h"
#include "Core/Macro/Macro.h"
#include <memory>
#include <cstdlib>

namespace MxEngine
{
    void ImageData::Free()
    {
        if (this->data != nullptr)
            free(this->data);
    }

    ImageData::ImageData() : ImageData(nullptr, 0, 0, 0, false) { }

    ImageData::ImageData(uint8_t* data, size_t width, size_t height, size_t channels, bool isFloatingPoint)
        : data(data), width(width), height(height), channels((uint8_t)channels), isFloatingPoint(isFloatingPoint)
    {
    }

    ImageData::~ImageData()
    {
        this->Free();
    }

    ImageData::ImageData(ImageData&& other) noexcept
    {
        this->data = other.data;
        this->width = other.width;
        this->height = other.height;
        this->channels = other.channels;
        this->isFloatingPoint = other.isFloatingPoint;
        other.data = nullptr;
        other.width = other.height = other.channels = other.isFloatingPoint = 0;
    }

    ImageData& ImageData::operator=(ImageData&& other) noexcept
    {
        this->Free();

        this->data = other.data;
        this->width = other.width;
        this->height = other.height;
        this->channels = other.channels;
        this->isFloatingPoint = other.isFloatingPoint;
        other.data = nullptr;
        other.width = other.height = other.channels = other.isFloatingPoint = 0;
        return *this;
    }

    uint8_t* ImageData::GetRawData() const
    {
        return this->data;
    }

    size_t ImageData::GetWidth() const
    {
        return this->width;
    }

    size_t ImageData::GetHeight() const
    {
        return this->height;
    }

    size_t ImageData::GetChannelCount() const
    {
        return (size_t)this->channels;
    }

    size_t ImageData::GetChannelSize() const
    {
        return this->isFloatingPoint ? sizeof(float) : sizeof(uint8_t);
    }

    size_t ImageData::GetPixelSize() const
    {
        return this->GetChannelCount() * this->GetChannelSize();
    }

    size_t ImageData::GetTotalByteSize() const
    {
        return this->GetHeight() * this->GetWidth() * this->GetPixelSize();
    }

    bool ImageData::IsFloatingPoint() const
    {
        return this->isFloatingPoint;
    }

    void ImageData::SetPixelByte(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (this->isFloatingPoint)
        {
            this->SetPixelFloat(x, y, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
            return;
        }

        MX_ASSERT(x < this->width && y < this->height);
        switch (this->channels)
        {
        case 1:
            this->data[x * this->height + y] = r;
            break;
        case 2:
            this->data[(x * this->height + y) * 2 + 0] = r;
            this->data[(x * this->height + y) * 2 + 1] = g;
            break;
        case 3:
            this->data[(x * this->height + y) * 3 + 0] = r;
            this->data[(x * this->height + y) * 3 + 1] = g;
            this->data[(x * this->height + y) * 3 + 2] = b;
            break;
        case 4:
            this->data[(x * this->height + y) * 4 + 0] = r;
            this->data[(x * this->height + y) * 4 + 1] = g;
            this->data[(x * this->height + y) * 4 + 2] = b;
            this->data[(x * this->height + y) * 4 + 3] = a;
            break;
        default:
            MX_ASSERT(false); // invalid channel count
            break;
        }
    }

    template<typename T>
    T Clamp(T v, T minv, T maxv)
    {
        return (v < minv) ? minv : (v > maxv ? maxv : v);
    }

    void ImageData::SetPixelFloat(size_t x, size_t y, float r, float g, float b, float a)
    {
        if (!this->isFloatingPoint)
        {
            this->SetPixelByte(x, y, 
                (uint8_t)Clamp(r * 255.0f, 0.0f, 255.0f),
                (uint8_t)Clamp(g * 255.0f, 0.0f, 255.0f),
                (uint8_t)Clamp(b * 255.0f, 0.0f, 255.0f),
                (uint8_t)Clamp(a * 255.0f, 0.0f, 255.0f)
            );
            return;
        }

        MX_ASSERT(x < this->width&& y < this->height);
        switch (this->channels)
        {
        case 1:
            ((float*)this->data)[x * this->height + y] = r;
            break;
        case 2:
            ((float*)this->data)[(x * this->height + y) * 2 + 0] = r;
            ((float*)this->data)[(x * this->height + y) * 2 + 1] = g;
            break;
        case 3:
            ((float*)this->data)[(x * this->height + y) * 3 + 0] = r;
            ((float*)this->data)[(x * this->height + y) * 3 + 1] = g;
            ((float*)this->data)[(x * this->height + y) * 3 + 2] = b;
            break;
        case 4:
            ((float*)this->data)[(x * this->height + y) * 4 + 0] = r;
            ((float*)this->data)[(x * this->height + y) * 4 + 1] = g;
            ((float*)this->data)[(x * this->height + y) * 4 + 2] = b;
            ((float*)this->data)[(x * this->height + y) * 4 + 3] = a;
            break;
        default:
            MX_ASSERT(false); // invalid channel count
            break;
        }
    }

    std::array<uint8_t, 4> ImageData::GetPixelByte(size_t x, size_t y) const
    {
        std::array<uint8_t, 4> rgba{ 0, 0, 0, 255 };
        if (this->isFloatingPoint)
        {
            auto pixel = this->GetPixelFloat(x, y);
            rgba[0] = (uint8_t)Clamp(pixel[0] * 255.0f, 0.0f, 255.0f);
            rgba[1] = (uint8_t)Clamp(pixel[1] * 255.0f, 0.0f, 255.0f);
            rgba[2] = (uint8_t)Clamp(pixel[2] * 255.0f, 0.0f, 255.0f);
            rgba[3] = (uint8_t)Clamp(pixel[3] * 255.0f, 0.0f, 255.0f);
            return rgba;
        }

        MX_ASSERT(x < this->width && y < this->height);
        switch (this->channels)
        {
        case 1:
            rgba[0] = this->data[x * this->height + y];
            break;
        case 2:
            rgba[0] = this->data[(x * this->height + y) * 2 + 0];
            rgba[1] = this->data[(x * this->height + y) * 2 + 1];
            break;
        case 3:
            rgba[0] = this->data[(x * this->height + y) * 3 + 0];
            rgba[1] = this->data[(x * this->height + y) * 3 + 1];
            rgba[2] = this->data[(x * this->height + y) * 3 + 2];
            break;
        case 4:
            rgba[0] = this->data[(x * this->height + y) * 4 + 0];
            rgba[1] = this->data[(x * this->height + y) * 4 + 1];
            rgba[2] = this->data[(x * this->height + y) * 4 + 2];
            rgba[3] = this->data[(x * this->height + y) * 4 + 3];
            break;
        default:
            MX_ASSERT(false); // invalid channel count
            break;
        }
        return rgba;
    }

    std::array<float, 4> ImageData::GetPixelFloat(size_t x, size_t y) const
    {
        std::array<float, 4> rgba{ 0.0f, 0.0f, 0.0f, 1.0f };
        if (!this->isFloatingPoint)
        {
            auto pixel = this->GetPixelByte(x, y);
            rgba[0] = (float)pixel[0] / 255.0f;
            rgba[1] = (float)pixel[1] / 255.0f;
            rgba[2] = (float)pixel[2] / 255.0f;
            rgba[3] = (float)pixel[3] / 255.0f;
            return rgba;
        }

        MX_ASSERT(x < this->width && y < this->height);
        switch (this->channels)
        {
        case 1:
            rgba[0] = ((float*)this->data)[x * this->height + y];
            break;
        case 2:
            rgba[0] = ((float*)this->data)[(x * this->height + y) * 2 + 0];
            rgba[1] = ((float*)this->data)[(x * this->height + y) * 2 + 1];
            break;
        case 3:
            rgba[0] = ((float*)this->data)[(x * this->height + y) * 3 + 0];
            rgba[1] = ((float*)this->data)[(x * this->height + y) * 3 + 1];
            rgba[2] = ((float*)this->data)[(x * this->height + y) * 3 + 2];
            break;
        case 4:
            rgba[0] = ((float*)this->data)[(x * this->height + y) * 4 + 0];
            rgba[1] = ((float*)this->data)[(x * this->height + y) * 4 + 1];
            rgba[2] = ((float*)this->data)[(x * this->height + y) * 4 + 2];
            rgba[3] = ((float*)this->data)[(x * this->height + y) * 4 + 3];
            break;
        default:
            MX_ASSERT(false); // invalid channel count
            break;
        }
        return rgba;
    }
}