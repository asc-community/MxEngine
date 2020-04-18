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

#include "GLTexture.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"
#include "Utilities/ImageLoader/ImageLoader.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Time/Time.h"

namespace MxEngine
{
	void GLTexture::FreeTexture()
	{
		if (id != 0)
		{
			GLCALL(glDeleteTextures(1, &id));
		}
		id = 0;
		activeId = 0;
	}

	GLTexture::GLTexture()
	{
		GLCALL(glGenTextures(1, &id));
		Logger::Instance().Debug("OpenGL::Texture", "created texture with id = " + std::to_string(id));
	}

	GLTexture::GLTexture(GLTexture&& texture) noexcept
		: width(texture.width), height(texture.height), channels(texture.channels)
	{
		this->id = texture.id;
		texture.id = 0;
		texture.activeId = 0;
		texture.width = 0;
		texture.height = 0;
		texture.channels = 0;
	}

	GLTexture::GLTexture(const std::string& filepath, bool genMipmaps, bool flipImage)
	{
		Load(filepath, genMipmaps, flipImage);
	}

	GLTexture::~GLTexture()
	{
		this->FreeTexture();
	}

	void GLTexture::Load(const std::string& filepath, bool genMipmaps, bool flipImage)
	{
		Image image = ImageLoader::LoadImage(filepath, flipImage);
		this->filepath = filepath;

		if (image.data == nullptr)
		{
			Logger::Instance().Error("Texture", "file with name '" + filepath + "' was not found");
			return;
		}
		MX_ASSERT(image.channels == 3);
		this->width = image.width;
		this->height = image.height;
		this->channels = image.channels;

		GLCALL(glBindTexture(GL_TEXTURE_2D, id));
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data));

		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
		if (genMipmaps)
		{
			GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
		}

		ImageLoader::FreeImage(image);
	}

	void GLTexture::Load(RawDataPointer data, int width, int height, bool genMipmaps)
	{
		this->filepath = "[[raw data]]";
		this->width = width;
		this->height = height;
		this->channels = 3;

		GLCALL(glBindTexture(GL_TEXTURE_2D, id));
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));

		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		if (genMipmaps)
		{
			GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
		}
	}

	void GLTexture::LoadMipmaps(Texture::RawDataPointer* data, size_t mipmaps, int biggestWidth, int biggestHeight)
	{
		this->filepath = "[[raw data]]";
		this->width = biggestWidth;
		this->height = biggestHeight;
		this->channels = 3;

		GLint level = 0;
		GLsizei width = biggestWidth;
		GLsizei height = biggestHeight;

		GLCALL(glBindTexture(GL_TEXTURE_2D, id));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)mipmaps - 1));
		while (width > 0 && height > 0)
		{
			MX_ASSERT(level < mipmaps);
			GLCALL(glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[level]));
			height /= 2;
			width /= 2;
			level++;
		}

		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}

	void GLTexture::LoadDepth(int width, int height)
	{
		this->filepath = "[[depth]]";
		this->width = width;
		this->height = height;
	
		this->Bind();

		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	}

	void GLTexture::Bind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(GL_TEXTURE_2D, id));
	}

	void GLTexture::Unbind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void GLTexture::Bind(IBindable::IdType id) const
	{
		this->activeId = id;
		this->Bind();
	}

	const std::string& GLTexture::GetPath() const
	{
		return this->filepath;
	}

	size_t GLTexture::GetWidth() const
	{
		return width;
	}

	size_t GLTexture::GetHeight() const
	{
		return height;
	}

	size_t GLTexture::GetChannelCount() const
	{
		return channels;
	}
}