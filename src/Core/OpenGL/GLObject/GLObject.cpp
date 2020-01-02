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
				if (group.material != nullptr)
				{
					#define MAKE_TEX(tex) if(!group.material->tex.empty()) {\
					if(textures.find(group.material->tex) == textures.end())\
						textures[group.material->tex] = MakeRef<Texture>(group.material->tex);\
					object.material->tex = textures[group.material->tex];}

					object.material = MakeUnique<Material>();

					MAKE_TEX(map_Ka);
					MAKE_TEX(map_Kd);
					MAKE_TEX(map_Ks);
					MAKE_TEX(map_Ke);
					MAKE_TEX(map_d);
					MAKE_TEX(map_bump);
					MAKE_TEX(bump);

					object.material->Tf = group.material->Tf;
					object.material->Ka = group.material->Ka;
					object.material->Kd = group.material->Kd;
					object.material->Ke = group.material->Ke;
					object.material->Ks = group.material->Ks;
					object.material->illum = group.material->illum;
					object.material->Ns = group.material->Ns;
					object.material->Ni = group.material->Ni;
					object.material->d = group.material->d;
					object.material->Tr = group.material->Tr;

					if (object.material->Ns == 0.0f) object.material->Ns = 128.0f; // bad as pow(0.0, 0.0) -> NaN
				}
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

	void GLInstance::AddInstanceBuffer(const std::vector<float>& buffer, size_t count, UsageType type)
	{
		if (this->object == nullptr)
		{
			Logger::Instance().Warning("MomoEngine::GLInstance", "trying to add buffer to nit existing object");
			return;
		}

		size_t objectCount = buffer.size() / count;

		if (this->instanceCount == 0) this->instanceCount = objectCount;
		if (this->instanceCount > objectCount)
		{
			Logger::Instance().Error("MomoEngine::GLInstance", "instance buffer was not added as it contains not enough information");
			return;
		}
		else if (this->instanceCount < objectCount)
		{
			Logger::Instance().Error("MomoEngine::GLInstance", "instance buffer contains more data than required, additional will be ignored");
		}

		for (auto& object : this->object->GetRenderObjects())
		{
			object.VBOs.emplace_back(buffer, type);
			VertexBufferLayout VBL;
			VBL.PushFloat(count);
			object.VAO.AddInstancedBuffer(object.VBOs.back(), VBL);
		}
	}

	void GLInstance::AddInstanceBuffer(const GeneratorFunction& generator, size_t componentCount, size_t count, UsageType type)
	{
		if (this->object == nullptr)
		{
			Logger::Instance().Warning("MomoEngine::GLInstance", "trying to add buffer to nit existing object");
			return;
		}
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

	void GLInstance::Hide()
	{
		this->shouldRender = false;
	}

	void GLInstance::Show()
	{
		this->shouldRender = true;
	}

	glm::vec3 GLInstance::GetTranslation() const
	{
		return this->translation;
	}

	glm::vec3 GLInstance::GetRotation() const
	{
		return this->rotation;
	}

	glm::vec3 GLInstance::GetScale() const
	{
		return this->scale;
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
		angle = glm::radians(angle);
		needUpdate = true;
		this->rotation += vec * angle;

		// check if rotation value is too high to avoid precision loss
		if (rotation.x > glm::two_pi<float>()) rotation.x -= glm::two_pi<float>();
		if (rotation.y > glm::two_pi<float>()) rotation.y -= glm::two_pi<float>();
		if (rotation.z > glm::two_pi<float>()) rotation.z -= glm::two_pi<float>();
		if (rotation.x < 0.0f) rotation.x += glm::two_pi<float>();
		if (rotation.y < 0.0f) rotation.y += glm::two_pi<float>();
		if (rotation.z < 0.0f) rotation.z += glm::two_pi<float>();

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
			auto Transalte = glm::translate(glm::mat4x4(1.0f), this->translation);
			auto RotateX = glm::rotate(glm::mat4x4(1.0f), this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			auto RotateY = glm::rotate(glm::mat4x4(1.0f), this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			auto RotateZ = glm::rotate(glm::mat4x4(1.0f), this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
			auto Scale = glm::scale(glm::mat4x4(1.0f), this->scale);
			this->Model = Transalte * RotateX * RotateY * RotateZ * Scale;
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

	bool GLInstance::IsDrawable() const
	{
		return this->shouldRender && this->object != nullptr;
	}

	bool GLInstance::HasTexture() const
	{
		return this->Texture != nullptr;
	}

	const MomoEngine::Texture& GLInstance::GetTexture() const
	{
		return *this->Texture;
	}

	size_t GLInstance::GetInstanceCount() const
	{
		return this->instanceCount;
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

	const Material& GLRenderObject::GetMaterial() const
	{
		return *this->material;
	}

	size_t GLRenderObject::GetVertexCount() const
	{
		return this->vertexCount;
	}

	bool GLRenderObject::HasMaterial() const
	{
		return this->material != nullptr;
	}
}