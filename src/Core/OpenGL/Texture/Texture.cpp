#include "Texture.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"
#include "Utilities/ImageLoader/ImageLoader.h"
#include "Utilities/Logger/Logger.h"

void MomoEngine::Texture::FreeTexture()
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

MomoEngine::Texture::Texture()
{
	this->id = 0;
}

MomoEngine::Texture::Texture(const std::string& filepath, bool genMipmaps, bool flipImage)
{
	Load(filepath, genMipmaps, flipImage);
}

MomoEngine::Texture::~Texture()
{
	this->FreeTexture();
}

void MomoEngine::Texture::Load(const std::string& filepath, bool genMipmaps, bool flipImage)
{
	this->FreeTexture();

	GLCALL(glGenTextures(1, &id));

	Image image = ImageLoader::LoadImage(filepath, flipImage);

	if (image.data == nullptr)
	{
		Logger::Instance().Error("MomoEngine::Texture", "file with name '" + filepath + "' was not found");
		return;
	}
	this->width = image.width;
	this->height = image.height;
	this->channels = image.channels;

	GLCALL(glBindTexture(GL_TEXTURE_2D, id));
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data));
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

void MomoEngine::Texture::Bind() const
{
	GLCALL(glBindTexture(GL_TEXTURE_2D, id));
}

void MomoEngine::Texture::Unbind() const
{
	GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}

int MomoEngine::Texture::GetWidth() const
{
	return width;
}

int MomoEngine::Texture::GetHeight() const
{
	return height;
}

int MomoEngine::Texture::GetChannelCount() const
{
	return channels;
}