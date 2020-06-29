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
#include <memory>

namespace MxEngine
{
	Image::Image(uint8_t* data, size_t width, size_t height, size_t channels)
		: data(data), width(width), height(height), channels(channels)
	{
	}

	Image::~Image()
	{
		if (this->data != nullptr)
			std::free(this->data);
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
}