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
#include "Core/BoundingObjects/BoundingSphere.h"
#include "Vertex.h"

namespace MxEngine
{
    class MeshData
    {
        using VertexData = MxVector<Vertex>;
        using IndexData = MxVector<uint32_t>;
        VertexData vertecies;
        IndexData indicies;
        AABB boundingBox;
        BoundingSphere boundingSphere;

        VertexBufferHandle VBO;
        VertexArrayHandle VAO;
        IndexBufferHandle IBO;
    public:
        MeshData();

        VertexArrayHandle GetVAO() const;
        VertexBufferHandle GetVBO() const;
        IndexBufferHandle GetIBO() const;
        const AABB& GetAABB() const;
        const BoundingSphere& GetBoundingSphere() const;

        VertexData& GetVerteciesReference();
        const VertexData& GetVertecies() const;
        IndexData& GetIndiciesReference();
        const IndexData& GetIndicies() const;
        void SetVertecies(VertexData&& vertecies);
        void SetIndicies(IndexData&& indicies);
        void SetVertecies(const VertexData& vertecies);
        void SetIndicies(const IndexData& indicies);
        size_t GetVerteciesCount() const;
        size_t GetIndiciesCount() const;
        
        void BufferVertecies();
        void BufferVertecies(UsageType usageType);
        void BufferIndicies();
        void BufferIndicies(UsageType usageType);
        void FreeMeshDataCopy();
        void UpdateBoundingGeometry();
        void RegenerateNormals();
        void RegenerateTangentSpace();
    };
}