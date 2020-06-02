#pragma once

#include "Core/Components/Transform.h"
#include "Platform/GraphicAPI.h"

namespace MxEngine
{
    struct Vertex
    {
        Vector3 Position{ 0.0f };
        Vector2 TexCoord{ 0.0f };
        Vector3 Normal{ 0.0f };
        Vector3 Tangent{ 0.0f };
        Vector3 Bitangent{ 0.0f };

        constexpr static size_t Size = 3 + 2 + 3 + 3 + 3;
    };

    class MeshData
    {
        using VertexData = MxVector<Vertex>;
        using IndexData = MxVector<uint32_t>;
        VertexData vertecies;
        IndexData indicies;

        GResource<VertexBuffer> VBO;
        GResource<VertexArray> VAO;
        GResource<IndexBuffer> IBO;
    public:
        MeshData();

        VertexArray& GetVAO() const;
        VertexBuffer& GetVBO() const;
        IndexBuffer& GetIBO() const;

        VertexData& GetVertecies();
        const VertexData& GetVertecies() const;
        IndexData& GetIndicies();
        const IndexData& GetIndicies() const;
        
        void BufferVertecies(UsageType usageType = UsageType::STATIC_DRAW);
        void BufferIndicies();
        void RegenerateNormals();
    };
}