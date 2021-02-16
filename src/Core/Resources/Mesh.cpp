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
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Utilities/Profiler/Profiler.h"
#include "Platform/GraphicAPI.h"
#include "Utilities/LODGenerator/LODGenerator.h"
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
		this->filePath = ToMxString(filepath);
		ObjectInfo objectInfo = ObjectLoader::Load(filepath);

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

		for (size_t i = 0; i < objectInfo.meshes.size(); i++)
		{
			auto& meshData = objectInfo.meshes[i];
			auto& materialId = materialIds[i];

			auto& submesh = this->AddSubMesh(materialId);
			submesh.Data.BufferVertecies(meshData.vertecies);
			submesh.Data.BufferIndicies(meshData.indicies);
			submesh.Data.UpdateBoundingGeometry(meshData.vertecies);
			submesh.Name = std::move(meshData.name);
		}
		this->UpdateBoundingGeometry(); // use submeshes boundings to update mesh boundings
	}

	template<>
    Mesh::Mesh(const std::filesystem::path& path)
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

	size_t Mesh::AddInstancedBuffer(VertexBufferHandle vbo, VertexBufferLayoutHandle vbl)
	{
		this->VBOs.push_back(std::move(vbo));
		this->VBLs.push_back(std::move(vbl));
		for (auto& mesh : this->GetSubMeshes())
		{
			mesh.Data.GetVAO()->AddInstancedBuffer(*this->VBOs.back(), *this->VBLs.back());
		}
		return this->VBOs.size() - 1;
	}

	VertexBufferHandle Mesh::GetBufferByIndex(size_t index) const
	{
		MX_ASSERT(index < this->VBOs.size());
		return this->VBOs[index];
	}

	VertexBufferLayoutHandle Mesh::GetBufferLayoutByIndex(size_t index) const
	{
		MX_ASSERT(index < this->VBLs.size());
		return this->VBLs[index];
	}

	size_t Mesh::GetTotalVerteciesCount() const
	{
		size_t result = 0;
		for (auto& mesh : this->GetSubMeshes())
		{
			result += mesh.Data.GetVBO()->GetSize() / Vertex::Size;
		}
		return result;
	}

	size_t Mesh::GetTotalIndiciesCount() const
	{
		size_t result = 0;
		for (auto& mesh : this->GetSubMeshes())
		{
			result += mesh.Data.GetIBO()->GetCount();
		}
		return result;
	}

	size_t Mesh::GetBufferCount() const
    {
		return this->VBOs.size();
    }

    void Mesh::PopInstancedBuffer()
    {
		MX_ASSERT(!this->VBOs.empty());
		for (auto& mesh : this->GetSubMeshes())
		{
			mesh.Data.GetVAO()->PopBuffer(*this->VBLs.back());
		}
		this->VBOs.pop_back();
		this->VBLs.pop_back();
    }

	const MxString& Mesh::GetFilePath() const
	{
		return this->filePath;
	}

	void Mesh::SetInternalEngineTag(const MxString& tag)
	{
		this->filePath = tag;
	}

	bool Mesh::IsInternalEngineResource() const
	{
		return this->filePath.find(MXENGINE_INTERNAL_TAG_SYMBOL) == 0;
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

	SubMesh& Mesh::AddSubMesh(SubMesh::MaterialId materialId)
	{
		auto& transform = *this->subMeshTransforms.emplace_back(MakeUnique<TransformComponent>());
		return this->submeshes.emplace_back(materialId, transform);
	}

	SubMesh& Mesh::LinkSubMesh(SubMesh& submesh)
	{
		// ALL submeshes in mesh should be linked, in any is linked
		MX_ASSERT(this->subMeshTransforms.empty());
		return this->submeshes.emplace_back(submesh.GetMaterialId(), submesh.GetTransformReference());
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
			.property("filepath", &Mesh::GetFilePath, (SetFilePath)&Mesh::Load)
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