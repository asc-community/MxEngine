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

#include "Mesh.h"
#include "Utilities/ObjectLoading/ObjectLoader.h"
#include "Utilities/Profiler/Profiler.h"
#include "Platform/GraphicAPI.h"
#include "Utilities/Format/Format.h"
#include "Core/Resources/AssetManager.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Runtime/Reflection.h"

#include <algorithm>

namespace MxEngine
{
    template<>
    void Mesh::LoadFromFile(const std::filesystem::path& filepath)
    {
        ObjectInfo objectInfo = ObjectLoader::Load(filepath);

        this->filepath = ToMxString(filepath);
        std::replace(this->filepath.begin(), this->filepath.end(), '\\', '/');

        MxVector<SubMesh::MaterialId> materialIds;
        materialIds.reserve(objectInfo.meshes.size());
        for (const auto& group : objectInfo.meshes)
        {
            if (group.useTexture && group.material != nullptr)
            {
                auto offset = size_t(group.material - objectInfo.materials.data());
                materialIds.push_back(offset);
            }
            else
            {
                materialIds.push_back(std::numeric_limits<SubMesh::MaterialId>::max());
            }
        }

        // dump all material to let user retrieve them for MeshRenderer component
        FilePath materialLibPath = filepath.native() + MeshRenderer::GetMaterialFileExtenstion().native();
        ObjectLoader::DumpMaterials(objectInfo.materials, materialLibPath);

        // optimize transform additions
        this->subMeshTransforms.reserve(objectInfo.meshes.size());

        // precompute and allocate size for per-mesh Vertex Buffer and Index Buffer
        size_t totalVerticies = 0;
        size_t totalIndicies = 0;
        for (const auto& meshInfo : objectInfo.meshes)
        {
            totalVerticies += meshInfo.vertecies.size();
            totalIndicies += meshInfo.indicies.size();
        }
        // create CPU-side array for verticies and indicies, and GPU-size VBO/IBO
        MxVector<Vertex> verticies;
        MxVector<IndexBuffer::IndexType> indicies;
        verticies.reserve(totalVerticies);
        indicies.reserve(totalIndicies);
        this->ReserveData(totalVerticies, totalIndicies);

        // insert all verticies and indicies into single VBO/IBO
        for (size_t i = 0; i < objectInfo.meshes.size(); i++)
        {
            auto& meshInfo = objectInfo.meshes[i];
            auto& materialId = materialIds[i];
            
            MeshData meshData{ 
                this->VBO, meshInfo.vertecies.size(), verticies.size(),
                this->IBO, meshInfo.indicies.size(), indicies.size()
            };
            meshData.UpdateBoundingGeometry(meshInfo.vertecies);

            // apply vertex offset to each index
            for (const auto& index : meshInfo.indicies)
                indicies.push_back(index + verticies.size());
            // no additional operations for verticies
            verticies.insert(verticies.end(), meshInfo.vertecies.begin(), meshInfo.vertecies.end());

            this->AddSubMesh(materialId, std::move(meshData));
        }
        // load verticies and indicies to GPU
        this->VBO->BufferSubData((float*)verticies.data(), verticies.size() * Vertex::Size);
        this->IBO->BufferSubData(indicies.data(), indicies.size());

        this->UpdateBoundingGeometry(); // use submeshes boundings to update mesh boundings
    }

    Mesh::Mesh()
    {
        this->filepath = MXENGINE_MAKE_INTERNAL_TAG("empty");
        this->VBO = Factory<VertexBuffer>::Create(nullptr, 0, UsageType::STATIC_DRAW);
        this->IBO = Factory<IndexBuffer>::Create(nullptr, 0, UsageType::STATIC_DRAW);
        this->VAO = Factory<VertexArray>::Create();

        std::array vertexLayout = {
            VertexLayout::Entry<Vector3>(), // position
            VertexLayout::Entry<Vector2>(), // texture uv
            VertexLayout::Entry<Vector3>(), // normal
            VertexLayout::Entry<Vector3>(), // tangent
            VertexLayout::Entry<Vector3>(), // bitangent
        };
        this->VAO->AddVertexBuffer(*this->VBO, vertexLayout);
        this->VAO->LinkIndexBuffer(*this->IBO);
    }

    template<>
    Mesh::Mesh(const std::filesystem::path& path)
        : Mesh()
    {
        this->LoadFromFile(std::filesystem::proximate(path));
    }

    template<>
    void Mesh::Load(const std::filesystem::path& filepath)
    {
        this->LoadFromFile(std::filesystem::proximate(filepath));
    }

    void Mesh::Load(const MxString& filepath)
    {
        this->Load(ToFilePath(filepath));
    }

    void Mesh::ReserveData(size_t vertexCount, size_t indexCount)
    {
        this->VBO->Load(nullptr, vertexCount * Vertex::Size, UsageType::STATIC_DRAW);
        this->IBO->Load(nullptr, indexCount, UsageType::STATIC_DRAW);
    }

