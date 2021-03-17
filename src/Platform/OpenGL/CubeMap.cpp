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

#include "CubeMap.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Image/ImageLoader.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/FileSystem/File.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
	void CubeMap::FreeCubeMap()
	{
		if (id != 0)
		{
			GLCALL(glDeleteTextures(1, &id));
			MXLOG_DEBUG("OpenGL::CubeMap", "deleted cubemap with id = " + ToMxString(id));
		}
		id = 0;
		activeId = 0;
	}

	CubeMap::CubeMap()
	{
		GLCALL(glGenTextures(1, &id));
		MXLOG_DEBUG("OpenGL::CubeMap", "created cubemap with id = " + ToMxString(id));
	}

	template<>
	void CubeMap::Load(const std::filesystem::path& filepath)
	{
		// TODO: support floating point textures
		bool flipImage = false;
		Image img = ImageLoader::LoadImage(filepath, flipImage);
		if (img.GetRawData() == nullptr)
		{
			MXLOG_WARNING("OpenGL::CubeMap", "file with name '" + ToMxString(filepath) + "' was not found");
			return;
		}
		auto images = ImageLoader::CreateCubemap(img);

		this->filepath = ToMxString(std::filesystem::proximate(filepath));
		std::replace(this->filepath.begin(), this->filepath.end(), '\\', '/');

		this->channels = img.GetChannelCount();
		this->width = img.GetWidth();
		this->height = img.GetHeight();

		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
		for (size_t i = 0; i < 6; i++)
		{
			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)i, 0, GL_RGB,
				(GLsizei)images[i].width(), (GLsizei)images[i].height() / (GLsizei)this->channels, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i].data()));
		}

		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

		this->GenerateMipmaps();
	}

	void CubeMap::Load(const MxString& filepath)
	{
		this->Load(ToFilePath(filepath));
	}

	template<>
    CubeMap::CubeMap(const std::filesystem::path& filepath)
		: CubeMap()
    {
		this->Load(filepath);
    }

	CubeMap::CubeMap(CubeMap&& other) noexcept
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

	CubeMap& CubeMap::operator=(CubeMap&& other) noexcept
	{
		this->FreeCubeMap();

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
		return *this;
	}

	CubeMap::~CubeMap()
	{
		this->FreeCubeMap();
	}

	void CubeMap::Bind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
	}

	void CubeMap::Unbind() const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}

	CubeMap::BindableId CubeMap::GetNativeHandle() const
    {
		return id;
    }

	void CubeMap::Bind(CubeMapBindId id) const
	{
		this->activeId = id;
		this->Bind();
	}

	CubeMap::CubeMapBindId CubeMap::GetBoundId() const
	{
		return this->activeId;
	}

	template<>
    void CubeMap::Load(const std::filesystem::path& right, const std::filesystem::path& left, const std::filesystem::path& top,
		               const std::filesystem::path& bottom, const std::filesystem::path& front, const std::filesystem::path& back)
    {
		bool flipImage = true;
		std::array<Image, 6> images =
		{
			ImageLoader::LoadImage(right, flipImage),
			ImageLoader::LoadImage(left, flipImage),
			ImageLoader::LoadImage(top, flipImage),
			ImageLoader::LoadImage(bottom, flipImage),
			ImageLoader::LoadImage(front, flipImage),
			ImageLoader::LoadImage(back, flipImage),
		};
		this->Load(images);
    }

	void CubeMap::Load(const std::array<Image, 6>& images, bool genMipmaps)
	{
		this->width = images.front().GetWidth();
		this->height = images.front().GetHeight();
		this->channels = images.front().GetChannelCount();
		this->filepath = MXENGINE_MAKE_INTERNAL_TAG("raw");

		GLenum pixelType = images.front().IsFloatingPoint() ? GL_FLOAT : GL_UNSIGNED_BYTE;
		GLenum pixelFormat = GL_RGBA;
		switch (this->channels)
		{
		case 1:
			pixelFormat = GL_RED;
			break;
		case 2:
			pixelFormat = GL_RG;
			break;
		case 3:
			pixelFormat = GL_RGB;
			break;
		case 4:
			pixelFormat = GL_RGBA;
			break;
		default:
			MXLOG_ERROR("OpenGL::Texture", "invalid channel count: " + ToMxString(this->channels));
			break;
		}

		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
		for (size_t i = 0; i < images.size(); i++)
		{
			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)i, 0, GL_RGB,
				(GLsizei)this->width, (GLsizei)this->height, 0, pixelFormat, pixelType, images[i].GetRawData()));
		}

		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
		
		if (genMipmaps)
		{
			this->GenerateMipmaps();
		}
	}

    void CubeMap::Load(const std::array<uint8_t*, 6>& data, size_t width, size_t height)
    {
		this->width = width;
		this->height = height;
		this->channels = 3;
		this->filepath = MXENGINE_MAKE_INTERNAL_TAG("raw");

		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
		for (size_t i = 0; i < data.size(); i++)
		{
			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)i, 0, GL_RGB,
				(GLsizei)this->width, (GLsizei)this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]));
		}

		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

		this->GenerateMipmaps();
    }

	void CubeMap::LoadDepth(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->filepath = MXENGINE_MAKE_INTERNAL_TAG("depth");
		this->channels = 1;
		
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
		for (size_t i = 0; i < 6; i++)
		{
			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)i, 0, GL_DEPTH_COMPONENT, 
				width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
		}

		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));

		float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLCALL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border));
	}

	const MxString& CubeMap::GetFilePath() const
	{
		return this->filepath;
	}

	void CubeMap::SetInternalEngineTag(const MxString& tag)
	{
		this->filepath = tag;
	}

	bool CubeMap::IsInternalEngineResource() const
	{
		return this->filepath.find(MXENGINE_INTERNAL_TAG_SYMBOL) == 0;
	}

	size_t CubeMap::GetWidth() const
	{
		return this->width;
	}

	size_t CubeMap::GetHeight() const
	{
		return this->height;
	}

	size_t CubeMap::GetChannelCount() const
	{
		return this->channels;
	}

	void CubeMap::GenerateMipmaps()
	{
		this->Bind(0);
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
	}

	MXENGINE_REFLECT_TYPE
	{
		using SetFilePath = void(CubeMap::*)(const MxString&);

		rttr::registration::class_<CubeMap>("CubeMap")
			(
				rttr::metadata(EditorInfo::HANDLE_EDITOR, GUI::HandleEditorExtra<CubeMap>)
			)
			.constructor<>()
			.property_readonly("filepath", &CubeMap::GetFilePath)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
			)
			.property("_filepath", &CubeMap::GetFilePath, (SetFilePath)&CubeMap::Load)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE)
			)
			.property_readonly("width", &CubeMap::GetWidth)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
			)
			.property_readonly("height", &CubeMap::GetHeight)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
			)
			.property_readonly("channel count", &CubeMap::GetChannelCount)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
			)
			.property_readonly("native handle", &CubeMap::GetNativeHandle)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
			)
			.property_readonly("editor-preview", &CubeMap::GetBoundId)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::CUSTOM_VIEW, GUI::EditorExtra<CubeMap>)
			);
	}
}