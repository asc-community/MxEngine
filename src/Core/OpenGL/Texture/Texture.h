#pragma once

#include "Core/Interfaces/IBindable.h"
#include <string>

namespace MomoEngine
{
	class Texture : IBindable
	{
		int width = 0, height = 0, channels = 0;
		mutable int activeId = 0;
		#ifdef _DEBUG
		unsigned char* texture = nullptr;
		#endif	
		void FreeTexture();
	public:
		Texture();
		Texture(const Texture&) = delete;
		Texture(Texture&& texture);
		Texture(const std::string& filepath, bool genMipmaps = true, bool flipImage = true);
		~Texture();

		void Load(const std::string& filepath, bool genMipmaps = true, bool flipImage = true);

		// Inherited via IBindable
		virtual void Bind() const override;
		virtual void Unbind() const override;

		void Bind(int id) const;

		int GetWidth() const;
		int GetHeight() const;
		int GetChannelCount() const;
	};
}