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
    public:
        using VertexData = MxVector<Vertex>;
        using IndexData = MxVector<uint32_t>;
    private:
        AABB boundingBox;
        BoundingSphere boundingSphere;

        VertexBufferHandle VBO;
        size_t vertexCount, vertexOffset;
        IndexBufferHandle IBO;
        size_t indexCount, indexOffset;
    public:
        MeshData(const VertexBufferHandle& VBO, size_t vertexCount, size_t vertexOffset, const IndexBufferHandle& IBO, size_t indexCount, size_t indexOffset);

        VertexBufferHandle GetVBO() const;
        IndexBufferHandle GetIBO() const;
        size_t GetVerteciesOffset() const;
        size_t GetIndiciesOffset() const;
        const AABB& GetAABB() const;
        const BoundingSphere& GetBoundingSphere() const;
        
        size_t GetVerteciesCount() const;
        size_t GetIndiciesCount() const;
        void BufferVertecies(const VertexData& vertecies);
        void BufferIndicies(const IndexData& indicies);
        void UpdateBoundingGeometry(const VertexData& vertecies);

        VertexData GetVerteciesFromGPU() const;
        IndexData GetIndiciesFromGPU() const;

        static void RegenerateNormals(VertexData& vertecies, const IndexData& indicies);
        static void RegenerateTangentSpace(VertexData& vertecies, const IndexData& indicies);
    };
}