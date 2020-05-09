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

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "Utilities/Math/Math.h"
#include "Core/BoundingObjects/AABB.h"

namespace MxEngine
{
	struct MaterialInfo
	{
		std::string name;

		std::string map_Ka;
		std::string map_Kd;
		std::string map_Ks;
		std::string map_Ke;
		std::string map_d;
		std::string map_height;
		std::string bump;

		float Ns = 0.0f;
		float Ni = 0.0f;
		float d = 0.0f;
		float displacement = 0.0f;
		Vector3 Tf{ 0.0f };
		Vector3 Ka{ 0.0f };
		Vector3 Kd{ 0.0f };
		Vector3 Ks{ 0.0f };
		Vector3 Ke{ 0.0f };
		int illum = 0;
		bool IsSuccess = true;
	};

	struct MeshInfo
	{
		std::string name;
		std::vector<float> buffer;
		std::vector<unsigned int> faces;		
		MaterialInfo* material = nullptr;
		bool useTexture = false;
		bool useNormal = false;
		constexpr static size_t VertexSize = (3 + 2 + 3 + 3 + 3);

		size_t GetVertexCount() const { return this->buffer.size() / VertexSize; }
	};

	using MaterialLibrary = std::vector<MaterialInfo>;

	struct ObjectInfo
	{
		MaterialLibrary materials;
		std::vector<MeshInfo> meshes;
		AABB boundingBox;
	};

	class ObjectLoader
	{
	public:
		static ObjectInfo Load(std::string path);
	};
}