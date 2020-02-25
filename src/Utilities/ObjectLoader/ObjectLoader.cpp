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

#include <fstream>
#include <algorithm>

namespace MxEngine
{
	static std::string trim(const std::string& str)
	{
		if (str.empty()) return str;
		std::string res;
		size_t begin = 0, end = str.size() - 1;
		while (begin < str.size() && std::isspace(str[begin])) begin++;
		while (end > 0 && std::isspace(str[end])) end--;
		if (begin > end) return "";
		return str.substr(begin, end - begin + 1);
	}

	#define INVOKE_ERR \
	Logger::Instance().Error("MxEngine::MxObjectLoader", "error occured on line " + std::to_string(obj.lineCount));\
	obj.isSuccess = false

	void ObjectLoader::ReadFace(std::stringstream& file, ObjFileInfo& obj)
	{
		#define GROUP(object) object.groups.back()
		GROUP(obj).faces.emplace_back(0);
		std::string face;
		file >> face;
		// vertex data
		auto v = face.substr(0, face.find('/'));
		long long vertexIndex = std::stoll(v) - 1;
		GROUP(obj).faces.back().x = vertexIndex;
		face.erase(0, v.size() + 1);
		if (face.empty())
		{
			if (GROUP(obj).useTexture)
			{
				Logger::Instance().Error("MxEngine::MxObjectLoader",
					"error while loading object file: texture index undefined"
				);
				INVOKE_ERR;
			}
			return;
		}
		// texture data
		auto vt = face.substr(0, face.find('/'));
		if (!vt.empty())
		{
			GROUP(obj).useTexture = true;
			long long textureIndex = std::stoll(vt) - 1;
			GROUP(obj).faces.back().y = textureIndex;
		}
		else if (GROUP(obj).useTexture)
		{
			Logger::Instance().Error("MxEngine::MxObjectLoader",
				"error while loading object file: texture index undefined"
			);
			INVOKE_ERR;
			return;
		}
		face.erase(0, vt.size() + 1);
		if (face.empty())
		{
			if (GROUP(obj).useNormal)
			{
				Logger::Instance().Error("MxEngine::MxObjectLoader",
					"error while loading object file: normal index undefined"
				);
				INVOKE_ERR;
			}
			return;
		}

		// normal data
		auto vn = face;
		if (!vn.empty())
		{
			GROUP(obj).useNormal = true;
			long long normalIndex = std::stoll(vn) - 1;
			GROUP(obj).faces.back().z = normalIndex;
		}
		else if (GROUP(obj).useNormal)
		{
			Logger::Instance().Error("MxEngine::MxObjectLoader",
				"error while loading object file: normal index undefined"
			);
			INVOKE_ERR;
			return;
		}
	}

	MaterialLibrary ObjectLoader::LoadMaterialLibrary(const std::string& path)
	{
		MAKE_SCOPE_PROFILER("ObjectLoader::LoadMaterialLibrary");
		MAKE_SCOPE_TIMER("MxEngine::MxObjectLoader", "ObjectLoader::LoadMaterialLibrary()");
		Logger::Instance().Debug("MxEngine::MxObjectLoader", "loading material library from file: " + path);
		MaterialLibrary library;
		std::ifstream fs(path);
		if (fs.bad() || fs.fail())
		{
			Logger::Instance().Error("MxEngine::MxObjectLoader", "could not open file: " + path);
			return library;
		}
		std::stringstream file;
		file << fs.rdbuf();
		std::string buff;
		while (!file.eof())
		{
			std::string name;
			file >> name; // name
			if (name[0] == '#')
			{
				std::getline(file, buff); // comment
			}
			else if(!name.empty())
			{
				if (name == "newmtl") file >> name;
				MaterialInfo material = LoadMaterial(file);
				if (!material.IsSuccess)
					return MaterialLibrary();
				library[name] = material;
			}
			else
			{
				Logger::Instance().Error("MxEngine::MxObjectLoader", "material library was not loaded: " + path);
				return library;
			}
		}
		return library;
	}

