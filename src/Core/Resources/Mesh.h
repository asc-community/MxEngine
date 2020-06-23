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

#include "Core/BoundingObjects/AABB.h"
#include "Core/Components/Transform.h"
#include "Utilities/String/String.h"
#include "Utilities/FileSystem/File.h"
#include "Utilities/Memory/Memory.h"
#include "Platform/GraphicAPI.h"
#include "Core/Resources/SubMesh.h"

namespace MxEngine
{
	class MeshRenderer;
	
	class Mesh
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;

		using SubmeshList = MxVector<SubMesh>;

		AABB boundingBox;
		
		SubmeshList submeshes;
		MxVector<GResource<VertexBuffer>> VBOs;
		MxVector<GResource<VertexBufferLayout>> VBLs;

		void LoadFromFile(const MxString& filepath);
	public:
		explicit Mesh() = default;
		Mesh(const MxString& path);
		Mesh(Mesh&) = delete;
		Mesh(Mesh&&) = default;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = default;
		
		void Load(const MxString& filepath);
		SubmeshList& GetSubmeshes();
		const SubmeshList& GetSubmeshes() const;
		const AABB& GetAABB() const;
		void SetAABB(const AABB& boundingBox);
		void UpdateAABB();
		size_t AddInstancedBuffer(GResource<VertexBuffer> vbo, GResource<VertexBufferLayout> vbl);
		GResource<VertexBuffer> GetBufferByIndex(size_t index) const; 
		GResource<VertexBufferLayout> GetBufferLayoutByIndex(size_t index) const;
		size_t GetBufferCount() const;
		void PopInstancedBuffer();
	};
}