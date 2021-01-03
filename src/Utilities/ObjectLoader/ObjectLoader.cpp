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
#include "Utilities/Logging/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Macro/Macro.h"
#include "Utilities/FileSystem/File.h"
#include "Utilities/Format/Format.h"
#include "Utilities/Random/Random.h"
#include "Utilities/Json/Json.h"
#include "Utilities/Image/ImageLoader.h"
#include "Utilities/Image/ImageManager.h"

#include <algorithm>

#if defined(MXENGINE_USE_ASSIMP)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

namespace MxEngine
{
	const char* const AlbedoTexName = "albedo";
	const char* const EmissiveTexName = "emmisive";
	const char* const HeightTexName = "height";
	const char* const NormalTexName = "normal";
	const char* const AOTexName = "ao";
	const char* const RoughnessTexName = "roughness";
	const char* const MetallicTexName = "metallic";
	const char* const RoughnessMetallicTexName = "roughness-metallic";
	constexpr ImageType PreferredFormat = ImageType::PNG;
	const char* const PreferredExtension = ".png";

	void SaveRoughnessMetallicTexture(const Image& image, const MxString& roughnessName, const MxString& metallicName, const FilePath& saveDirectory)
	{
		auto roughnessPath = saveDirectory /  ToFilePath(roughnessName + PreferredExtension);
		auto metallicPath  = saveDirectory /  ToFilePath(metallicName + PreferredExtension);
		if (File::Exists(roughnessPath) || File::Exists(metallicPath))
			return; // avoid rewriting existing textures

		auto roughnessData = (uint8_t*)std::malloc(image.GetTotalByteSize());
		auto metallicData = (uint8_t*)std::malloc(image.GetTotalByteSize());
		Image roughness(roughnessData, image.GetWidth(), image.GetHeight(), 1, image.IsFloatingPoint());
		Image metallic(metallicData, image.GetWidth(), image.GetHeight(), 1, image.IsFloatingPoint());

		for (size_t x = 0; x < image.GetWidth(); x++)
		{
			for (size_t y = 0; y < image.GetHeight(); y++)
			{
				if (image.IsFloatingPoint())
				{
					auto pixel = image.GetPixelFloat(x, y);
					// G channel
					roughness.SetPixelFloat(x, y, pixel[1], 0.0f, 0.0f, 0.0f);
					// B channel
					metallic.SetPixelFloat(x, y, pixel[2], 0.0f, 0.0f, 0.0f);
				}
				else
				{
					auto pixel = image.GetPixelByte(x, y);
					// G channel
					roughness.SetPixelByte(x, y, pixel[1], 0, 0, 0);
					// B channel
					metallic.SetPixelByte(x, y, pixel[2], 0, 0, 0);
				}
			}
		}
		ImageManager::SaveImage(roughnessPath, roughness, PreferredFormat);
		ImageManager::SaveImage(metallicPath, metallic, PreferredFormat);
	}

	FilePath GetActualTexturePath(const FilePath& lookupDirectory, const MxString& name, const aiScene* scene, const aiMaterial* material, aiTextureType type)
	{
		auto path = lookupDirectory / ToFilePath(name + PreferredExtension);
		if (File::Exists(path)) return path; // check if texture already on disk

		if (material->GetTextureCount(type) > 0)
		{
			aiString assimpPath;
			if (material->GetTexture(type, 0, &assimpPath) == aiReturn_SUCCESS)
			{
				const char* filepath = assimpPath.C_Str();

				// check if texture is embedded into object file
				// if no - just return path to a texture
				const aiTexture* data = scene->GetEmbeddedTexture(filepath);
				if (data == nullptr) return lookupDirectory / filepath;

				// if texture data is embedded, we need to create a file from it
				Image image;
				if (data->mHeight == 0.0f) // compressed data (jpg)
					image = ImageLoader::LoadImageFromMemory((const uint8_t*)data->pcData, data->mWidth);
				else
					image = ImageLoader::LoadImageFromMemory((const uint8_t*)data->pcData, data->mWidth * data->mHeight * sizeof(aiTexel));

				// PBR roughness & metallic texture encoded in G & B channels
				if (type == aiTextureType_UNKNOWN)
				{
					SaveRoughnessMetallicTexture(image, RoughnessTexName, MetallicTexName, lookupDirectory);
				}
				else
				{				
					// create image on disk to later load it
					ImageManager::SaveImage(path, image, PreferredFormat);
				}
				return path;
			}
		}
		return FilePath();
	}

