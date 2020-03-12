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

#include <filesystem>

namespace MxEngine
{
	class RenderObject : public IRenderable
	{
		bool useTexture = false, useNormal = false;
		mutable bool meshGenerated = false;
		mutable UniqueRef<IndexBuffer> IBO;		
		UniqueRef<VertexBuffer> VBO;
		UniqueRef<VertexArray> VAO;
		UniqueRef<Material> material;
		size_t vertexCount = 0;

		void GenerateMeshIndicies() const;
		friend class Mesh;
	public:
		RenderObject(UniqueRef<VertexBuffer> VBO, UniqueRef<VertexArray> VAO, UniqueRef<Material> material, 
			bool useTexture, bool useNormal, size_t vertexCount);

		RenderObject(const RenderObject&) = delete;
		RenderObject(RenderObject&&) noexcept = default;
		RenderObject& operator=(const RenderObject&) = delete;
		RenderObject& operator=(RenderObject&&) noexcept = default;

		Material& GetMaterial();
		bool UsesTexture() const;
		bool UsesNormals() const;

		virtual const VertexArray& GetVAO() const override;
		virtual const IndexBuffer& GetMeshIBO() const override;
		virtual const Material& GetMaterial() const override;
		virtual size_t GetVertexCount() const override;
		virtual bool HasMaterial() const override;
	};

	class Mesh
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;

		Vector3 objectCenter = MakeVector3(0.0f);
		std::vector<RenderObject> subObjects;
		std::vector<UniqueRef<VertexBuffer>> VBOs;
		std::vector<UniqueRef<VertexBufferLayout>> VBLs;

		void LoadFromFile(const std::string& filepath);
	public:
		explicit Mesh() = default;
		explicit Mesh(const std::string& filepath);
		Mesh(Mesh&) = delete;
		Mesh(Mesh&&) = default;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = default;

		void Load(const std::string& filepath);
		std::vector<RenderObject>& GetRenderObjects();
		const std::vector<RenderObject>& GetRenderObjects() const;
		const Vector3& GetObjectCenter() const;
		void AddInstancedBuffer(UniqueRef<VertexBuffer> vbo, UniqueRef<VertexBufferLayout> vbl);
		VertexBuffer& GetBufferByIndex(size_t index); 
		VertexBufferLayout& GetBufferLayoutByIndex(size_t index);
		size_t GetBufferCount() const;
	};
}