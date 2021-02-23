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

#include "MeshData.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    MeshData::MeshData()
    {
        this->VBO = GraphicFactory::Create<VertexBuffer>(nullptr, 0, UsageType::STATIC_DRAW);
        this->IBO = GraphicFactory::Create<IndexBuffer>(nullptr, 0, UsageType::STATIC_DRAW);
        this->VAO = GraphicFactory::Create<VertexArray>();

        auto VBL = GraphicFactory::Create<VertexBufferLayout>();
        VBL->PushFloat(3); // position //-V525
        VBL->PushFloat(2); // texture
        VBL->PushFloat(3); // normal
        VBL->PushFloat(3); // tangent
        VBL->PushFloat(3); // bitangent

        this->VAO->AddBuffer(*this->VBO, *VBL);
    }

    VertexArrayHandle MeshData::GetVAO() const
    {
        return this->VAO;
    }

    VertexBufferHandle MeshData::GetVBO() const
    {
        return this->VBO;
    }

    IndexBufferHandle MeshData::GetIBO() const
    {
        return this->IBO;
    }

    const AABB& MeshData::GetAABB() const
    {
        return this->boundingBox;
    }

    const BoundingSphere& MeshData::GetBoundingSphere() const
    {
        return this->boundingSphere;
    }

    size_t MeshData::GetVerteciesCount() const
    {
        return this->GetVBO()->GetSize() / Vertex::Size;
    }

    size_t MeshData::GetIndiciesCount() const
    {
        return this->GetIBO()->GetSize();
    }

    void MeshData::BufferVertecies(const VertexData& vertecies)
    {
        this->BufferVertecies(vertecies, UsageType::STATIC_DRAW);
    }

    void MeshData::BufferVertecies(const VertexData& vertecies, UsageType usageType)
    {
        this->VBO->Load((const float*)vertecies.data(), vertecies.size() * Vertex::Size, usageType);
    }

    void MeshData::BufferIndicies(const IndexData& indicies)
    {
        this->BufferIndicies(indicies, UsageType::STATIC_DRAW);
    }

    void MeshData::BufferIndicies(const IndexData& indicies, UsageType usageType)
    {
        this->IBO->Load(indicies.data(), indicies.size(), usageType);
    }

    void MeshData::UpdateBoundingGeometry(const VertexData& vertecies)
    {
        this->boundingBox = { MakeVector3(0.0f), MakeVector3(0.0f) };
        if (vertecies.size() > 0)
        {
            this->boundingBox = { vertecies[0].Position, vertecies[0].Position };
            for (const auto& vertex : vertecies)
            {
                this->boundingBox.Min = VectorMin(this->boundingBox.Min, vertex.Position);
                this->boundingBox.Max = VectorMax(this->boundingBox.Max, vertex.Position);
            }
        }

        auto center = this->boundingBox.GetCenter();
        float maxRadius = 0.0f;
        for (const auto& vertex : vertecies)
        {
            auto distance = vertex.Position - center;
            maxRadius = Max(maxRadius, Length2(distance));
        }
        this->boundingSphere = BoundingSphere(center, std::sqrt(maxRadius));
    }

    MeshData::VertexData MeshData::GetVerteciesFromGPU() const
    {
        VertexData vertecies(this->GetVerteciesCount());
        this->VBO->GetBufferData((float*)vertecies.data(), vertecies.size() * Vertex::Size);
        return vertecies;
    }

    MeshData::IndexData MeshData::GetIndiciesFromGPU() const
    {
        IndexData indicies(this->GetIndiciesCount());
        this->IBO->GetBufferData(indicies.data(), indicies.size());
        return indicies;
    }

    void MeshData::RegenerateNormals(VertexData& vertecies, const IndexData& indicies)
    {
        // first set all normal-space vectors to 0
        for (auto& vertex : vertecies)
        {
            vertex.Normal    = MakeVector3(0.0f);
            vertex.Tangent   = MakeVector3(0.0f);
            vertex.Bitangent = MakeVector3(0.0f);
        }

        // then compute normal space vectors for each triangle
        for (size_t i = 0; i < indicies.size() / 3; i++)
        {
            std::array triangle = { // copy 3 vertecies
                vertecies[indicies[3 * i + 0]],
                vertecies[indicies[3 * i + 1]],
                vertecies[indicies[3 * i + 2]],
            };

            auto normal = ComputeNormal(triangle[0].Position, triangle[1].Position, triangle[2].Position);
            auto tanbitan = ComputeTangentSpace(
                triangle[0].Position, triangle[1].Position, triangle[2].Position,
                triangle[0].TexCoord, triangle[1].TexCoord, triangle[2].TexCoord
            );

            for (auto& vertex : triangle)
            {
                vertex.Normal += normal;
                vertex.Tangent += tanbitan[0];
                vertex.Bitangent += tanbitan[1];
            }
            
            for (size_t j = 0; j < triangle.size(); j++)
            {
                vertecies[indicies[3 * i + j]] = triangle[j]; // reassign copied vertecies
            }
        }

        // at the end normalize all normal-space vectors using vertex weights
        for (size_t i = 0; i < vertecies.size(); i++)
        {
            vertecies[i].Normal    = Normalize(vertecies[i].Normal);
            vertecies[i].Tangent   = Normalize(vertecies[i].Tangent);
            vertecies[i].Bitangent = Normalize(vertecies[i].Bitangent);
        }
    }

    // TODO: remove code dublication (see RegenerateNormals())
    void MeshData::RegenerateTangentSpace(VertexData& vertecies, const IndexData& indicies)
    {
        // first set all normal-space vectors to 0
        for (auto& vertex : vertecies)
        {
            vertex.Tangent = MakeVector3(0.0f);
            vertex.Bitangent = MakeVector3(0.0f);
        }

        // then compute normal space vectors for each triangle
        for (size_t i = 0; i < indicies.size() / 3; i++)
        {
            std::array triangle = { // copy 3 vertecies
                vertecies[indicies[3 * i + 0]],
                vertecies[indicies[3 * i + 1]],
                vertecies[indicies[3 * i + 2]],
            };

            auto tanbitan = ComputeTangentSpace(
                triangle[0].Position, triangle[1].Position, triangle[2].Position,
                triangle[0].TexCoord, triangle[1].TexCoord, triangle[2].TexCoord
            );

            for (auto& vertex : triangle)
            {
                vertex.Tangent += tanbitan[0];
                vertex.Bitangent += tanbitan[1];
            }

            for (size_t j = 0; j < triangle.size(); j++)
            {
                vertecies[indicies[3 * i + j]] = triangle[j]; // reassign copied vertecies
            }
        }

        // at the end normalize all normal-space vectors using vertex weights
        for (size_t i = 0; i < vertecies.size(); i++)
        {
            vertecies[i].Tangent = Normalize(vertecies[i].Tangent);
            vertecies[i].Bitangent = Normalize(vertecies[i].Bitangent);
        }
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<Vertex>("Vertex")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<Vertex>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("position", &Vertex::Position)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("tex coord", &Vertex::TexCoord)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("normal", &Vertex::Normal)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("tangent", &Vertex::Tangent)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("bitangent", &Vertex::Bitangent)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );

        rttr::registration::class_<MeshData>("MeshData")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<MeshData>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property_readonly("vertecies count", &MeshData::GetVerteciesCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("indicies count", &MeshData::GetIndiciesCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("aabb", &MeshData::GetAABB)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("bounding sphere", &MeshData::GetBoundingSphere)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            );
    }
}