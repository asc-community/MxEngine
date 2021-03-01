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
#include "Utilities/String/String.h"
#include "Utilities/Memory/Memory.h"
#include "Platform/GraphicAPI.h"
#include "Core/Resources/SubMesh.h"

namespace MxEngine
{
	class MeshRenderer;
	
	class Mesh
	{
		using SubMeshList = MxVector<SubMesh>;
		
		SubMeshList submeshes;
		MxString filePath;
		VertexBufferHandle VBO;
		VertexArrayHandle VAO;
		IndexBufferHandle IBO;
		MxVector<VertexBufferHandle> instancedVBOs;
		MxVector<VertexBufferLayoutHandle> instancedVBLs;
		MxVector<UniqueRef<TransformComponent>> subMeshTransforms;

		template<typename FilePath>
		void LoadFromFile(const FilePath& filepath);

	public:
		AABB MeshAABB;
		BoundingSphere MeshBoundingSphere;

		explicit Mesh();
		Mesh(Mesh&) = delete;
		Mesh(Mesh&&) = default;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = default;

		template<typename FilePath>
		Mesh(const FilePath& path);
		
		void Load(const MxString& filepath);
		template<typename FilePath> void Load(const FilePath& filepath);

		void ReserveData(size_t vertexCount, size_t indexCount);
		void UpdateBoundingGeometry();
		size_t AddInstancedBuffer(VertexBufferHandle vbo, VertexBufferLayoutHandle vbl);
		VertexBufferHandle GetBufferByIndex(size_t index) const; 
		VertexBufferLayoutHandle GetBufferLayoutByIndex(size_t index) const;
		VertexBufferHandle GetVBO() const;
		IndexBufferHandle GetIBO() const;
		VertexArrayHandle GetVAO() const;
		size_t GetTotalVerteciesCount() const;
		size_t GetTotalIndiciesCount() const;
		size_t GetInstancedBufferCount() const;
		void PopInstancedBuffer();
		void SetSubMeshesInternal(const SubMeshList& submeshes);
		const SubMeshList& GetSubMeshes() const;
		const SubMesh& GetSubMeshByIndex(size_t index) const;
		SubMesh& GetSubMeshByIndex(size_t index);
		SubMesh& AddSubMesh(SubMesh::MaterialId materialId, MeshData data);
		void DeleteSubMeshByIndex(size_t index);

		const MxString& GetFilePath() const;
		void SetInternalEngineTag(const MxString& tag);
		bool IsInternalEngineResource() const;
	};
}