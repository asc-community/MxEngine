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

#pragma once

#include "Core/Interfaces/IRenderable.h"
#include "Core/BoundingObjects/AABB.h"
#include "Core/Components/Transform/Transform.h"

#include <filesystem>

namespace MxEngine
{
	class SubMesh : public IRenderable
	{
		bool useTexture = false, useNormal = false;
		mutable bool meshGenerated = false;
		mutable UniqueRef<IndexBuffer> meshIBO;		
		UniqueRef<VertexBuffer> VBO;
		UniqueRef<VertexArray> VAO;
		UniqueRef<IndexBuffer> IBO;
		size_t vertexBufferSize = 0;
		Ref<Material> material;
		Ref<Vector4> renderColor;
		Ref<Transform> transform;
		std::string name;

		void GenerateMeshIndicies() const;
		friend class Mesh;
	public:
		SubMesh(std::string name, UniqueRef<VertexBuffer> VBO, UniqueRef<VertexArray> VAO, UniqueRef<IndexBuffer> IBO, 
			Ref<Material> material, Ref<Vector4> color, Ref<Transform> transform,
			bool useTexture, bool useNormal, size_t sizeInFloats);

		SubMesh(const SubMesh&) = delete;
		SubMesh(SubMesh&&) noexcept = default;
		SubMesh& operator=(const SubMesh&) = delete;
		SubMesh& operator=(SubMesh&&) noexcept = default;

		Material& GetMaterial();
		const std::string& GetName() const;
		bool UsesTexture() const;
		bool UsesNormals() const;
		void SetRenderColor(const Vector4& color);
		Transform& GetTransform();

		virtual const Transform& GetTransform() const override;
		virtual const Vector4& GetRenderColor() const override;
		virtual const VertexArray& GetVAO() const override;
		virtual IndexBuffer& GetIBO() const override;
		virtual const IndexBuffer& GetMeshIBO() const override;
		virtual const Material& GetMaterial() const override;
		virtual size_t GetVertexBufferSize() const override;
		virtual bool HasMaterial() const override;
	};

	class SubMeshLOD
	{

	};

	class Mesh
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;

		using LOD = std::vector<SubMesh>;

		AABB boundingBox;
		size_t currentLOD = 0;
		
		std::vector<LOD> LODs = std::vector<LOD>(1);
		std::vector<UniqueRef<VertexBuffer>> VBOs;
		std::vector<UniqueRef<VertexBufferLayout>> VBLs;

		void LoadFromFile(const std::string& filepath);
	public:
		size_t RefCounter = 0;

		explicit Mesh() = default;
		explicit Mesh(const std::string& filepath);
		Mesh(Mesh&) = delete;
		Mesh(Mesh&&) = default;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = default;
		
		void Load(const std::string& filepath);
		std::vector<SubMesh>& GetRenderObjects();
		const std::vector<SubMesh>& GetRenderObjects() const;
		void PushEmptyLOD();
		void PopLastLOD();
		void SetLOD(size_t LOD);
		size_t GetLOD() const;
		size_t GetLODCount() const;
		const AABB& GetAABB() const;
		void SetAABB(const AABB& boundingBox);
		void AddInstancedBuffer(UniqueRef<VertexBuffer> vbo, UniqueRef<VertexBufferLayout> vbl);
		VertexBuffer& GetBufferByIndex(size_t index); 
		VertexBufferLayout& GetBufferLayoutByIndex(size_t index);
		size_t GetBufferCount() const;
	};
}