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

#include <algorithm>

#if defined(MXENGINE_USE_ASSIMP)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace MxEngine
{
	ObjectInfo ObjectLoader::Load(std::string filename)
	{
		ObjectInfo object;
		MAKE_SCOPE_PROFILER("ObjectLoader::LoadObject");
		MAKE_SCOPE_TIMER("MxEngine::ObjectLoader", "ObjectLoader::LoadObject");
		Logger::Instance().Debug("Assimp::Importer", "loading object from file: " + filename);
		static Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
		if (scene == nullptr)
		{
			MxEngine::Logger::Instance().Error("Assimp::Importer", importer.GetErrorString());
			object.isSuccess = false;
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
			GET_COLOR(TRANSPARENT, Tf);

			#define GET_TEXTURE(type, field)\
			if (material->GetTextureCount(type) > 0)\
			{\
				aiString path;\
				if (material->GetTexture(type, 0, &path) == aiReturn_SUCCESS)\
				{\
					materialInfo.field = path.C_Str(); \
				}\
			}
			GET_TEXTURE(aiTextureType_AMBIENT, map_Ka);
			GET_TEXTURE(aiTextureType_DIFFUSE, map_Kd);
			GET_TEXTURE(aiTextureType_SPECULAR, map_Ks);
			GET_TEXTURE(aiTextureType_EMISSIVE, map_Ke);
		}

		for (size_t i = 0; i < object.meshes.size(); i++)
		{
			auto& mesh = scene->mMeshes[i];
			auto& meshInfo = object.meshes[i];
			meshInfo.name = mesh->mName.C_Str();
			meshInfo.useNormal = mesh->HasNormals();
			meshInfo.useTexture = mesh->HasTextureCoords(0);
			meshInfo.material = object.materials.data() + mesh->mMaterialIndex;

			assert(mesh->mNormals != nullptr);
			assert(mesh->mTextureCoords != nullptr);
			assert(mesh->mVertices != nullptr);
			assert(mesh->mNumFaces > 0);
			constexpr size_t VertexSize = (3 + 3 + 3);

			std::vector<float> vertex;
			vertex.reserve(VertexSize * (size_t)mesh->mNumVertices);
			for (size_t i = 0; i < (size_t)mesh->mNumVertices; i++)
			{
				vertex.push_back(mesh->mVertices[i].x);
				vertex.push_back(mesh->mVertices[i].y);
				vertex.push_back(mesh->mVertices[i].z);
				if (meshInfo.useTexture)
				{
					vertex.push_back(mesh->mTextureCoords[0][i].x);
					vertex.push_back(mesh->mTextureCoords[0][i].y);
					vertex.push_back(mesh->mTextureCoords[0][i].z);
				}
				if (meshInfo.useNormal)
				{
					vertex.push_back(mesh->mNormals[i].x);
					vertex.push_back(mesh->mNormals[i].y);
					vertex.push_back(mesh->mNormals[i].z);
				}
			}

			meshInfo.buffer = std::move(vertex);

			//  meshInfo.faces.resize((size_t)mesh->mNumFaces);
			//  for (size_t i = 0; i < (size_t)mesh->mNumFaces; i++)
			//  {
			//  	assert(mesh->mFaces[i].mNumIndices == 3);
			//  	meshInfo.faces[i].x = mesh->mFaces[i].mIndices[0];
			//  	meshInfo.faces[i].y = mesh->mFaces[i].mIndices[1];
			//  	meshInfo.faces[i].z = mesh->mFaces[i].mIndices[2];
			//  }
			  
			//  // convert IBO + VBO -> VBO
			//  meshInfo.buffer.reserve(mesh->mNumFaces * 3 * VertexSize);
			//  for (size_t i = 0; i < (size_t)mesh->mNumFaces; i++)
			//  {
			//  	for (size_t j = 0; j < 3; j++)
			//  	{
			//  		size_t index = mesh->mFaces[i].mIndices[j];
			//  		meshInfo.buffer.insert(
			//  			meshInfo.buffer.end(),
			//  			vertex.begin() + VertexSize * (index + 0), 
			//  			vertex.begin() + VertexSize * (index + 1)
			//  		);
			//  	}
			//  }
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