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

#include "GLCubeMap.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"
#include "Utilities/ImageLoader/ImageLoader.h"
#include "Utilities/Logger/Logger.h"

namespace MxEngine
{
	void GLCubeMap::FreeCubeMap()
	{
		if (id != 0)
		{
			GLCALL(glDeleteTextures(1, &id));
		}
		id = 0;
		activeId = 0;
	}

	GLCubeMap::GLCubeMap()
	{
		GLCALL(glGenTextures(1, &id));
		Logger::Instance().Debug("OpenGL::CubeMap", "created cubemap with id = " + std::to_string(id));
	}

	GLCubeMap::GLCubeMap(GLCubeMap&& other) noexcept
	{
		this->id = other.id;
		this->activeId = other.activeId;
		this->width = other.width;
		this->height = other.height;
		this->channels = other.channels;

		other.id = 0;
		other.activeId = 0;
		other.width = 0;
		other.height = 0;
		other.channels = 0;
	}

	GLCubeMap::~GLCubeMap()
	{
		this->FreeCubeMap();
	}

	void GLCubeMap::Bind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
	}

	void GLCubeMap::Unbind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}

	void GLCubeMap::Bind(CubeMapId id) const
	{
		this->activeId = id;
		this->Bind();
	}

	void GLCubeMap::Load(const std::string& filepath, bool genMipmaps, bool flipImage)
	{
		Image img = ImageLoader::LoadImage(filepath, flipImage);
		if (img.data == nullptr)
		{
			Logger::Instance().Error("OpenGL::CubeMap", "file with name '" + filepath + "' was not found");
			return;
		}
		auto images = ImageLoader::CreateFromSingle(img);
		MX_ASSERT(img.channels >= 3);
		this->filepath = filepath;
		this->channels = img.channels;
		this->width = img.width;
		this->height = img.height;
		ImageLoader::FreeImage(img);

		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
		int glChannels = 3;
		for (size_t i = 0; i < 6; i++)
		{
			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)i, 0, GL_RGBA, 
				(GLsizei)images[i].width(), (GLsizei)images[i].height() / 3, 0, GL_RGB, GL_UNSIGNED_BYTE, images[i].data()));
		}
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
		if (genMipmaps)
		{
			GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
		}
	}

	void GLCubeMap::Load(std::array<RawDataPointer, 6> data, int width, int height, bool genMipmaps)
	{
		this->width = width;
		this->height = height;
		this->filepath = "[[raw data]]";
		this->channels = 3;

		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
		for (size_t i = 0; i < data.size(); i++)
		{

			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)i, 0, GL_RGBA,
				(GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]));
		}
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
		if (genMipmaps)
		{
			GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
		}
	}

	void GLCubeMap::LoadDepth(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->filepath = "[[depth]]";
		this->channels = 3;
		
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
		for (size_t i = 0; i < 6; i++)
		{
			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)i, 0, GL_DEPTH_COMPONENT, 
				width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
		}
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	}

	const std::string& GLCubeMap::GetPath() const
	{
		return this->filepath;
	}

	size_t GLCubeMap::GetWidth() const
	{
		return this->width;
	}

	size_t GLCubeMap::GetHeight() const
	{
		return this->height;
	}

	size_t GLCubeMap::GetChannelCount() const
	{
		return this->channels;
	}
}