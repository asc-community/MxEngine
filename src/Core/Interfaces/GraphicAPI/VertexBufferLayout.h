#pragma once

#include "Core/Interfaces/GraphicAPI/IBindable.h"
#include <vector>

namespace MxEngine
{
    struct VertexBufferLayout
    {
        struct VertexBufferElement
        {
            unsigned int count;
            unsigned int type;
            unsigned char normalized;
        };

        using StrideType = unsigned int;
        using ElementBuffer = std::vector<VertexBufferElement>;

        virtual const ElementBuffer& GetElements() const = 0;
        virtual StrideType GetStride() const = 0;
        virtual void PushFloat(size_t count) = 0;
    };
}