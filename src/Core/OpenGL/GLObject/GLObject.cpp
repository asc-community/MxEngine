#include "GLObject.h"
#include "../Utilities/Logger/Logger.h"
#include <fstream>

namespace MomoEngine
{
	std::vector<GLObject::GLfloat> GLObject::LoadFromFile(const std::string& filepath)
	{
		std::vector<GLfloat> buffer;
		std::ifstream file(filepath);
		if (file.bad())
		{
			Logger::Get().Warning("glObject", "object file was not found: " + filepath);
			return buffer;
		}
		std::vector<glm::vec3> verteces;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::string type;
		while (file >> type)
		{
			if(type == "v")
			{
				glm::vec3 v;
				file >> v.x >> v.y >> v.z;
				verteces.push_back(v);
			}
			else if (type == "vt")
			{
				glm::vec2 vt;
				file >> vt.x >> vt.y;
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
				Logger::Get().Error("globject",
					"parameter space vertices are not supported by now");
				vertexCount = 0;
				buffer.clear();
				return buffer;
			}
			else if (type == "f")
			{
				for (int i = 0; i < 3; i++)
				{
					this->vertexCount++;
					std::string face;
					file >> face;
					// vertex data
					auto v = face.substr(0, face.find('/'));
					size_t vertexIndex = std::stoul(v) - 1;
					if (vertexIndex >= verteces.size())
					{
						Logger::Get().Error("globject",
							"error while loading object file: vertex index too big: " + v);
						vertexCount = 0;
						buffer.clear();
						return buffer;
					}
					buffer.push_back(verteces[vertexIndex].x);
					buffer.push_back(verteces[vertexIndex].y);
					buffer.push_back(verteces[vertexIndex].z);
					face.erase(0, v.size() + 1);
					if (face.empty()) continue;

					// texture data
					auto vt = face.substr(0, face.find('/'));
					size_t textureIndex = std::stoul(vt) - 1;
					if (textureIndex >= texCoords.size())
					{
						Logger::Get().Error("globject",
							"error while loading object file: texture too big: " + vt);
						vertexCount = 0;
						buffer.clear();
						return buffer;
					}
					buffer.push_back(texCoords[textureIndex].x);
					buffer.push_back(texCoords[textureIndex].y);
					face.erase(0, vt.size() + 1);
					if (face.empty()) continue;

					// normal data
					auto vn = face;
					size_t normalIndex = std::stoul(vn) - 1;
					if (normalIndex >= normals.size())
					{
						Logger::Get().Error("globject",
							"error while loading object file: normal too big: " + vn);
						vertexCount = 0;
						buffer.clear();
						return buffer;
					}
					buffer.push_back(normals[normalIndex].x);
					buffer.push_back(normals[normalIndex].y);
					buffer.push_back(normals[normalIndex].z);
				}
			}
			else if (type == "0")
			{
				continue; // extra parameter from vt
			}
			else if (type[0] == '#')
			{
				getline(file, type); // read comment
			}
			else
			{
				Logger::Get().Error("globject", "unexpected symbol in object file: " + type);
				vertexCount = 0;
				buffer.clear();
				return buffer;
			}
		}
		#ifdef _DEBUG
		this->buffer = buffer;
		#endif
		return buffer;
	}

	GLObject::GLObject(const std::string& filepath)
		: vertexCount(0), VBO(LoadFromFile(filepath))
	{
		
	}

	GLObject::GLObject(const std::vector<GLfloat>& bufferSource, size_t vertexCount)
		: vertexCount(vertexCount), VBO(bufferSource) { }

	VertexBuffer& GLObject::GetBuffer()
	{
		return this->VBO;
	}
	std::vector<GLuint> GLObject::GeneratePolygonIndicies() const
	{
		std::vector<GLuint> indicies;
		indicies.reserve(this->vertexCount * 3);
		for (int i = 0; i < this->vertexCount; i += 3)
		{
			indicies.push_back(i + 0);
			indicies.push_back(i + 1);
			indicies.push_back(i + 1);
			indicies.push_back(i + 2);
			indicies.push_back(i + 2);
			indicies.push_back(i + 0);
		}
		return indicies;
	}

	size_t GLObject::GetVertexCount() const
	{
		return this->vertexCount;
	}
}