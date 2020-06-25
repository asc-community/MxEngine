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

#include "ObjectLoader.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Macro/Macro.h"
#include "Utilities/FileSystem/File.h"
#include "Utilities/Format/Format.h"
#include "Utilities/Random/Random.h"
#include "Utilities/Json/Json.h"

#include <algorithm>

#if defined(MXENGINE_USE_ASSIMP)
#include "Vendors/assimp/Importer.hpp"
#include "Vendors/assimp/scene.h"
#include "Vendors/assimp/postprocess.h"

namespace MxEngine
{
	ObjectInfo ObjectLoader::Load(const MxString& filename)
	{
		auto filepath = FilePath(filename.c_str());
		auto directory = filepath.parent_path();
		ObjectInfo object;

		if (!File::Exists(filepath) || !File::IsFile(filepath))
		{
			MxEngine::Logger::Instance().Error("Assimp::Importer", "file does not exist: " + filename);
			return object;
		}

		MAKE_SCOPE_PROFILER("ObjectLoader::LoadObject");
		MAKE_SCOPE_TIMER("MxEngine::ObjectLoader", "ObjectLoader::LoadObject");
		Logger::Instance().Debug("Assimp::Importer", MxFormat("loading object from file: {}", filename));
		static Assimp::Importer importer; // not thread safe
		const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices 
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

			materialInfo.Name = material->GetName().C_Str();
			
			#define GET_FLOAT(type, field)\
			{ ai_real val;\
				if (material->Get(AI_MATKEY_##type, val) == aiReturn_SUCCESS)\
				{\
					materialInfo.field = (float)val;\
				}\
			}
			GET_FLOAT(OPACITY, Transparency);
			GET_FLOAT(SHININESS, SpecularExponent);

			// TODO: this is workaround, because some object formats export alpha channel as 0, but its actually means 1
			if (materialInfo.Transparency == 0.0f) materialInfo.Transparency = 1.0f;

			#define GET_COLOR(type, field)\
			{ aiColor3D color;\
				if (material->Get(AI_MATKEY_COLOR_##type, color) == aiReturn_SUCCESS)\
				{\
					materialInfo.field.x = color.r;\
					materialInfo.field.y = color.g;\
					materialInfo.field.z = color.b;\
				}\
			}
			GET_COLOR(AMBIENT,  AmbientColor);
			GET_COLOR(DIFFUSE,  DiffuseColor);
			GET_COLOR(SPECULAR, SpecularColor);
			GET_COLOR(EMISSIVE, EmmisiveColor);

			#define GET_TEXTURE(type, field)\
			if (material->GetTextureCount(type) > 0)\
			{\
				aiString path;\
				if (material->GetTexture(type, 0, &path) == aiReturn_SUCCESS)\
				{\
					materialInfo.field = MxString((directory / path.C_Str()).string().c_str());\
				}\
			}
			GET_TEXTURE(aiTextureType_AMBIENT, AmbientMap);
			GET_TEXTURE(aiTextureType_DIFFUSE, DiffuseMap);
			GET_TEXTURE(aiTextureType_SPECULAR, SpecularMap);
			GET_TEXTURE(aiTextureType_EMISSIVE, EmmisiveMap);
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

		for (size_t i = 0; i < object.meshes.size(); i++)
		{
			auto& mesh = scene->mMeshes[i];
			auto& meshInfo = object.meshes[i];
			meshInfo.name = mesh->mName.C_Str();
			meshInfo.useNormal = mesh->HasNormals();
			meshInfo.useTexture = mesh->HasTextureCoords(0);
			meshInfo.material = object.materials.data() + mesh->mMaterialIndex;

			MX_ASSERT(mesh->mNormals != nullptr);
			MX_ASSERT(mesh->mVertices != nullptr);
			MX_ASSERT(mesh->mNumFaces > 0);
			constexpr size_t VertexSize = (3 + 2 + 3 + 3 + 3);

			MxVector<Vertex> vertex;
			vertex.resize((size_t)mesh->mNumVertices);
			for (size_t i = 0; i < (size_t)mesh->mNumVertices; i++)
			{
				vertex[i].Position = ((Vector3*)mesh->mVertices)[i];
				vertex[i].Position -= objectCenter;
				vertex[i].Normal    = ((Vector3*)mesh->mNormals)[i];

				if (meshInfo.useTexture)
				{
					vertex[i].TexCoord = MakeVector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
					vertex[i].Tangent = ((Vector3*)mesh->mTangents)[i];
					vertex[i].Bitangent = ((Vector3*)mesh->mBitangents)[i];
				}
			}

			meshInfo.indicies.resize((size_t)mesh->mNumFaces * 3);
			for (size_t i = 0; i < (size_t)mesh->mNumFaces; i++)
			{
				MX_ASSERT(mesh->mFaces[i].mNumIndices == 3);
				meshInfo.indicies[3 * i + 0] = mesh->mFaces[i].mIndices[0];
				meshInfo.indicies[3 * i + 1] = mesh->mFaces[i].mIndices[1];
				meshInfo.indicies[3 * i + 2] = mesh->mFaces[i].mIndices[2];
			}
			if (meshInfo.name.empty())
				meshInfo.name = UUIDGenerator::Get();
			meshInfo.useTexture = true;
			meshInfo.vertecies = std::move(vertex);
		}
		importer.FreeScene();

		return object;
	}

    MaterialLibrary ObjectLoader::LoadMaterials(const MxString& path)
    {
		MaterialLibrary materials;

		File file(path);
		if (!file.IsOpen())
		{
			Logger::Instance().Error("MxEngine::ObjectLoader", "cannot open file: " + path);
			return materials;
		}
		Logger::Instance().Debug("MxEngine::ObjectLoader", "loading materials from file: " + path);

		auto materialList = Json::LoadJson(file);

		size_t materialCount = materialList.size();
		materials.resize(materialCount);
		for(size_t i = 0; i < materialCount; i++)
		{
			auto& json = materialList[i];
			auto& material = materials[i];

			material.Transparency     = json["Transparency"].get<float>();
			material.Displacement     = json["Displacement"].get<float>();
			material.SpecularExponent = json["SpecularExponent"].get<float>();

			material.AmbientColor  = json["AmbientColor" ].get<Vector3>();
			material.DiffuseColor  = json["DiffuseColor" ].get<Vector3>();
			material.SpecularColor = json["SpecularColor"].get<Vector3>();
			material.EmmisiveColor = json["EmmisiveColor"].get<Vector3>();

			material.AmbientMap       = json["AmbientMap"     ].get<MxString>();
			material.DiffuseMap       = json["DiffuseMap"     ].get<MxString>();
			material.SpecularMap      = json["SpecularMap"    ].get<MxString>();
			material.EmmisiveMap      = json["EmmisiveMap"    ].get<MxString>();
			material.HeightMap        = json["HeightMap"      ].get<MxString>();
			material.NormalMap        = json["NormalMap"      ].get<MxString>();
			material.Name             = json["Name"           ].get<MxString>();
		}

		return materials;
    }

	void ObjectLoader::DumpMaterials(const MaterialLibrary& materials, const MxString& path)
	{
		JsonFile json;
		File file(path, File::WRITE);
		Logger::Instance().Debug("MxEngine::ObjectLoader", "dumping materials to file: " + path);

		#define DUMP(index, name) json[index][#name] = materials[index].name

		for (size_t i = 0; i < materials.size(); i++)
		{
			DUMP(i, Transparency);
			DUMP(i, Displacement);
			DUMP(i, SpecularExponent);

			DUMP(i, AmbientColor);
			DUMP(i, DiffuseColor);
			DUMP(i, SpecularColor);
			DUMP(i, EmmisiveColor);

			DUMP(i, AmbientMap);
			DUMP(i, DiffuseMap);
			DUMP(i, SpecularMap);
			DUMP(i, EmmisiveMap);
			DUMP(i, HeightMap);
			DUMP(i, NormalMap);
			DUMP(i, Name);
		}

		Json::SaveJson(file, json);
	}
}
#else

namespace MxEngine
{
	ObjectInfo ObjectLoader::Load(MxString path)
	{
		Logger::Instance().Error("MxEngine::ObjectLoader", "object cannot be loaded as Assimp library was turned off in engine settings");
		ObjectInfo object;
		object.isSuccess = false;
		return object;
	}
}
#endif