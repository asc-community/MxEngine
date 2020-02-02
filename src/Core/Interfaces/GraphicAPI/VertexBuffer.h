#pragma once

#include "Core/Interfaces/GraphicAPI/IBindable.h"
#include <vector>

namespace MomoEngine
{
    enum class UsageType
    {
        STREAM_DRAW = 0x88E0,
        STREAM_READ = 0x88E1,
        STREAM_COPY = 0x88E2,
        STATIC_DRAW = 0x88E4,
        STATIC_READ = 0x88E5,
        STATIC_COPY = 0x88E6,
        DYNAMIC_DRAW = 0x88E8,
        DYNAMIC_READ = 0x88E9,
        DYNAMIC_COPY = 0x88EA,
    };

    struct VertexBuffer : IBindable
    {
        using BufferData = std::vector<float>;
        virtual void Load(const BufferData& data, UsageType type) = 0;
    };
}