	ObjectInfo ObjectLoader::Load(const FilePath& filepath)
	{
		auto directory = filepath.parent_path();
		ObjectInfo object;

		if (!File::Exists(filepath) || !File::IsFile(filepath))
		{
			MXLOG_ERROR("Assimp::Importer", "file does not exist: " + ToMxString(filepath));
			return object;
		}

		MAKE_SCOPE_PROFILER("ObjectLoader::LoadObject");
		MAKE_SCOPE_TIMER("MxEngine::ObjectLoader", "ObjectLoader::LoadObject");
		MXLOG_INFO("Assimp::Importer", "loading object from file: " + ToMxString(filepath));

		static Assimp::Importer importer; // TODO: not thread safe
		const aiScene* scene = importer.ReadFile(filepath.string().c_str(), 
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes |
			aiProcess_ImproveCacheLocality | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);
		if (scene == nullptr)
		{
			MXLOG_ERROR("Assimp::Importer", importer.GetErrorString());
			return object;
		}

		object.meshes.resize((size_t)scene->mNumMeshes);
		object.materials.resize((size_t)scene->mNumMaterials);

		for (size_t i = 0; i < object.materials.size(); i++)
		{
			auto& materialInfo = object.materials[i];
			auto& material = scene->mMaterials[i];

			materialInfo.Name = material->GetName().C_Str();
			if (materialInfo.Name.empty()) materialInfo.Name = MxFormat("material #{}", i);
			
			#define GET_FLOAT(type, field)\
			{ ai_real val;\
				if (material->Get(type, val) == aiReturn_SUCCESS)\
				{\
					materialInfo.field = (float)val;\
				}\
			}

			GET_FLOAT(AI_MATKEY_OPACITY, Transparency);
			GET_FLOAT(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, MetallicFactor);
			GET_FLOAT(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, RoughnessFactor);

			// TODO: this is workaround, because some object formats export alpha channel as 0, but its actually means 1
			if (materialInfo.Transparency == 0.0f) materialInfo.Transparency = 1.0f;

			aiColor4D baseColorPBR;
			if (material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, baseColorPBR) == aiReturn_SUCCESS)
			{
				materialInfo.BaseColor[0]  = baseColorPBR[0];
				materialInfo.BaseColor[1]  = baseColorPBR[1];
				materialInfo.BaseColor[2]  = baseColorPBR[2];
				materialInfo.Transparency *= baseColorPBR[3];
			}

			aiColor3D emissiveColor;
			if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == aiReturn_SUCCESS)
			{
				materialInfo.Emission = Max(emissiveColor.r, emissiveColor.g, emissiveColor.b);
			}

			// process first to make sure metallic / roughness will present when checking for existing textures in GetActualTexturePath
			auto _ = GetActualTexturePath(directory, MxFormat(RoughnessMetallicTexName, "{}_{}", i), scene, material, aiTextureType_UNKNOWN);

			materialInfo.AlbedoMap           = GetActualTexturePath(directory, MxFormat("{}_{}", AlbedoTexName,    i), scene, material, aiTextureType_DIFFUSE);
			materialInfo.EmissiveMap         = GetActualTexturePath(directory, MxFormat("{}_{}", EmissiveTexName,  i), scene, material, aiTextureType_EMISSIVE);
			materialInfo.HeightMap           = GetActualTexturePath(directory, MxFormat("{}_{}", HeightTexName,    i), scene, material, aiTextureType_HEIGHT);
			materialInfo.NormalMap           = GetActualTexturePath(directory, MxFormat("{}_{}", NormalTexName,    i), scene, material, aiTextureType_NORMALS);
			materialInfo.AmbientOcclusionMap = GetActualTexturePath(directory, MxFormat("{}_{}", AOTexName,        i), scene, material, aiTextureType_AMBIENT_OCCLUSION);
			materialInfo.RoughnessMap        = GetActualTexturePath(directory, MxFormat("{}_{}", RoughnessTexName, i), scene, material, aiTextureType_DIFFUSE_ROUGHNESS);
			materialInfo.MetallicMap         = GetActualTexturePath(directory, MxFormat("{}_{}", MetallicTexName,  i), scene, material, aiTextureType_METALNESS);

			// if aiTextureType_AMBIENT_OCCLUSION failed to load, try aiTextureType_LIGHTMAP as alternative, as it also can store ambient occlusion
			if(materialInfo.AmbientOcclusionMap.empty()) materialInfo.AmbientOcclusionMap = GetActualTexturePath(directory, MxFormat("{}_{}", AOTexName, i), scene, material, aiTextureType_LIGHTMAP);

