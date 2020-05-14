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
	/*!
	material info is a plain data structure, containing info how to load all resources of object material
	*/
	struct MaterialInfo
	{
		/*!
		name of material
		*/
		std::string name;

		/*!
		ambient texture path
		*/
		std::string map_Ka;
		/*!
		diffuse texture path
		*/
		std::string map_Kd;
		/*!
		specular texture path
		*/
		std::string map_Ks;
		/*!
		emmisive texture path
		*/
		std::string map_Ke;
		/*!
		transparency texture path
		*/
		std::string map_d;
		/*!
		height texture path
		*/
		std::string map_height;
		/*!
		normal texture path
		*/
		std::string map_normal;

		/*
		specular power value
		*/
		float Ns = 0.0f;
		/*
		specular intensity value
		*/
		float Ni = 0.0f;
		/*
		transparency value
		*/
		float d = 0.0f;
		/*
		height displacement value
		*/
		float displacement = 0.0f;
		
		/*!
		transmission color
		*/
		Vector3 Tf{ 0.0f };
		/*!
		ambient color
		*/
		Vector3 Ka{ 0.0f };
		/*!
		diffuse color
		*/
		Vector3 Kd{ 0.0f };
		/*!
		specular color
		*/
		Vector3 Ks{ 0.0f };
		/*!
		emmisive color
		*/
		Vector3 Ke{ 0.0f };
		/*!
		illumination type
		*/
		int illum = 0;
	};

	/*!
	mesh info is a temporary class which stores object model data. Later it is used to construct SubMesh class object
	*/
	struct MeshInfo
	{
		/*!
		name of mesh
		*/
		std::string name;
		/*!
		buffer containing all vertex data (use faces[i] * VertexSize as index to access each vertex in buffer)
		*/
		std::vector<float> buffer;
		/*
		face indicies to access buffer of verteces
		*/
		std::vector<unsigned int> faces;		
		/*!
		mesh material pointer (to external table passed with MeshInfo inside ObjectInfo)
		*/
		MaterialInfo* material = nullptr;
		/*!
		has the mesh texture data (uv-coords) or not
		*/
		bool useTexture = false;
		/*!
		has the mesh normal data (and tangent space) or not
		*/
		bool useNormal = false;
		/*!
		size of each vertex: (vec3 position, vec2 uv-coords, vec3 normal, vec3 tangent, vec3 bitangent)
		*/
		constexpr static size_t VertexSize = (3 + 2 + 3 + 3 + 3);

		/*!
		returns count of verteces in buffer
		*/
		size_t GetVertexCount() const { return this->buffer.size() / VertexSize; }
	};

	using MaterialLibrary = std::vector<MaterialInfo>;

	/*!
	object info a special class which contains all data from which in-game object can be constructed
	it includes verteces, materials and precomputed bounding box. Also note that object is guaranteed to be aligned at (0, 0, 0), game world center
	*/
	struct ObjectInfo
	{
		/*!
		list of all materials. MeshInfo references each by pointer, so MaterialLibrary should not be altered after ObjectInfo construction
		*/
		MaterialLibrary materials;
		/*!
		list of all object meshes. For more info see MeshInfo documentation
		*/
		std::vector<MeshInfo> meshes;
		/*!
		precomputed bounding box of whole object. TODO: compute bounding boxes for each submesh
		*/
		AABB boundingBox;
	};

	/*!
	object loader is a special class which loads any type of file with object data into MxEngine compatible format (i.e. ObjectInfo)
	it supports same file types as Assimp library does, as it is base on it. For more info check documentation: https://github.com/assimp/assimp
	*/
	class ObjectLoader
	{
	public:
		/*
		loads object from disk by its file path
		\param path absoulute or relative to executable folder path to a file to load
		\returns ObjectInfo instance
		*/
		static ObjectInfo Load(std::string path);
	};
}