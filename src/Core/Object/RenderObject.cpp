// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
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

#include "RenderObject.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"

namespace MxEngine
{
	void RenderObjectContainer::LoadFromFile(const std::string& filepath)
	{
		ObjFileInfo objectInfo;
		{
			MAKE_SCOPE_TIMER("MxEngine::Object", "ObjectLoader::LoadFromFile()");
			objectInfo = ObjectLoader::Load(filepath);
		}

		MAKE_SCOPE_PROFILER("Object::GenBuffers");
		MAKE_SCOPE_TIMER("MxEngine::Object", "Object::GenBuffers()");
		if (!objectInfo.isSuccess)
		{
			Logger::Instance().Debug("MxEngine::Object", "failed to load object from file: " + filepath);
		}
		this->subObjects.reserve(objectInfo.groups.size());
		this->objectCenter = objectInfo.objectCenter;
		std::unordered_map<std::string, Ref<Texture>> textures;
		for (const auto& group : objectInfo.groups)
		{
			RenderObject object;
			object.VAO = Graphics::Instance()->CreateVertexArray();
			object.vertexCount = group.faces.size();
			object.useTexture = group.useTexture;
			object.useNormal = group.useNormal;
			object.VBOs.emplace_back(Graphics::Instance()->CreateVertexBuffer(group.buffer, UsageType::STATIC_DRAW));

			auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
			VBL->PushFloat(3);
			if (group.useTexture) VBL->PushFloat(3);
			if (group.useNormal) VBL->PushFloat(3);
			object.VAO->AddBuffer(*object.VBOs.back(), *VBL);

			if (group.useTexture)
			{
				if (group.material != nullptr)
				{
					#define MAKE_TEX(tex) if(!group.material->tex.empty()) {\
						if(textures.find(group.material->tex) == textures.end())\
							textures[group.material->tex] = Graphics::Instance()->CreateTexture(group.material->tex);\
						object.material->tex = textures[group.material->tex];}

					object.material = MakeUnique<Material>();

					MAKE_TEX(map_Ka);
					MAKE_TEX(map_Kd);
					MAKE_TEX(map_Ks);
					MAKE_TEX(map_Ke);
					MAKE_TEX(map_d);
					MAKE_TEX(map_bump);
					MAKE_TEX(bump);

					object.material->Tf = group.material->Tf;
					object.material->Ka = group.material->Ka;
					object.material->Kd = group.material->Kd;
					object.material->Ke = group.material->Ke;
					object.material->Ks = group.material->Ks;
					object.material->illum = group.material->illum;
					object.material->Ns = group.material->Ns;
					object.material->Ni = group.material->Ni;
					object.material->d = group.material->d;
					object.material->Tr = group.material->Tr;

					if (object.material->Ns == 0.0f) object.material->Ns = 128.0f; // bad as pow(0.0, 0.0) -> NaN
				}
			}
			this->subObjects.push_back(std::move(object));
		}
	}

	RenderObjectContainer::RenderObjectContainer(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	void RenderObjectContainer::Load(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	std::vector<RenderObject>& RenderObjectContainer::GetRenderObjects()
	{
		return this->subObjects;
	}

	const Vector3& RenderObjectContainer::GetObjectCenter() const
	{
		return this->objectCenter;
	}

	void RenderObject::GenerateMeshIndicies() const
	{
		std::vector<IndexBuffer::IndexType> indicies;
		indicies.reserve(this->vertexCount * 3);
		for (int i = 0; i < this->vertexCount; i += 3)
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

	size_t RenderObject::GetVertexCount() const
	{
		return this->vertexCount;
	}

	bool RenderObject::HasMaterial() const
	{
		return this->material != nullptr;
	}

	void RenderObject::AddInstancedBuffer(UniqueRef<VertexBuffer> vbo, UniqueRef<VertexBufferLayout> vbl)
	{
		this->VBOs.push_back(std::move(vbo));
		this->VAO->AddInstancedBuffer(*this->VBOs.back(), *vbl);
	}

	void RenderObject::AddBuffer(UniqueRef<VertexBuffer> vbo, UniqueRef<VertexBufferLayout> vbl)
	{
		this->VBOs.push_back(std::move(vbo));
		this->VAO->AddBuffer(*this->VBOs.back(), *vbl);
	}
}