			// if emmision texture provided, set emmision to some non-zero value
			if (!materialInfo.EmissiveMap.empty() && materialInfo.Emission == 0.0f) materialInfo.Emission = 1.0f;
		}

		Vector3 minCoords = MakeVector3(std::numeric_limits<float>::max());
		Vector3 maxCoords = MakeVector3(-1.0f * std::numeric_limits<float>::max());
		for (size_t i = 0; i < object.meshes.size(); i++)
		{
			auto& mesh = scene->mMeshes[i];
			auto coords = MinMaxComponents((Vector3*)mesh->mVertices, (size_t)mesh->mNumVertices);

			minCoords = VectorMin(minCoords, coords.first);
			maxCoords = VectorMax(maxCoords, coords.second);
		}
		auto objectCenter = (minCoords + maxCoords) * 0.5f;

		for (size_t i = 0; i < object.meshes.size(); i++)
		{
			auto& mesh = scene->mMeshes[i];
			auto& meshInfo = object.meshes[i];

			meshInfo.name = mesh->mName.C_Str();
			// check if mesh name was generated by assimp
			if (meshInfo.name.find("meshes_") != meshInfo.name.npos)
				meshInfo.name = MxFormat("mesh #{}", i);

			meshInfo.useNormal = mesh->HasNormals();
			meshInfo.useTexture = mesh->HasTextureCoords(0);
			meshInfo.material = object.materials.data() + (size_t)mesh->mMaterialIndex;

			MX_ASSERT(mesh->mNormals != nullptr);
			MX_ASSERT(mesh->mVertices != nullptr);
			MX_ASSERT(mesh->mNumFaces > 0);

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
				else
				{
					auto randomVec = Random::GetUnitVector3();
					vertex[i].Tangent = Cross(vertex[i].Normal, randomVec);
					vertex[i].Bitangent = Cross(vertex[i].Normal, vertex[i].Tangent);
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

    MaterialLibrary ObjectLoader::LoadMaterials(const FilePath& path)
    {
		MaterialLibrary materials;

		File file(path);
		if (!file.IsOpen())
		{
			MXLOG_ERROR("MxEngine::ObjectLoader", "cannot open file: " + ToMxString(path));
			return materials;
		}
		MXLOG_INFO("MxEngine::ObjectLoader", "loading materials from file: " + ToMxString(path));

		auto materialList = LoadJson(file);

		size_t materialCount = materialList.size();
		materials.resize(materialCount);
		for(size_t i = 0; i < materialCount; i++)
		{
			auto& json = materialList[i];
			auto& material = materials[i];

			material.Transparency        = json["Transparency"       ].get<float>();
			material.Displacement        = json["Displacement"       ].get<float>();
			material.Emission            = json["Emission"           ].get<float>();
			material.MetallicFactor      = json["MetallicFactor"     ].get<float>();
			material.RoughnessFactor     = json["RoughnessFactor"    ].get<float>();
									    
			material.BaseColor           = json["BaseColor"          ].get<Vector3>();
			material.UVMultipliers       = json["UVMultipliers"      ].get<Vector2>();
									    
			material.AlbedoMap           = json["AlbedoMap"          ].get<FilePath>();
			material.EmissiveMap         = json["EmissiveMap"        ].get<FilePath>();
			material.HeightMap           = json["HeightMap"          ].get<FilePath>();
			material.NormalMap           = json["NormalMap"          ].get<FilePath>();
			material.AmbientOcclusionMap = json["AmbientOcclusionMap"].get<FilePath>();
			material.RoughnessMap        = json["RoughnessMap"       ].get<FilePath>();
			material.MetallicMap         = json["MetallicMap"        ].get<FilePath>();
			material.Name                = json["Name"               ].get<MxString>();
		}													   

		return materials;
    }

	void ObjectLoader::DumpMaterials(const MaterialLibrary& materials, const FilePath& path)
	{
		JsonFile json;
		File file(path, File::WRITE);
		MXLOG_INFO("MxEngine::ObjectLoader", "dumping materials to file: " + ToMxString(path));

		#define DUMP(index, name) json[index][#name] = materials[index].name

		for (size_t i = 0; i < materials.size(); i++)
		{
			DUMP(i, Transparency);
			DUMP(i, Displacement);
			DUMP(i, MetallicFactor);
			DUMP(i, RoughnessFactor);
			DUMP(i, Emission);

			DUMP(i, BaseColor);
			DUMP(i, UVMultipliers);

			DUMP(i, AlbedoMap);
			DUMP(i, EmissiveMap);
			DUMP(i, HeightMap);
			DUMP(i, NormalMap);
			DUMP(i, MetallicMap);
			DUMP(i, RoughnessMap);
			DUMP(i, AmbientOcclusionMap);

			DUMP(i, Name);
		}

		SaveJson(file, json);
	}
}
#else

namespace MxEngine
{
	ObjectInfo ObjectLoader::Load(const FilePath& path)
	{
		MXLOG_ERROR("MxEngine::ObjectLoader", "object cannot be loaded as Assimp library was turned off in engine settings");
		ObjectInfo object;
		object.isSuccess = false;
		return object;
	}
}
#endif