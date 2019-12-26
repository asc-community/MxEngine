#include "ObjectLoader.h"
#include "Utilities/Logger/Logger.h"
#include <fstream>

namespace MomoEngine
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
	Logger::Instance().Error("MomoEngine::ObjectLoader", "error occured on line " + std::to_string(obj.lineCount));\
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
				Logger::Instance().Error("MomoEngine::ObjectLoader",
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
			Logger::Instance().Error("MomoEngine::ObjectLoader",
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
				Logger::Instance().Error("MomoEngine::ObjectLoader",
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
			Logger::Instance().Error("MomoEngine::ObjectLoader",
				"error while loading object file: normal index undefined"
			);
			INVOKE_ERR;
			return;
		}
	}

	MaterialLibrary ObjectLoader::LoadMaterialLibrary(const std::string& path)
	{
		MaterialLibrary library;
		std::ifstream fs(path);
		if (fs.bad() || fs.fail())
		{
			Logger::Instance().Error("MomoEngine::ObjectLoader", "material library was not loaded: " + path);
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
				Logger::Instance().Error("MomoEngine::ObjectLoader", "material library was not loaded: " + path);
				return library;
			}
		}
		Logger::Instance().Debug("MomoEngine::ObjectLoader", "material library loaded: " + path);
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
				std::getline(file, material.map_bump);
				material.map_bump = trim(material.map_bump);
			}
			else if (data == "bump")
			{
				std::getline(file, material.bump);
				material.bump = trim(material.bump);
			}
			else
			{
				Logger::Instance().Error("MomoEngine::ObjectLoader", "unresolved symbol in material file: " + data);
				material.IsSuccess = false;
				return material;
			}
		}
		return material;
	}

	ObjFileInfo MomoEngine::ObjectLoader::Load(const std::string& path)
	{
		auto CheckGroup = [](ObjFileInfo& object)
		{
			if (object.groups.empty())
			{
				Logger::Instance().Warning("MomoEngine::ObjectLoader",
					"group was not specifies, creating one as dafault. line: " + std::to_string(object.lineCount));
				object.groups.emplace_back();
				GROUP(object).name = "__DEFAULT";
			}
		};

		std::ifstream fs(path);
		ObjFileInfo obj;
		if (fs.bad() || fs.fail())
		{
			Logger::Instance().Error("MomoEngine::ObjectLoader", "object file was not found: " + path);
			INVOKE_ERR;
			return obj;
		}
		std::vector<glm::vec3> verteces;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> texCoords;
		std::string type, strBuff;
		std::stringstream file, tmpBuff;
		file << fs.rdbuf();
		while (file >> type)
		{
			obj.lineCount++;
			if (type == "v")
			{
				glm::vec3 v;
				file >> v.x >> v.y >> v.z;
				verteces.push_back(v);
			}
			else if (type == "vt")
			{
				glm::vec3 vt(0.0f);
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
				glm::vec3 vn;
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
					Logger::Instance().Error("MomoEngine::ObjectLoader", "material was not found in library: " + strBuff);
					INVOKE_ERR;
					return obj;
				}
				CheckGroup(obj);
				GROUP(obj).material = &obj.materials[strBuff];
			}
			else if (type == "mtllib")
			{
				file >> strBuff; 
				obj.materials = LoadMaterialLibrary(strBuff);
			}
			else if (type == "me_usetex") // MomoEngine extension
			{
				file >> strBuff;
				CheckGroup(obj);
				if (GROUP(obj).material == nullptr)
				{
					GROUP(obj).material = new MaterialInfo(); // memory leak. added for testing
					GROUP(obj).material->map_Ka = strBuff;
				}
			}
			else
			{
				Logger::Instance().Error("MomoEngine::ObjectLoader", "unexpected symbol in object file: " + type);
				INVOKE_ERR;
				return obj;
			}
		}
		for(auto& group : obj.groups)
		{
			group.buffer.reserve(group.faces.size() * 3);
			for (const auto& face : group.faces)
			{
				auto vertex = face.x;
				auto texture = face.y;
				auto normal = face.z;
				if (vertex < 0 || vertex >= verteces.size())
				{
					Logger::Instance().Error("MomoEngine::ObjectLoader",
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
					if (texture < 0 || texture >= texCoords.size())
					{
						Logger::Instance().Error("MomoEngine::ObjectLoader",
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
					if (normal < 0 || normal >= normals.size())
					{
						Logger::Instance().Error("MomoEngine::ObjectLoader",
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