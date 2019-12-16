#pragma once

#include "IBindable.h"
#include <string>

namespace MomoEngine
{
	class Texture : IBindable
	{
		int width = 0, height = 0, channels = 0;
		#ifdef _DEBUG
		unsigned char* texture = nullptr;
		#endif	
	public:
		Texture(const std::string& filepath, bool genMipmaps = true, bool flipImage = true);
		~Texture();

		// Inherited via IBindable
		virtual void Bind() const override;
		virtual void Unbind() const override;

		int GetWidth() const;
		int GetHeight() const;
		int GetChannelCount() const;
	};
}