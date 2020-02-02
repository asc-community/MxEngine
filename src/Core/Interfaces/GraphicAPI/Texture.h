#pragma once

#include "Core/Interfaces/GraphicAPI/IBindable.h"
#include <string>

namespace MomoEngine
{
    struct Texture : IBindable
    {
        virtual void Load(const std::string& filepath, bool genMipmaps = true, bool flipImage = true) = 0;
        virtual void Bind(IBindable::IdType id) const = 0;

        virtual size_t GetWidth() const = 0;
        virtual size_t GetHeight() const = 0;
        virtual size_t GetChannelCount() const = 0;
    };
}