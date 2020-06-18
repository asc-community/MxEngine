#pragma once

#include "Core/Components/Transform.h"
#include "Platform/GraphicAPI.h"
#include "Core/BoundingObjects/AABB.h"

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
        AABB boundingBox;

        GResource<VertexBuffer> VBO;
        GResource<VertexArray> VAO;
        GResource<IndexBuffer> IBO;
    public:
        MeshData();

        GResource<VertexArray> GetVAO() const;
        GResource<VertexBuffer> GetVBO() const;
        GResource<IndexBuffer> GetIBO() const;
        const AABB& GetAABB() const;

        VertexData& GetVertecies();
        const VertexData& GetVertecies() const;
        IndexData& GetIndicies();
        const IndexData& GetIndicies() const;
        
        void BufferVertecies(UsageType usageType = UsageType::STATIC_DRAW);
        void BufferIndicies();
        void UpdateBoundingBox();
        void RegenerateNormals();
    };
}