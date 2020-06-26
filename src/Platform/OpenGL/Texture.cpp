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

#include "Texture.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Image/ImageLoader.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Time/Time.h"

namespace MxEngine
{
	GLenum formatTable[] =
	{
		GL_RGB,
		GL_RGBA,
		GL_RGB16,
		GL_RGB16F,
		GL_RGBA16,
		GL_RGBA16F,
		GL_RGB32F,
		GL_RGBA32F,
		GL_DEPTH_COMPONENT
	};

	GLenum wrapTable[] =
	{
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER,
		GL_MIRRORED_REPEAT,
		GL_REPEAT,
	};

	void Texture::FreeTexture()
	{
		if (id != 0)
		{
			GLCALL(glDeleteTextures(1, &id));
		}
		id = 0;
		activeId = 0;
	}

	Texture::Texture()
	{
		GLCALL(glGenTextures(1, &id));
		Logger::Instance().Debug("OpenGL::Texture", "created texture with id = " + ToMxString(id));
	}

	Texture::Texture(Texture&& texture) noexcept
	{
		this->width = texture.width;
		this->height = texture.height;
		this->channels = texture.channels;
		this->textureType = texture.textureType;
		this->filepath = std::move(texture.filepath);
		this->wrapType = texture.wrapType;
		this->samples = texture.samples;
		this->format = texture.format;
		this->id = texture.id;

		texture.id = 0;
		texture.activeId = 0;
		texture.width = 0;
		texture.height = 0;
		texture.channels = 0;
		texture.filepath = "[[deleted]]";
		texture.samples = 0;
	}

	Texture& Texture::operator=(Texture&& texture) noexcept
	{
		this->width = texture.width;
		this->height = texture.height;
		this->channels = texture.channels;
		this->textureType = texture.textureType;
		this->filepath = std::move(texture.filepath);
		this->wrapType = texture.wrapType;
		this->samples = texture.samples;
		this->format = texture.format;
		this->id = texture.id;
		
		texture.id = 0;
		texture.activeId = 0;
		texture.width = 0;
		texture.height = 0;
		texture.channels = 0;
		texture.filepath = "[[deleted]]";
		texture.samples = 0;

		return *this;
	}

	Texture::Texture(const MxString& filepath, TextureWrap wrap, bool genMipmaps, bool flipImage)
		: Texture()
	{
		this->Load(filepath, wrap, genMipmaps, flipImage);
	}

	Texture::~Texture()
	{
		this->FreeTexture();
	}

