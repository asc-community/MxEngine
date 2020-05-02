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

#include "ObjectLoader.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Macro/Macro.h"
#include "Utilities/FileSystem/FileSystem.h"
#include "Utilities/Format/Format.h"
#include "Utilities/Random/Random.h"

#include <algorithm>

#if defined(MXENGINE_USE_ASSIMP)
#include "Vendors/assimp/Importer.hpp"
#include "Vendors/assimp/scene.h"
#include "Vendors/assimp/postprocess.h"

namespace MxEngine
{
	ObjectInfo ObjectLoader::Load(std::string filename)
	{
		auto directory = FilePath(filename).parent_path();
		ObjectInfo object;
		MAKE_SCOPE_PROFILER("ObjectLoader::LoadObject");
		MAKE_SCOPE_TIMER("MxEngine::ObjectLoader", "ObjectLoader::LoadObject");
		Logger::Instance().Debug("Assimp::Importer", "loading object from file: " + filename);
		static Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices 
			| aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);
		if (scene == nullptr)
		{
			MxEngine::Logger::Instance().Error("Assimp::Importer", importer.GetErrorString());
			return object;
		}

		object.meshes.resize(scene->mNumMeshes);
		object.materials.resize(scene->mNumMaterials);

		for (size_t i = 0; i < object.materials.size(); i++)
		{
			auto& materialInfo = object.materials[i];
			auto& material = scene->mMaterials[i];

			materialInfo.name = material->GetName().C_Str();
			
			#define GET_FLOAT(type, field)\
			{ ai_real val;\
				if (material->Get(AI_MATKEY_##type, val) == aiReturn_SUCCESS)\
				{\
					materialInfo.field = (float)val;\
				}\
			}
			GET_FLOAT(OPACITY, d);
			GET_FLOAT(SHININESS, Ns);

			#define GET_COLOR(type, field)\
			{ aiColor3D color;\
				if (material->Get(AI_MATKEY_COLOR_##type, color) == aiReturn_SUCCESS)\
				{\
					materialInfo.field.x = color.r;\
					materialInfo.field.y = color.g;\
					materialInfo.field.z = color.b;\
				}\
			}
			GET_COLOR(AMBIENT,     Ka);
			GET_COLOR(DIFFUSE,     Kd);
			GET_COLOR(SPECULAR,    Ks);
			GET_COLOR(EMISSIVE,    Ke);

			#define GET_TEXTURE(type, field)\
			if (material->GetTextureCount(type) > 0)\
			{\
				aiString path;\
				if (material->GetTexture(type, 0, &path) == aiReturn_SUCCESS)\
				{\
					materialInfo.field = (directory / path.C_Str()).string(); \
				}\
			}
			GET_TEXTURE(aiTextureType_AMBIENT, map_Ka);
			GET_TEXTURE(aiTextureType_DIFFUSE, map_Kd);
			GET_TEXTURE(aiTextureType_SPECULAR, map_Ks);
			GET_TEXTURE(aiTextureType_EMISSIVE, map_Ke);
		}

		Vector3 minCoords = MakeVector3(std::numeric_limits<float>::max());
		Vector3 maxCoords = MakeVector3(-1.0f * std::numeric_limits<float>::max());
		for (size_t i = 0; i < object.meshes.size(); i++)
		{
			auto& mesh = scene->mMeshes[i];
			auto coords = MinMaxComponents((Vector3*)mesh->mVertices, mesh->mNumVertices);

			minCoords = VectorMin(minCoords, coords.first);
			maxCoords = VectorMax(maxCoords, coords.second);
		}
		auto objectCenter = (minCoords + maxCoords) * 0.5f;
		object.boundingBox = AABB{ minCoords - objectCenter, maxCoords - objectCenter };

		for (size_t i = 0; i < object.meshes.size(); i++)
		{
			auto& mesh = scene->mMeshes[i];
			auto& meshInfo = object.meshes[i];
			meshInfo.name = mesh->mName.C_Str();
			meshInfo.useNormal = mesh->HasNormals();
			meshInfo.useTexture = mesh->HasTextureCoords(0);
			meshInfo.material = object.materials.data() + mesh->mMaterialIndex;

			MX_ASSERT(mesh->mNormals != nullptr);
			MX_ASSERT(mesh->mTextureCoords != nullptr);
			MX_ASSERT(mesh->mVertices != nullptr);
			MX_ASSERT(mesh->mNumFaces > 0);
			constexpr size_t VertexSize = (3 + 2 + 3 + 3 + 3);

			std::vector<float> vertex;
			vertex.reserve(VertexSize * (size_t)mesh->mNumVertices);
			for (size_t i = 0; i < (size_t)mesh->mNumVertices; i++)
			{
				((Vector3*)mesh->mVertices)[i] -= objectCenter;
				vertex.push_back(mesh->mVertices[i].x);
				vertex.push_back(mesh->mVertices[i].y);
				vertex.push_back(mesh->mVertices[i].z);
				if (meshInfo.useTexture)
				{
					vertex.push_back(mesh->mTextureCoords[0][i].x);
					vertex.push_back(mesh->mTextureCoords[0][i].y);
				}
				else
				{
					vertex.push_back(0.0f);
					vertex.push_back(0.0f);
				}

				vertex.push_back(mesh->mNormals[i].x);
				vertex.push_back(mesh->mNormals[i].y);
				vertex.push_back(mesh->mNormals[i].z);

				vertex.push_back(mesh->mTangents[i].x);
				vertex.push_back(mesh->mTangents[i].y);
				vertex.push_back(mesh->mTangents[i].z);

				vertex.push_back(mesh->mBitangents[i].x);
				vertex.push_back(mesh->mBitangents[i].y);
				vertex.push_back(mesh->mBitangents[i].z);
			}

			meshInfo.faces.resize((size_t)mesh->mNumFaces * 3);
			for (size_t i = 0; i < (size_t)mesh->mNumFaces; i++)
			{
				MX_ASSERT(mesh->mFaces[i].mNumIndices == 3);
				meshInfo.faces[3 * i + 0] = mesh->mFaces[i].mIndices[0];
				meshInfo.faces[3 * i + 1] = mesh->mFaces[i].mIndices[1];
				meshInfo.faces[3 * i + 2] = mesh->mFaces[i].mIndices[2];
			}
			if (meshInfo.name.empty()) 
				meshInfo.name = Format(FMT_STRING("unnamed_hash_{0}"), Random::Get(0LL, Random::Max));
			meshInfo.useTexture = true;
			meshInfo.buffer = std::move(vertex);
		}
		return object;
	}
}
#else

namespace MxEngine
{
	ObjectInfo ObjectLoader::Load(std::string path)
	{
		Logger::Instance().Error("MxEngine::ObjectLoader", "object cannot be loaded as Assimp library was turned off in engine settings");
		ObjectInfo object;
		object.isSuccess = false;
		return object;
	}
}
#endif