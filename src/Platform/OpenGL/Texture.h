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

#include "Utilities/STL/MxString.h"
#include "Utilities/STL/MxVector.h"
#include "Utilities/Math/Math.h"
#include "Utilities/Image/Image.h"

namespace std::filesystem
{
	class path;
}

namespace MxEngine
{
	enum class TextureFormat : uint8_t
	{
		R,
		R16,
		RG,
		RG16,
		R16F,
		R32F,
		RG16F,
		RG32F,
		RGB,
		RGBA,
		RGB16,
		RGB16F,
		RGBA16,
		RGBA16F,
		RGB32F,
		RGBA32F,
		DEPTH,
		DEPTH32F
	};

	enum class TextureWrap : uint8_t
	{
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRRORED_REPEAT,
		REPEAT,
	};

	const char* EnumToString(TextureFormat format);
	const char* EnumToString(TextureWrap wrap);

	class Texture
	{
		using BindableId = unsigned int;

		MxString filepath;
		size_t width = 0, height = 0;
		BindableId id = 0;
		mutable BindableId activeId = 0;
		unsigned int textureType = 0;
		TextureFormat format = TextureFormat::RGB;
		TextureWrap wrapType = TextureWrap::REPEAT;
		uint8_t samples = 0;

		void FreeTexture();
	public:
		using RawData = uint8_t;
		using RawDataPointer = RawData*;
		using TextureBindId = BindableId;

		Texture();
		Texture(const Texture&) = delete;
		Texture(Texture&& texture) noexcept;
		Texture& operator=(const Texture& texture) = delete;
		Texture& operator=(Texture&& texture) noexcept;
		Texture(const std::filesystem::path& filepath, TextureFormat format, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true, bool flipImage = true);
		~Texture();

		void Bind() const;
		void Bind(TextureBindId id) const;
		void Unbind() const;
		BindableId GetBoundId() const;
		BindableId GetNativeHandle() const;
		void Load(const std::filesystem::path& filepath, TextureFormat format, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true, bool flipImage = true);
		void Load(RawDataPointer data, int width, int height, int channels, bool isFloating, TextureFormat format = TextureFormat::RGB, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true);
		void Load(const Image& image, TextureFormat format = TextureFormat::RGB, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true);
		void LoadDepth(int width, int height, TextureFormat format = TextureFormat::DEPTH, TextureWrap wrap = TextureWrap::CLAMP_TO_BORDER);
		void SetSamplingFromLOD(size_t lod);
		size_t GetMaxTextureLOD() const;
		Image GetRawTextureData() const;
		void GenerateMipmaps();
		void SetBorderColor(const Vector3& color);
		bool IsMultisampled() const;
		bool IsFloatingPoint() const;
		bool IsDepthOnly() const;
		int GetSampleCount() const;
		size_t GetPixelSize() const;
		TextureFormat GetFormat() const;
		TextureWrap GetWrapType() const;
		const MxString& GetFilePath() const;
		void SetPath(const MxString& newPath);
		unsigned int GetTextureType() const;
		size_t GetWidth() const;
		size_t GetHeight() const;
		size_t GetChannelCount() const;
	};
}