	void Texture::Load(const MxString& filepath, TextureWrap wrap, bool genMipmaps, bool flipImage)
	{
		Image image = ImageLoader::LoadImage(filepath, flipImage);
		this->filepath = filepath;
		this->wrapType = wrap;
		this->format = TextureFormat::RGB;

		if (image.data == nullptr)
		{
			Logger::Instance().Error("Texture", "file with name '" + filepath + "' was not found");
			return;
		}
		this->width = image.width;
		this->height = image.height;
		this->channels = 3;
		this->textureType = GL_TEXTURE_2D;

		GLCALL(glBindTexture(GL_TEXTURE_2D, id));
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, formatTable[(int)this->format], (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data));

		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapTable[(int)this->wrapType]));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTable[(int)this->wrapType]));
		
		if (genMipmaps) this->GenerateMipmaps();

		ImageLoader::FreeImage(image);
	}

	void Texture::Load(RawDataPointer data, int width, int height, TextureFormat format, TextureWrap wrap, bool genMipmaps)
	{
		this->filepath = "[[raw data]]";
		this->width = width;
		this->height = height;
		this->channels = 3;
		this->textureType = GL_TEXTURE_2D;
		this->format = format;
		this->wrapType = wrap;

		GLenum type = this->IsFloatingPoint() ? GL_FLOAT : GL_UNSIGNED_BYTE;

		GLCALL(glBindTexture(GL_TEXTURE_2D, id));
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, formatTable[(int)this->format], (GLsizei)width, (GLsizei)height, 0, GL_RGB, type, data));

		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapTable[(int)this->wrapType]));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTable[(int)this->wrapType]));

		if (genMipmaps) this->GenerateMipmaps();
	}

	void Texture::LoadMipmaps(Texture::RawDataPointer* data, size_t mipmaps, int biggestWidth, int biggestHeight, TextureWrap wrap)
	{
		this->filepath = "[[raw data]]";
		this->width = biggestWidth;
		this->height = biggestHeight;
		this->channels = 3;
		this->textureType = GL_TEXTURE_2D;
		this->wrapType = wrap;
		this->format = TextureFormat::RGB;

		GLint level = 0;
		GLsizei width = biggestWidth;
		GLsizei height = biggestHeight;

		GLCALL(glBindTexture(GL_TEXTURE_2D, id));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)mipmaps - 1));
		while (width > 0 && height > 0)
		{
			MX_ASSERT(level < mipmaps);
			GLCALL(glTexImage2D(GL_TEXTURE_2D, level, formatTable[(int)this->format], width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[level]));
			height /= 2;
			width /= 2;
			level++;
		}

		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapTable[(int)this->wrapType]));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTable[(int)this->wrapType]));
	}

	void Texture::LoadDepth(int width, int height, TextureWrap wrap)
	{
		this->filepath = "[[depth]]";
		this->width = width;
		this->height = height;
		this->channels = 1;
		this->textureType = GL_TEXTURE_2D;
		this->format = TextureFormat::DEPTH;
		this->wrapType = wrap;
	
		this->Bind();

		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, formatTable[(int)this->format], width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapTable[(int)this->wrapType]));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTable[(int)this->wrapType]));

		this->SetBorderColor(MakeVector4(1.0f));
	}

    void Texture::LoadMultisample(int width, int height, TextureFormat format, int samples, TextureWrap wrap)
    {
		this->filepath = "[[multisample]]";
		this->width = width;
		this->height = height;
		this->channels = 3;
		this->textureType = GL_TEXTURE_2D_MULTISAMPLE;
		this->format = format;
		this->wrapType = wrap;
		this->samples = samples;

		this->Bind();

		GLenum glFormat = formatTable[(int)this->format];

		GLCALL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, glFormat, width, height, GL_TRUE));
    }

    MxVector<Texture::RawData> Texture::GetRawTextureData() const
    {
		MxVector<RawData> result;
		if (this->height == 0 || this->width == 0) 
			return result;

		GLenum type = this->IsFloatingPoint() ? GL_FLOAT : GL_UNSIGNED_BYTE;
		size_t totalByteSize = this->width * this->height;
		GLenum internalFormat = GL_RGBA;
		switch (this->format)
		{
		case MxEngine::TextureFormat::RGB:
		case MxEngine::TextureFormat::RGBA:
		case MxEngine::TextureFormat::DEPTH:
			totalByteSize *= 4 * sizeof(uint8_t);
			internalFormat = GL_RGBA;
			break;
		case MxEngine::TextureFormat::RGB16:
		case MxEngine::TextureFormat::RGBA16:
			totalByteSize *= 4 * sizeof(uint16_t);
			internalFormat = GL_RGBA16;
			break;
		case MxEngine::TextureFormat::RGB16F:
		case MxEngine::TextureFormat::RGBA16F:
			totalByteSize *= 4 * sizeof(uint16_t);
			internalFormat = GL_RGBA16F;
			break;
		case MxEngine::TextureFormat::RGB32F:
		case MxEngine::TextureFormat::RGBA32F:
			totalByteSize *= 4 * sizeof(uint32_t);
			internalFormat = GL_RGBA32F;
		default:
			MX_ASSERT(false);
		}
		result.resize(totalByteSize);

		this->Bind(0);
		GLCALL(glGetTexImage(this->textureType, 0, internalFormat, type, (void*)result.data()));
		return result;
    }

	void Texture::GenerateMipmaps()
	{
		this->Bind(0);
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
	}

    void Texture::SetBorderColor(const Vector3& color)
    {
		this->Bind(0);
		auto normalized = Clamp(color, MakeVector3(0.0f), MakeVector3(1.0f));
		GLCALL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &normalized[0]));
    }

    bool Texture::IsMultisampled() const
    {
		return this->textureType == GL_TEXTURE_2D_MULTISAMPLE;
    }

	bool Texture::IsFloatingPoint() const
	{
		return (format == TextureFormat::RGB16F ) || (format == TextureFormat::RGB32F ) ||
			   (format == TextureFormat::RGBA16F) || (format == TextureFormat::RGBA32F);
	}

	bool Texture::IsDepthOnly() const
	{
		return this->format == TextureFormat::DEPTH;
	}

    int Texture::GetSampleCount() const
    {
		return (int)this->samples;
    }

	size_t Texture::GetPixelSize() const
	{
		switch (this->format)
		{
		case MxEngine::TextureFormat::RGB:
			return 3 * sizeof(uint8_t);
		case MxEngine::TextureFormat::RGBA:
			return 4 * sizeof(uint8_t);
		case MxEngine::TextureFormat::RGB16:
			return 3 * sizeof(uint16_t);
		case MxEngine::TextureFormat::RGB16F:
			return 3 * sizeof(uint16_t);
		case MxEngine::TextureFormat::RGBA16:
			return 4 * sizeof(uint16_t);
		case MxEngine::TextureFormat::RGBA16F:
			return 4 * sizeof(uint16_t);
		case MxEngine::TextureFormat::RGB32F:
			return 3 * sizeof(uint32_t);
		case MxEngine::TextureFormat::RGBA32F:
			return 4 * sizeof(uint32_t);
		case MxEngine::TextureFormat::DEPTH:
			return 1 * sizeof(uint8_t);
		default:
			return 0;
		}
	}

	TextureFormat Texture::GetFormat() const
	{
		return this->format;
	}

	TextureWrap Texture::GetWrapType() const
	{
		return this->wrapType;
	}

	void Texture::Bind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(this->textureType, id));
	}

	void Texture::Unbind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(this->textureType, 0));
	}

	Texture::BindableId Texture::GetNativeHandle() const
    {
		return id;
    }

	void Texture::Bind(TextureBindId id) const
	{
		this->activeId = id;
		this->Bind();
	}

	const MxString& Texture::GetPath() const
	{
		return this->filepath;
	}

    unsigned int Texture::GetTextureType() const
    {
		return this->textureType;
    }

	size_t Texture::GetWidth() const
	{
		return width;
	}

	size_t Texture::GetHeight() const
	{
		return height;
	}

	size_t Texture::GetChannelCount() const
	{
		return channels;
	}

    const char* EnumToString(TextureFormat format)
    {
		#define TEX_FMT_STR(val) case TextureFormat::val: return #val
		switch (format)
		{
			TEX_FMT_STR(RGB);
			TEX_FMT_STR(RGBA);
			TEX_FMT_STR(RGB16);
			TEX_FMT_STR(RGB16F);
			TEX_FMT_STR(RGBA16);
			TEX_FMT_STR(RGBA16F);
			TEX_FMT_STR(RGB32F);
			TEX_FMT_STR(RGBA32F);
			TEX_FMT_STR(DEPTH);
		default:
			return "INVALID_FORMAT";
		}
    }

    const char* EnumToString(TextureWrap wrap)
    {
		#define TEX_WRAP_STR(val) case TextureWrap::val: return #val
		switch (wrap)
		{
			TEX_WRAP_STR(CLAMP_TO_EDGE);
			TEX_WRAP_STR(CLAMP_TO_BORDER);
			TEX_WRAP_STR(MIRRORED_REPEAT);
			TEX_WRAP_STR(REPEAT);
		default:
			return "INVALID_WRAPTYPE";
		}
    }
}