    void Mesh::UpdateBoundingGeometry()
    {
        // compute bounding box, taking min and max points from each sub-box
        this->MeshAABB = { MakeVector3(0.0f), MakeVector3(0.0f) };
        if (!this->GetSubMeshes().empty()) 
            this->MeshAABB = this->GetSubMeshByIndex(0).Data.GetAABB();

        for (const auto& submesh : this->GetSubMeshes())
        {
            this->MeshAABB.Min = VectorMin(this->MeshAABB.Min, submesh.Data.GetAABB().Min);
            this->MeshAABB.Max = VectorMax(this->MeshAABB.Max, submesh.Data.GetAABB().Max);
        }

        // compute bounding sphere, taking sum of max sub-sphere radius and distance to it
        auto center = MakeVector3(0.0f);
        auto maxRadius = 0.0f;
        for (const auto& submesh : this->GetSubMeshes())
        {
            auto sphere = submesh.Data.GetBoundingSphere();
            auto distanceToCenter = Length(sphere.Center);
            maxRadius = Max(maxRadius, distanceToCenter + sphere.Radius);
        }
        this->MeshBoundingSphere = MxEngine::BoundingSphere(center, maxRadius);
    }

    size_t Mesh::AddInstancedBuffer(VertexBufferHandle vbo, ArrayView<VertexLayout> layout)
    {
        this->instancedVBOs.push_back(std::move(vbo));
        this->instancedVBLs.emplace_back(layout.begin(), layout.end());
        this->VAO->AddInstancedVertexBuffer(*this->instancedVBOs.back(), this->instancedVBLs.back());
        return this->instancedVBOs.size() - 1;
    }

    VertexBufferHandle Mesh::GetBufferByIndex(size_t index) const
    {
        MX_ASSERT(index < this->instancedVBOs.size());
        return this->instancedVBOs[index];
    }

    const MxVector<VertexLayout>& Mesh::GetBufferLayoutByIndex(size_t index) const
    {
        MX_ASSERT(index < this->instancedVBLs.size());
        return this->instancedVBLs[index];
    }

    VertexBufferHandle Mesh::GetVBO() const
    {
        return this->VBO;
    }

    IndexBufferHandle Mesh::GetIBO() const
    {
        return this->IBO;
    }

    VertexArrayHandle Mesh::GetVAO() const
    {
        return this->VAO;
    }

    size_t Mesh::GetTotalVerteciesCount() const
    {
        return this->VBO->GetSize() / Vertex::Size;
    }

    size_t Mesh::GetTotalIndiciesCount() const
    {
        return this->IBO->GetSize();
    }

    size_t Mesh::GetInstancedBufferCount() const
    {
        return this->instancedVBOs.size();
    }

    void Mesh::PopInstancedBuffer()
    {
        MX_ASSERT(!this->instancedVBOs.empty());
        this->VAO->PopBuffer(this->instancedVBLs.back());
        this->instancedVBOs.pop_back();
        this->instancedVBLs.pop_back();
    }

    const MxString& Mesh::GetFilePath() const
    {
        return this->filepath;
    }

    void Mesh::SetInternalEngineTag(const MxString& tag)
    {
        this->filepath = tag;
    }

    bool Mesh::IsInternalEngineResource() const
    {
        return this->filepath.find(MXENGINE_INTERNAL_TAG_SYMBOL) == 0;
    }

    void Mesh::SetSubMeshesInternal(const SubMeshList& submeshes)
    {
        this->submeshes = submeshes;
    }

    const Mesh::SubMeshList& Mesh::GetSubMeshes() const
    {
        return this->submeshes;
    }

    const SubMesh& Mesh::GetSubMeshByIndex(size_t index) const
    {
        MX_ASSERT(index < this->submeshes.size());
        return this->submeshes[index];
    }

    SubMesh& Mesh::GetSubMeshByIndex(size_t index)
    {
        MX_ASSERT(index < this->submeshes.size());
        return this->submeshes[index];
    }

    SubMesh& Mesh::AddSubMesh(SubMesh::MaterialId materialId, MeshData data)
    {
        auto& transform = *this->subMeshTransforms.emplace_back(MakeUnique<TransformComponent>());
        return this->submeshes.emplace_back(materialId, transform, std::move(data));
    }

    void Mesh::DeleteSubMeshByIndex(size_t index)
    {
        // you cannot delete linkes meshes
        MX_ASSERT(!this->subMeshTransforms.empty());

        this->submeshes.erase(this->submeshes.begin() + index);
        this->subMeshTransforms.erase(this->subMeshTransforms.begin() + index);
    }

    namespace GUI
    {
        void MeshEditorExtra(rttr::instance&);
        rttr::variant MeshHandleEditorExtra(rttr::instance&);
    }

    void StubSetSubMeshes(Mesh*, const MxVector<SubMesh>&) { }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<AABB>("AABB")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<AABB>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("min", &AABB::Min)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max", &AABB::Max)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );
        
        using SetFilePath = void(Mesh::*)(const MxString&);

        rttr::registration::class_<Mesh>("Mesh")
            (
                rttr::metadata(EditorInfo::HANDLE_EDITOR, GUI::HandleEditorExtra<Mesh>)
            )
            .constructor<>()
            .property_readonly("filepath", &Mesh::GetFilePath)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("_filepath", &Mesh::GetFilePath, (SetFilePath)&Mesh::Load)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE)
            )
            .method("update bounding geometry", &Mesh::UpdateBoundingGeometry)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("total vertecies count", &Mesh::GetTotalVerteciesCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("total indicies count", &Mesh::GetTotalIndiciesCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("aabb", &Mesh::MeshAABB)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("bounding sphere", &Mesh::MeshBoundingSphere)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("submeshes", &Mesh::GetSubMeshes, &Mesh::SetSubMeshesInternal)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            );
    }
}