#pragma once

#include "Core/Interfaces/GraphicAPI/IBindable.h"

#include <vector>

namespace MomoEngine
{
    struct IndexBuffer : IBindable
    {
    public:
        using IndexType = unsigned int;
        using IndexBufferType = std::vector<IndexType>;

        virtual void Load(const IndexBufferType& data) = 0;
        virtual size_t GetCount() const = 0;
        virtual size_t GetIndexTypeId() const = 0;
    };
}