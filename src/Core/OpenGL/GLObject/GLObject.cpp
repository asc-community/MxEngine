#include "GLObject.h"
#include "Utilities/Logger/Logger.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "Utilities/Time/Time.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"

namespace MomoEngine
{
	void GLObject::LoadFromFile(const std::string& filepath)
	{
		TimeStep objBeginLoad = Time::Current();
		auto objectInfo = ObjectLoader::Load(filepath);
		TimeStep objEndLoad = Time::Current();
		if (objectInfo.isSuccess)
		{
			Logger::Instance().Debug("MomoEngine::GLObject", "object loaded from file: " +
				filepath + " in " + BeautifyTime(objEndLoad - objBeginLoad));
		}
		else
		{
			Logger::Instance().Debug("MomoEngine::GLObject", "failed to load object from file: " +
				filepath + " in " + BeautifyTime(objEndLoad - objBeginLoad));
		}
		this->subObjects.reserve(objectInfo.groups.size());
		std::unordered_map<std::string, Ref<Texture>> textures;
		for (const auto& group : objectInfo.groups)
		{
			GLRenderObject object;
			object.vertexCount = group.faces.size();
			object.useTexture = group.useTexture;
			object.useNormal = group.useNormal;
			object.VBOs.emplace_back(group.buffer, UsageType::STATIC_DRAW);

			VertexBufferLayout VBL;
			VBL.PushFloat(3);
			if (group.useTexture) VBL.PushFloat(3);
			if (group.useNormal) VBL.PushFloat(3);
			object.VAO.AddBuffer(object.VBOs.back(), VBL);

			if (group.useTexture)
			{
				if (group.material != nullptr && textures.find(group.material->map_Ka) == textures.end())
					textures[group.material->map_Ka] = MakeRef<Texture>(group.material->map_Ka);
				object.Texture = textures[group.material->map_Ka];
			}

			this->subObjects.push_back(std::move(object));
		}
	}

	GLObject::GLObject(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	void GLObject::Load(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	std::vector<GLRenderObject>& GLObject::GetRenderObjects()
	{
		return this->subObjects;
	}

	void GLObject::AddInstanceBuffer(const std::vector<float>& buffer, size_t count, UsageType type)
	{
		for (auto& object : this->subObjects)
		{
			object.VBOs.emplace_back(buffer, type);
			VertexBufferLayout VBL;
			VBL.PushFloat(count);
			object.VAO.AddInstancedBuffer(object.VBOs.back(), VBL);
		}
	}

	void GLObject::AddInstanceBuffer(const GeneratorFunction& generator, size_t componentCount, size_t count, UsageType type)
	{
		std::vector<float> buffer;
		buffer.reserve(componentCount * count);
		for (size_t i = 0; i < count; i++)
		{
			for (size_t j = 0; j < componentCount; j++)
			{
				buffer.push_back(generator(int(i), int(j)));
			}
		}
		this->AddInstanceBuffer(buffer, componentCount, type);
	}

	GLInstance::GLInstance()
	{
		
	}

	GLInstance::GLInstance(const Ref<GLObject>& object)
	{
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
		needUpdate = true;
		this->scale.x *= scale.x;
		this->scale.y *= scale.y;
		this->scale.z *= scale.z;
		return *this;
	}

	GLInstance& GLInstance::Rotate(float angle, const glm::vec3& vec)
	{
		needUpdate = true;
		this->rotation += vec * angle;
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
		needUpdate = true;
		this->translation += glm::vec3(x, y, z);
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

	size_t GLInstance::GetIterator() const
	{
		return 0;
	}

	bool GLInstance::IsLast(size_t iterator) const
	{
		return this->GetGLObject()->GetRenderObjects().size() == iterator;
	}

	size_t GLInstance::GetNext(size_t iterator) const
	{
		return iterator + 1;
	}

	const IRenderable& GLInstance::GetCurrent(size_t iterator) const
	{
		return this->GetGLObject()->GetRenderObjects()[iterator];
	}

	const glm::mat4x4& GLInstance::GetModel() const
	{
		if (needUpdate)
		{
			this->Model = glm::mat4x4(1.0f);
			this->Model = glm::translate(this->Model, this->translation);
			this->Model = glm::rotate(this->Model, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			this->Model = glm::rotate(this->Model, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			this->Model = glm::rotate(this->Model, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
			this->Model = glm::scale(this->Model, this->scale);
			needUpdate = false;
		}
		return this->Model;
	}

	bool GLInstance::HasShader() const
	{
		return this->Shader != nullptr;
	}

	const MomoEngine::Shader& GLInstance::GetShader() const
	{
		return *this->Shader;
	}

	void GLRenderObject::GenerateMeshIndicies() const
	{
		std::vector<unsigned int> indicies;
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
		this->IBO.Load(indicies);
		this->meshGenerated = true;
	}

	const VertexArray& GLRenderObject::GetVAO() const
	{
		return this->VAO;
	}

	const IndexBuffer& GLRenderObject::GetIBO() const
	{
		if (!this->meshGenerated) GenerateMeshIndicies();
		return this->IBO;
	}

	const Texture& GLRenderObject::GetTexture() const
	{
		return *this->Texture;
	}

	size_t GLRenderObject::GetVertexCount() const
	{
		return this->vertexCount;
	}

	bool GLRenderObject::HasTexture() const
	{
		return this->useTexture;
	}
}