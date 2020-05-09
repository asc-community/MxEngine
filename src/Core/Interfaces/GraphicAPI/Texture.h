// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "Core/Interfaces/GraphicAPI/IBindable.h"
#include <string>

namespace MxEngine
{
	enum class TextureFormat
	{
		RGB,
		RGBA,
		RGB16,
		RGB16F,
		RGBA16,
		RGBA16F,
		RGB32F,
		RGBA32F,
	};

	enum class TextureWrap
	{
		CLAMP_TO_EDGE, 
		MIRRORED_REPEAT, 
		REPEAT,
	};

	struct Texture : IBindable
	{
		using RawDataPointer = const uint8_t*;
		using IdType = IBindable::IdType;

		virtual void Load(const std::string& filepath, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true, bool flipImage = true) = 0;
		virtual void Load(RawDataPointer data, int width, int height, TextureFormat format = TextureFormat::RGBA, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true) = 0;
		virtual void LoadMipmaps(RawDataPointer* textures, size_t mipmapCount, int biggestWidth, int biggestHeight, TextureWrap wrap = TextureWrap::REPEAT) = 0;
		virtual void LoadDepth(int width, int height, TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE) = 0;
		virtual void LoadMultisample(int width, int height, TextureFormat format, int samples, TextureWrap wrap = TextureWrap::REPEAT) = 0;
		virtual void Bind(IdType id) const = 0;
		virtual const std::string& GetPath() const = 0;
		virtual unsigned int GetTextureType() const = 0;

		virtual size_t GetWidth() const = 0;
		virtual size_t GetHeight() const = 0;
		virtual size_t GetChannelCount() const = 0;
	};
}