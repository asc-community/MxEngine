#pragma once

#include "Core/Interfaces/GraphicAPI/Texture.h"

namespace MxEngine
{
	class GLTexture final : public Texture
	{
		size_t width = 0, height = 0, channels = 0;
		mutable IBindable::IdType activeId = 0;
		#ifdef _DEBUG
		unsigned char* texture = nullptr;
		#endif	
		void FreeTexture();
	public:
		GLTexture();
		GLTexture(const GLTexture&) = delete;
		GLTexture(GLTexture&& texture);
		GLTexture(const std::string& filepath, bool genMipmaps = true, bool flipImage = true);
		~GLTexture();

        // Inherited via ITexture
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void Load(const std::string& filepath, bool genMipmaps = true, bool flipImage = true) override;
        virtual void Bind(IBindable::IdType id) const override;
        virtual size_t GetWidth() const override;
        virtual size_t GetHeight() const override;
        virtual size_t GetChannelCount() const override;
    };
}