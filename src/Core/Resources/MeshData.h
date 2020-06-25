// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
        void FreeMeshDataCopy();
        void BufferIndicies();
        void UpdateBoundingBox();
        void RegenerateNormals();
        void RegenerateTangentSpace();
    };
}