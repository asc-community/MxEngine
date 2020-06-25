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
#include "Core/Resources/ResourceFactory.h"
#include "Core/Components/Rendering/MeshRenderer.h"

#include <algorithm>

namespace MxEngine
{
	void Mesh::LoadFromFile(const MxString& filepath)
	{
		ObjectInfo objectInfo = ObjectLoader::Load(filepath);
		MxVector<Transform::Handle> submeshTransforms;

		for (const auto& group : objectInfo.meshes)
		{
			submeshTransforms.push_back(ComponentFactory::CreateComponent<Transform>());
		}

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

		if (!objectInfo.materials.empty())
		{
			// dump all material to let user retrieve them for MeshRenderer component
			auto materialLibPath = filepath + MeshRenderer::GetMaterialFileSuffix();
			if (!File::Exists(materialLibPath))
				ObjectLoader::DumpMaterials(objectInfo.materials, materialLibPath);
		}
		
		for (size_t i = 0; i < objectInfo.meshes.size(); i++)
		{
			auto& meshData = objectInfo.meshes[i];
			auto& materialId = materialIds[i];
			auto& transform = submeshTransforms[i];

			SubMesh submesh(materialId, transform);
			submesh.MeshData.GetVertecies() = std::move(meshData.vertecies);
			submesh.MeshData.GetIndicies() = std::move(meshData.indicies);
			submesh.MeshData.BufferVertecies();
			submesh.MeshData.BufferIndicies();
			submesh.MeshData.UpdateBoundingBox();
			submesh.Name = std::move(meshData.name);

			submeshes.push_back(std::move(submesh));
		}
		this->UpdateAABB(); // use submeshes AABB to update mesh bounding box
	}

    Mesh::Mesh(const MxString& path)
    {
		this->LoadFromFile(path);
    }

	void Mesh::Load(const MxString& filepath)
	{
		this->LoadFromFile(filepath);
	}

	Mesh::SubmeshList& Mesh::GetSubmeshes()
	{
		return this->submeshes;
	}

	const Mesh::SubmeshList& Mesh::GetSubmeshes() const
	{
		return this->submeshes;
	}

	const AABB& Mesh::GetAABB() const
	{
		return this->boundingBox;
	}

	void Mesh::SetAABB(const AABB& boundingBox)
	{
		this->boundingBox = boundingBox;
	}

	void Mesh::UpdateAABB()
	{
		this->boundingBox = { MakeVector3(0.0f), MakeVector3(0.0f) };
		if (!this->submeshes.empty()) 
			this->boundingBox = this->submeshes.front().MeshData.GetAABB();

		for (const auto& submesh : this->submeshes)
		{
			this->boundingBox.Min = VectorMin(this->boundingBox.Min, submesh.MeshData.GetAABB().Min);
			this->boundingBox.Max = VectorMax(this->boundingBox.Max, submesh.MeshData.GetAABB().Max);
		}
	}

	size_t Mesh::AddInstancedBuffer(GResource<VertexBuffer> vbo, GResource<VertexBufferLayout> vbl)
	{
		this->VBOs.push_back(std::move(vbo));
		this->VBLs.push_back(std::move(vbl));
		for (auto& mesh : submeshes)
		{
			mesh.MeshData.GetVAO()->AddInstancedBuffer(*this->VBOs.back(), *this->VBLs.back());
		}
		return this->VBOs.size() - 1;
	}

	GResource<VertexBuffer> Mesh::GetBufferByIndex(size_t index) const
	{
		MX_ASSERT(index < this->VBOs.size());
		return this->VBOs[index];
	}

	GResource<VertexBufferLayout> Mesh::GetBufferLayoutByIndex(size_t index) const
	{
		MX_ASSERT(index < this->VBLs.size());
		return this->VBLs[index];
	}

    size_t Mesh::GetBufferCount() const
    {
		return this->VBOs.size();
    }

    void Mesh::PopInstancedBuffer()
    {
		MX_ASSERT(!this->VBOs.empty());
		for (auto& mesh : submeshes)
		{
			mesh.MeshData.GetVAO()->PopBuffer(*this->VBLs.back());
		}
		this->VBOs.pop_back();
		this->VBLs.pop_back();
    }
}