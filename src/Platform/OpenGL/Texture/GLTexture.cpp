#include "GLTexture.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"
#include "Utilities/ImageLoader/ImageLoader.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Time/Time.h"

void MxEngine::GLTexture::FreeTexture()
{
	if (id != 0)
	{
		GLCALL(glDeleteTextures(1, &id));
		#ifdef _DEBUG
		if (texture != nullptr)
			ImageLoader::FreeImage(texture);
		#endif
	}
}

MxEngine::GLTexture::GLTexture()
{
	this->id = 0;
}

MxEngine::GLTexture::GLTexture(GLTexture&& texture)
	: width(texture.width), height(texture.height), channels(texture.channels)
{
	this->id = texture.id;
	texture.id = 0;
	#ifdef _DEBUG
	this->texture = texture.texture;
	texture.texture = nullptr;
	#endif
}

MxEngine::GLTexture::GLTexture(const std::string& filepath, bool genMipmaps, bool flipImage)
{
	Load(filepath, genMipmaps, flipImage);
}

MxEngine::GLTexture::~GLTexture()
{
	this->FreeTexture();
}

void MxEngine::GLTexture::Load(const std::string& filepath, bool genMipmaps, bool flipImage)
{
	this->FreeTexture();

	GLCALL(glGenTextures(1, &id));

	Image image = ImageLoader::LoadImage(filepath, flipImage);

	if (image.data == nullptr)
	{
		Logger::Instance().Error("MxEngine::Texture", "file with name '" + filepath + "' was not found");
		return;
	}
	this->width = image.width;
	this->height = image.height;
	this->channels = image.channels;

	GLCALL(glBindTexture(GL_TEXTURE_2D, id));
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data));
	if (genMipmaps)
	{
		GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
	}

	#ifdef _DEBUG
	this->texture = image.data;
	#else
	ImageLoader::FreeImage(image);
	#endif
}

void MxEngine::GLTexture::Bind() const
{
	GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
	GLCALL(glBindTexture(GL_TEXTURE_2D, id));
}

void MxEngine::GLTexture::Unbind() const
{
	GLCALL(glActiveTexture(GL_TEXTURE0 + this->activeId));
	GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void MxEngine::GLTexture::Bind(IBindable::IdType id) const
{
	this->activeId = id;
	this->Bind();
}

size_t MxEngine::GLTexture::GetWidth() const
{
	return width;
}

size_t MxEngine::GLTexture::GetHeight() const
{
	return height;
}

size_t MxEngine::GLTexture::GetChannelCount() const
{
	return channels;
}