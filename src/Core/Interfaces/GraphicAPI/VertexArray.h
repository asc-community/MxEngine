#pragma once

#include "Core/Interfaces/GraphicAPI/IBindable.h"
#include "Core/Interfaces/GraphicAPI/VertexBuffer.h"
#include "Core/Interfaces/GraphicAPI/VertexBufferLayout.h"

namespace MxEngine
{
    struct VertexArray : IBindable
    {
        virtual void AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout) = 0;
        virtual void AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout) = 0;
    };
}