	MaterialInfo ObjectLoader::LoadMaterial(std::stringstream& file)
	{
		MaterialInfo material;
		std::string data;
		while (file >> data)
		{
			if (data == "newmtl")
				break;
			else if (data[0] == '#')
			{
				std::getline(file, data); // comment
			}
			else if (data == "Ns")
			{
				file >> material.Ns;
			}
			else if (data == "Ni")
			{
				file >> material.Ni;
			}
			else if (data == "d")
			{
				file >> material.d;
			}
			else if (data == "Tr")
			{
				file >> material.Tr;
			}
			else if (data == "Tf")
			{
				file >> material.Tf.x;
				file >> material.Tf.y;
				file >> material.Tf.z;
			}
			else if (data == "illum")
			{
				file >> material.illum;
			}
			else if (data == "Ka")
			{
				file >> material.Ka.x;
				file >> material.Ka.y;
				file >> material.Ka.z;
			}
			else if (data == "Kd")
			{
				file >> material.Kd.x;
				file >> material.Kd.y;
				file >> material.Kd.z;
			}
			else if (data == "Ks")
			{
				file >> material.Ks.x;
				file >> material.Ks.y;
				file >> material.Ks.z;
			}
			else if (data == "Ke")
			{
				file >> material.Ke.x;
				file >> material.Ke.y;
				file >> material.Ke.z;
			}
			else if (data == "map_Ka")
			{
				std::getline(file, material.map_Ka);
				material.map_Ka = trim(material.map_Ka);
			}
			else if (data == "map_Kd")
			{
				std::getline(file, material.map_Kd);
				material.map_Kd = trim(material.map_Kd);
			}
			else if (data == "map_Ks")
			{
				std::getline(file, material.map_Ks);
				material.map_Ke = trim(material.map_Ks);
			}
			else if (data == "map_Ke")
			{
				std::getline(file, material.map_Ke);
				material.map_Ke = trim(material.map_Ke);
			}
			else if (data == "map_d")
			{
				std::getline(file, material.map_d);
				material.map_d = trim(material.map_d);
			}
			else if (data == "map_bump")
			{
				std::string bumpInfo;
				file >> bumpInfo;
				if (bumpInfo == "-bm")
				{
					file >> material.bm;
					std::getline(file, material.map_bump);
					material.map_bump = trim(material.map_bump);
				}
				else // bump info was not specified, read filename accidentally
				{
					std::getline(file, material.map_bump);
					material.map_bump = trim(material.map_bump);
					material.map_bump = bumpInfo + ' ' + material.map_bump;
				}
			}
			else if (data == "bump")
			{
				std::getline(file, material.bump);
				material.bump = trim(material.bump);
			}
			else
			{
				Logger::Instance().Error("MxEngine::MxObjectLoader", "unresolved symbol in material file: " + data);
				material.IsSuccess = false;
				return material;
			}
		}
		return material;
	}

