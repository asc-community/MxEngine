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
	void Image::Free()
	{
		if (this->data != nullptr)
			free(this->data);
	}

	Image::Image() : Image(nullptr, 0, 0, 0) { }

    Image::Image(uint8_t* data, size_t width, size_t height, size_t channels)
		: data(data), width(width), height(height), channels(channels)
	{
	}

	Image::~Image()
	{
		this->Free();
	}

	Image::Image(Image&& other) noexcept
	{
		this->data = other.data;
		this->width = other.width;
		this->height = other.height;
		this->channels = other.channels;
		other.data = nullptr;
		other.width = other.height = other.channels = 0;
	}

	Image& Image::operator=(Image&& other) noexcept
	{
		this->Free();

		this->data = other.data;
		this->width = other.width;
		this->height = other.height;
		this->channels = other.channels;
		other.data = nullptr;
		other.width = other.height = other.channels = 0;
		return *this;
	}

	uint8_t* Image::GetRawData() const
	{
		return this->data;
	}

	size_t Image::GetWidth() const
	{
		return this->width;
	}

	size_t Image::GetHeight() const
	{
		return this->height;
	}

	size_t Image::GetChannels() const
	{
		return this->channels;
	}

    void Image::SetPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
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
			MX_ASSERT(false);
			break;
		}
    }

	std::array<uint8_t, 4> Image::GetPixel(size_t x, size_t y)
	{
		std::array<uint8_t, 4> rgba{ 0, 0, 0, 0 };
		MX_ASSERT(x < this->width&& y < this->height);
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
			MX_ASSERT(false);
			break;
		}
		return rgba;
	}
}