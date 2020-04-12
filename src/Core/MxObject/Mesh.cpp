// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"

#include <algorithm>

namespace MxEngine
{
	void Mesh::LoadFromFile(const std::string& filepath)
	{
		ObjectInfo objectInfo;
		{
			MAKE_SCOPE_TIMER("MxEngine::MxObject", "ObjectLoader::LoadFromFile()");
			objectInfo = ObjectLoader::Load(filepath);
		}

		MAKE_SCOPE_PROFILER("Object::GenBuffers");
		MAKE_SCOPE_TIMER("MxEngine::MxObject", "Object::GenBuffers()");
		if (!objectInfo.isSuccess)
		{
			Logger::Instance().Debug("MxEngine::MxObject", "failed to load object from file: " + filepath);
		}
		this->subObjects.reserve(objectInfo.meshes.size());
		this->boundingBox = objectInfo.boundingBox;
		std::unordered_map<std::string, Ref<Texture>> textures;

		UniqueRef<Material> material;
		for (const auto& group : objectInfo.meshes)
		{
			if (group.useTexture)
			{
				if (group.material != nullptr)
				{
					material = MakeUnique<Material>();
					#define MAKE_TEX(tex) if(!group.material->tex.empty()) {\
						if(textures.find(group.material->tex) == textures.end())\
							textures[group.material->tex] = Graphics::Instance()->CreateTexture(group.material->tex);\
						material->tex = textures[group.material->tex];}

					MAKE_TEX(map_Ka);
					MAKE_TEX(map_Kd);
					MAKE_TEX(map_Ks);
					MAKE_TEX(map_Ke);
					MAKE_TEX(map_d);
					MAKE_TEX(map_bump);
					MAKE_TEX(bump);

					material->Tf = group.material->Tf;
					material->Ka = group.material->Ka;
					material->Kd = group.material->Kd;
					material->Ke = group.material->Ke;
					material->Ks = group.material->Ks;
					material->illum = group.material->illum;
					material->Ns = group.material->Ns;
					material->Ni = group.material->Ni;
					material->d = group.material->d;
					material->Tr = group.material->Tr;

					if (material->Ns == 0.0f) material->Ns = 128.0f; // bad as pow(0.0, 0.0) -> NaN
				}
				auto VBO = Graphics::Instance()->CreateVertexBuffer(group.buffer.data(), group.buffer.size(), UsageType::STATIC_DRAW);
				auto VAO = Graphics::Instance()->CreateVertexArray();
				auto VBL = Graphics::Instance()->CreateVertexBufferLayout();

				VBL->PushFloat(3);
				if (group.useTexture) VBL->PushFloat(3);
				if (group.useNormal) VBL->PushFloat(3);
				VAO->AddBuffer(*VBO, *VBL);

				RenderObject object(std::move(VBO), std::move(VAO), std::move(material), group.useTexture, group.useNormal, group.buffer.size());
				this->subObjects.push_back(std::move(object));
			}
			if (!group.useTexture)
			{
				Logger::Instance().Warning("MxEngine::MxObject", "object file does not have texture data: " + filepath);
			}
			if (!group.useNormal)
			{
				Logger::Instance().Warning("MxEngine::MxObject", "object file does not have normal data: " + filepath);
			}
		}
	}

	Mesh::Mesh(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	void Mesh::Load(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	std::vector<RenderObject>& Mesh::GetRenderObjects()
	{
		return this->subObjects;
	}

	const std::vector<RenderObject>& Mesh::GetRenderObjects() const
	{
		return this->subObjects;
	}

	Vector3 Mesh::GetObjectCenter() const
	{
		return (this->boundingBox.first + this->boundingBox.second) * 0.5f;
	}

	const std::pair<Vector3, Vector3> Mesh::GetBoundingBox()
	{
		return this->boundingBox;
	}

	void RenderObject::GenerateMeshIndicies() const
	{
		std::vector<IndexBuffer::IndexType> indicies;
		indicies.reserve(this->vertexBufferSize * 3);
		for (int i = 0; i < this->vertexBufferSize; i += 3)
		{
			indicies.push_back(i + 0);
			indicies.push_back(i + 1);
			indicies.push_back(i + 1);
			indicies.push_back(i + 2);
			indicies.push_back(i + 2);
			indicies.push_back(i + 0);
		}
		this->IBO = Graphics::Instance()->CreateIndexBuffer(indicies);
		this->meshGenerated = true;
	}

    RenderObject::RenderObject(UniqueRef<VertexBuffer> VBO, UniqueRef<VertexArray> VAO, UniqueRef<Material> material, bool useTexture, bool useNormal, size_t sizeInFloats)
    {
		this->VBO = std::move(VBO);
		this->VAO = std::move(VAO);
		this->material = std::move(material);
		this->useTexture = useTexture;
		this->useNormal = useTexture;
		this->vertexBufferSize = sizeInFloats;
    }

	const VertexArray& RenderObject::GetVAO() const
	{
		return *this->VAO;
	}

	const IndexBuffer& RenderObject::GetMeshIBO() const
	{
		if (!this->meshGenerated) GenerateMeshIndicies();
		return *this->IBO;
	}

	const Material& RenderObject::GetMaterial() const
	{
		return *this->material;
	}

	Material& RenderObject::GetMaterial()
	{
		return *this->material;
	}

    bool RenderObject::UsesTexture() const
    {
		return this->useTexture;
    }

	bool RenderObject::UsesNormals() const
	{
		return this->useNormal;
	}

	size_t RenderObject::GetVertexBufferSize() const
	{
		return this->vertexBufferSize;
	}

	bool RenderObject::HasMaterial() const
	{
		return this->material != nullptr;
	}

	void Mesh::AddInstancedBuffer(UniqueRef<VertexBuffer> vbo, UniqueRef<VertexBufferLayout> vbl)
	{
		this->VBOs.push_back(std::move(vbo));
		this->VBLs.push_back(std::move(vbl));
		for (const auto& subObject : this->subObjects)
		{
			subObject.VAO->AddInstancedBuffer(*this->VBOs.back(), *this->VBLs.back());
		}
	}

	VertexBuffer& Mesh::GetBufferByIndex(size_t index)
	{
		MX_ASSERT(index < this->VBOs.size());
		return *this->VBOs[index];
	}

	VertexBufferLayout& Mesh::GetBufferLayoutByIndex(size_t index)
	{
		MX_ASSERT(index < this->VBLs.size());
		return *this->VBLs[index];
	}

    size_t Mesh::GetBufferCount() const
    {
		return this->VBOs.size();
    }
}