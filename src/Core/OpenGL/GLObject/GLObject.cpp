#include "GLObject.h"
#include "Utilities/Logger/Logger.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

namespace MomoEngine
{
	VertexArray GLObject::LoadFromFile(const std::string& filepath)
	{
		std::vector<GLfloat> buffer;
		std::ifstream fs(filepath);
		if (fs.bad())
		{
			Logger::Instance().Warning("MomoEngine::GLObject", "object file was not found: " + filepath);
			return VertexArray();
		}
		std::vector<glm::vec3> verteces;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::string type;
		std::stringstream file; 
		file << fs.rdbuf();
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
				Logger::Instance().Error("MomoEngine::GLObject",
					"parameter space vertices are not supported by now");
				vertexCount = 0;
				buffer.clear();
				return VertexArray();
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
						Logger::Instance().Error("MomoEngine::GLObject",
							"error while loading object file: vertex index too big: " + v);
						vertexCount = 0;
						buffer.clear();
						return VertexArray();
					}
					buffer.push_back(verteces[vertexIndex].x);
					buffer.push_back(verteces[vertexIndex].y);
					buffer.push_back(verteces[vertexIndex].z);
					face.erase(0, v.size() + 1);
					if (face.empty())
					{
						if (this->useTexture)
						{
							Logger::Instance().Error("MomoEngine::GLObject",
								"error while loading object file: texture index undefined"
							);
							vertexCount = 0;
							buffer.clear();
							return VertexArray();
						}
						else
							continue;
					}
					// texture data
					auto vt = face.substr(0, face.find('/'));
					if (!vt.empty())
					{
						this->useTexture = true;
						size_t textureIndex = std::stoul(vt) - 1;
						if (textureIndex >= texCoords.size())
						{
							Logger::Instance().Error("MomoEngine::GLObject",
								"error while loading object file: texture index too big: " + vt);
							vertexCount = 0;
							buffer.clear();
							return VertexArray();
						}
						buffer.push_back(texCoords[textureIndex].x);
						buffer.push_back(texCoords[textureIndex].y);
					}
					else if (this->useTexture)
					{
						Logger::Instance().Error("MomoEngine::GLObject",
							"error while loading object file: texture index undefined"
						);
						vertexCount = 0;
						buffer.clear();
						return VertexArray();
					}
					face.erase(0, vt.size() + 1);
					if (face.empty())
					{
						if (this->useNormal)
						{
							Logger::Instance().Error("MomoEngine::GLObject",
								"error while loading object file: normal index undefined"
							);
							vertexCount = 0;
							buffer.clear();
							return VertexArray();
						}
						else
							continue;
					}

					// normal data
					auto vn = face;
					if (!vn.empty())
					{
						this->useNormal = true;
						size_t normalIndex = std::stoul(vn) - 1;
						if (normalIndex >= normals.size())
						{
							Logger::Instance().Error("MomoEngine::GLObject",
								"error while loading object file: normal index too big: " + vn);
							vertexCount = 0;
							buffer.clear();
							return VertexArray();
						}
						buffer.push_back(normals[normalIndex].x);
						buffer.push_back(normals[normalIndex].y);
						buffer.push_back(normals[normalIndex].z);
					}
					else if (this->useNormal)
					{
						Logger::Instance().Error("MomoEngine::GLObject",
							"error while loading object file: normal index undefined"
						);
						vertexCount = 0;
						buffer.clear();
						return VertexArray();
					}
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
				Logger::Instance().Error("MomoEngine::GLObject", "unexpected symbol in object file: " + type);
				vertexCount = 0;
				buffer.clear();
				return VertexArray();
			}
		}
		#ifdef _DEBUG
		this->buffer = buffer;
		#endif
		return LoadFromBuffer(buffer, this->useTexture, this->useNormal);
	}

	VertexArray GLObject::LoadFromBuffer(const std::vector<GLfloat>& buffer, bool useTexture, bool useNormal)
	{
		VertexArray VAO;
		this->VBO = MakeUnique<VertexBuffer>(buffer, UsageType::STATIC_DRAW);
		VertexBufferLayout VBL;
		VBL.PushFloat(3);
		if (useTexture) VBL.PushFloat(2);
		if (useNormal) VBL.PushFloat(3);
		VAO.AddBuffer(*VBO, VBL);
		#ifdef _DEBUG
		this->buffer = buffer;
		#endif
		return VAO;
	}

	GLObject::GLObject()
		: useTexture(false), useNormal(false), vertexCount(0), VAO()
	{
	}

	GLObject::GLObject(const std::string& filepath)
		: useTexture(false), useNormal(false), vertexCount(0), VAO(LoadFromFile(filepath))
	{
		
	}

	GLObject::GLObject(const std::vector<GLfloat>& bufferSource, size_t vertexCount, bool useTexture, bool useNormal)
		: useTexture(useTexture), useNormal(useNormal), vertexCount(vertexCount), VAO(LoadFromBuffer(bufferSource, useTexture, useNormal)) { }

	void GLObject::Load(const std::string& filepath)
	{
		this->VAO = LoadFromFile(filepath);
	}

	void GLObject::Load(const std::vector<GLfloat>& buffer, size_t vertexCount, bool useTexture, bool useNormal)
	{
		this->VAO = LoadFromBuffer(buffer, useTexture, useNormal);
		this->vertexCount = vertexCount;
	}

	VertexArray& GLObject::GetVertexData()
	{
		return this->VAO;
	}

	std::vector<unsigned int> GLObject::GeneratePolygonIndicies() const
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

	bool GLObject::HasTextureData() const
	{
		return this->useTexture;
	}

	bool GLObject::HasNormalData() const
	{
		return this->useNormal;
	}

	GLInstance::GLInstance()
	{
		this->Model = glm::mat4x4(1.0f);
	}

	GLInstance::GLInstance(const Ref<GLObject>& object)
	{
		this->Model = glm::mat4x4(1.0f);
		Load(object);
	}

	void GLInstance::Load(const Ref<GLObject>& object)
	{
		this->object = object;
	}

	Ref<GLObject>& GLInstance::GetGLObject()
	{
		return this->object;
	}

	const Ref<GLObject>& GLInstance::GetGLObject() const
	{
		return this->object;
	}

	GLInstance& GLInstance::Scale(float scale)
	{
		return Scale(glm::vec3(scale));
	}

	GLInstance& GLInstance::Scale(float scaleX, float scaleY, float scaleZ)
	{
		return Scale(glm::vec3(scaleX, scaleY, scaleZ));
	}

	GLInstance& GLInstance::Scale(const glm::vec3& scale)
	{
		this->Model = glm::scale(this->Model, scale);
		return *this;
	}

	GLInstance& GLInstance::Rotate(float angle, const glm::vec3& vec)
	{
		this->Model = glm::rotate(this->Model, angle, vec);
		return *this;
	}

	GLInstance& GLInstance::RotateX(float angle)
	{
		return Rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	GLInstance& GLInstance::RotateY(float angle)
	{
		return Rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	GLInstance& GLInstance::RotateZ(float angle)
	{
		return Rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	GLInstance& GLInstance::Translate(float x, float y, float z)
	{
		this->Model = glm::translate(this->Model, glm::vec3(x, y, z));
		return *this;
	}

	GLInstance& GLInstance::TranslateX(float x)
	{
		return Translate(x, 0.0f, 0.0f);
	}

	GLInstance& GLInstance::TranslateY(float y)
	{
		return Translate(0.0f, y, 0.0f);
	}

	GLInstance& GLInstance::TranslateZ(float z)
	{
		return Translate(0.0f, 0.0f, z);
	}
} 