	ObjFileInfo MxEngine::ObjectLoader::Load(const std::string& path)
	{
		MAKE_SCOPE_PROFILER("ObjectLoader::Load");
		Logger::Instance().Debug("MxEngine::MxObjectLoader", "loading object from file: " + path);
		auto CheckGroup = [](ObjFileInfo& object)
		{
			if (object.groups.empty())
			{
				Logger::Instance().Warning("MxEngine::MxObjectLoader",
					"group was not specifies, creating one as dafault. line: " + std::to_string(object.lineCount));
				object.groups.emplace_back();
				GROUP(object).name = "__DEFAULT";
			}
		};
		std::ifstream fs(path);
		ObjFileInfo obj;
		if (fs.bad() || fs.fail())
		{
			Logger::Instance().Error("MxEngine::MxObjectLoader", "object file was not found: " + path);
			INVOKE_ERR;
			return obj;
		}
		std::vector<Vector3> verteces;
		std::vector<Vector3> normals;
		std::vector<Vector3> texCoords;
		std::string type, strBuff;
		std::stringstream file, tmpBuff;
		file << fs.rdbuf();
		while (file >> type)
		{
			obj.lineCount++;
			if (type == "v")
			{
				Vector3 v;
				file >> v.x >> v.y >> v.z;
				verteces.push_back(v);
			}
			else if (type == "vt")
			{
				Vector3 vt(0.0f);
				file >> vt.x >> vt.y;
				std::getline(file, type);
				type = trim(type);
				if (!type.empty())
				{
					vt.z = std::stof(type);
				}
				texCoords.push_back(vt);
			}
			else if (type == "vn")
			{
				Vector3 vn;
				file >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}
			else if (type == "vp")
			{
				std::getline(file, strBuff);
			}
			else if (type == "f")
			{
				CheckGroup(obj);
				for (int i = 0; i < 3; i++)
				{
					ReadFace(file, obj);
					if(!obj.isSuccess) return obj;
				}
				std::getline(file, strBuff);
				strBuff = trim(strBuff);
				if (!strBuff.empty())
				{
					tmpBuff << strBuff;
					ReadFace(tmpBuff, obj);
					if (!obj.isSuccess) return obj;
					tmpBuff.clear();
					// transform {..., f1, f2, f3, f4 } ->
					// {..., f1, f2, f3, f3, f4, f1 }
					auto end = GROUP(obj).faces.size();
					auto f4 = GROUP(obj).faces[end - 1];
					auto f3 = GROUP(obj).faces[end - 2];
					auto f2 = GROUP(obj).faces[end - 3];
					auto f1 = GROUP(obj).faces[end - 4];
					GROUP(obj).faces.pop_back(); // pop f4
					GROUP(obj).faces.push_back(f3);
					GROUP(obj).faces.push_back(f4);
					GROUP(obj).faces.push_back(f1);
				}
			}
			else if (type[0] == '#')
			{
				getline(file, strBuff); // read comment
			}
			else if (type == "s")
			{
				file >> strBuff; // smoothing group id / 'off'
			}
			else if (type == "g")
			{
				file >> strBuff; // group name
				obj.groups.emplace_back();
				GROUP(obj).name = strBuff;
			}
			else if (type == "usemtl")
			{
				file >> strBuff; // mtl file
				if (obj.materials.find(strBuff) == obj.materials.end())
				{
					Logger::Instance().Error("MxEngine::MxObjectLoader", "material was not found in library: " + strBuff);
					INVOKE_ERR;
					return obj;
				}
				CheckGroup(obj);
				if (GROUP(obj).material != nullptr)
				{
					obj.groups.emplace_back();
					GROUP(obj).name = "__GENERATED";
				}
				GROUP(obj).material = &obj.materials[strBuff];
			}
			else if (type == "mtllib")
			{
				file >> strBuff; 
				obj.materials = LoadMaterialLibrary(strBuff);
			}
			else
			{
				Logger::Instance().Error("MxEngine::MxObjectLoader", "unexpected symbol in object file: " + type);
				INVOKE_ERR;
				return obj;
			}
		}

		Vector3 maxCoords(-1.0f * std::numeric_limits<float>::max());
		Vector3 minCoords(std::numeric_limits<float>::max());
		for (const auto& vertex : verteces)
		{
			minCoords.x = std::min(minCoords.x, vertex.x);
			minCoords.y = std::min(minCoords.y, vertex.y);
			minCoords.z = std::min(minCoords.z, vertex.z);

			maxCoords.x = std::max(maxCoords.x, vertex.x);
			maxCoords.y = std::max(maxCoords.y, vertex.y);
			maxCoords.z = std::max(maxCoords.z, vertex.z);
		}
		Vector3 center = (maxCoords + minCoords) * 0.5f;
		for (auto& vertex : verteces)
		{
			vertex -= center;
		}

		for(auto& group : obj.groups)
		{
			group.buffer.reserve(group.faces.size() * 3);
			for (const auto& face : group.faces)
			{
				auto vertex = face.x;
				auto texture = face.y;
				auto normal = face.z;
				if (vertex < 0 || (size_t)vertex >= verteces.size())
				{
					Logger::Instance().Error("MxEngine::MxObjectLoader",
						"vertex index is invalid: " + std::to_string(vertex)
					);
					INVOKE_ERR;
					return obj;
				}
				group.buffer.push_back(verteces[vertex].x);
				group.buffer.push_back(verteces[vertex].y);
				group.buffer.push_back(verteces[vertex].z);
				if (group.useTexture)
				{
					if (texture < 0 || (size_t)texture >= texCoords.size())
					{
						Logger::Instance().Error("MxEngine::MxObjectLoader",
							"texture index is invalid: " + std::to_string(texture)
						);
						INVOKE_ERR;
						return obj;
					}
					group.buffer.push_back(texCoords[texture].x);
					group.buffer.push_back(texCoords[texture].y);
					group.buffer.push_back(texCoords[texture].z);
				}
				if (group.useNormal)
				{
					if (normal < 0 || (size_t)normal >= normals.size())
					{
						Logger::Instance().Error("MxEngine::MxObjectLoader",
							"normal index is invalid: " + std::to_string(normal)
						);
						INVOKE_ERR;
						return obj;
					}
					group.buffer.push_back(normals[normal].x);
					group.buffer.push_back(normals[normal].y);
					group.buffer.push_back(normals[normal].z);
				}
			}
		}
		return obj;
	}
}