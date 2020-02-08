#include "BaseObject.h"
#include "Utilities/Logger/Logger.h"
#include <fstream>
#include <sstream>
#include "Utilities/Time/Time.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
	void BaseObject::LoadFromFile(const std::string& filepath)
	{
        ObjFileInfo objectInfo;
        {
            MAKE_SCOPE_TIMER("MxEngine::BaseObject", "ObjectLoader::LoadFromFile()");
            objectInfo = ObjectLoader::Load(filepath);
        }

        MAKE_SCOPE_PROFILER("BaseObject::GenBuffers");
        MAKE_SCOPE_TIMER("MxEngine::BaseObject", "BaseObject::GenBuffers()");
		if (!objectInfo.isSuccess)
		{
            Logger::Instance().Debug("MxEngine::BaseObject", "failed to load object from file: " + filepath);
		}
		this->subObjects.reserve(objectInfo.groups.size());
        this->objectCenter = objectInfo.objectCenter;
		std::unordered_map<std::string, Ref<Texture>> textures;
		for (const auto& group : objectInfo.groups)
		{
			RenderObject object;
            object.VAO = Graphics::Instance()->CreateVertexArray();
			object.vertexCount = group.faces.size();
			object.useTexture = group.useTexture;
			object.useNormal = group.useNormal;
			object.VBOs.emplace_back(Graphics::Instance()->CreateVertexBuffer(group.buffer, UsageType::STATIC_DRAW));

            auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
			VBL->PushFloat(3);
			if (group.useTexture) VBL->PushFloat(3);
			if (group.useNormal) VBL->PushFloat(3);
			object.VAO->AddBuffer(*object.VBOs.back(), *VBL);

			if (group.useTexture)
			{
				if (group.material != nullptr)
				{
					#define MAKE_TEX(tex) if(!group.material->tex.empty()) {\
					if(textures.find(group.material->tex) == textures.end())\
						textures[group.material->tex] = Graphics::Instance()->CreateTexture(group.material->tex);\
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

	BaseObject::BaseObject(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	void BaseObject::Load(const std::string& filepath)
	{
		LoadFromFile(filepath);
	}

	std::vector<RenderObject>& BaseObject::GetRenderObjects()
	{
		return this->subObjects;
	}

    const Vector3& BaseObject::GetObjectCenter() const
    {
        return this->objectCenter;
    }

	void ObjectInstance::AddInstanceBuffer(const std::vector<float>& buffer, size_t count, UsageType type)
	{
		if (this->object == nullptr)
		{
			Logger::Instance().Warning("MxEngine::GLInstance", "trying to add buffer to nit existing object");
			return;
		}

		size_t objectCount = buffer.size() / count;

		if (this->instanceCount == 0) this->instanceCount = objectCount;
		if (this->instanceCount > objectCount)
		{
			Logger::Instance().Error("MxEngine::GLInstance", "instance buffer was not added as it contains not enough information");
			return;
		}
		else if (this->instanceCount < objectCount)
		{
			Logger::Instance().Error("MxEngine::GLInstance", "instance buffer contains more data than required, additional will be ignored");
		}

		for (auto& object : this->object->GetRenderObjects())
		{
			object.VBOs.emplace_back(Graphics::Instance()->CreateVertexBuffer(buffer, type));
            auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
			VBL->PushFloat(count);
			object.VAO->AddInstancedBuffer(*object.VBOs.back(), *VBL);
		}
	}

	void ObjectInstance::AddInstanceBuffer(const GeneratorFunction& generator, size_t componentCount, size_t count, UsageType type)
	{
		if (this->object == nullptr)
		{
			Logger::Instance().Warning("MxEngine::GLInstance", "trying to add buffer to nit existing object");
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

	ObjectInstance::ObjectInstance(const Ref<BaseObject>& object)
	{
		Load(object);
	}

	void ObjectInstance::Load(const Ref<BaseObject>& object)
	{
		this->object = object;
	}

	Ref<BaseObject>& ObjectInstance::GetBaseObject()
	{
		return this->object;
	}

	const Ref<BaseObject>& ObjectInstance::GetBaseObject() const
	{
		return this->object;
	}

	void ObjectInstance::Hide()
	{
		this->shouldRender = false;
	}

	void ObjectInstance::Show()
	{
		this->shouldRender = true;
	}

	const Vector3& ObjectInstance::GetTranslation() const
	{
		return this->translation;
	}

	const Quaternion& ObjectInstance::GetRotation() const
	{
		return this->rotation;
	}

	const Vector3& ObjectInstance::GetScale() const
	{
		return this->scale;
	}

	ObjectInstance& ObjectInstance::Scale(float scale)
	{
		return Scale(Vector3(scale));
	}

	ObjectInstance& ObjectInstance::Scale(float scaleX, float scaleY, float scaleZ)
	{
		return Scale(Vector3(scaleX, scaleY, scaleZ));
	}

	ObjectInstance& ObjectInstance::Scale(const Vector3& scale)
	{
		needUpdate = true;
		this->scale.x *= scale.x;
		this->scale.y *= scale.y;
		this->scale.z *= scale.z;
		return *this;
	}

	ObjectInstance& ObjectInstance::Rotate(float angle, const Vector3& vec)
	{
		needUpdate = true;
        this->rotation *= MakeQuaternion(Radians(angle), vec);
		return *this;
	}

	ObjectInstance& ObjectInstance::RotateX(float angle)
	{
		return Rotate(angle, Vector3(1.0f, 0.0f, 0.0f));
	}

	ObjectInstance& ObjectInstance::RotateY(float angle)
	{
		return Rotate(angle, Vector3(0.0f, 1.0f, 0.0f));
	}

	ObjectInstance& ObjectInstance::RotateZ(float angle)
	{
		return Rotate(angle, Vector3(0.0f, 0.0f, 1.0f));
	}

	ObjectInstance& ObjectInstance::Translate(float x, float y, float z)
	{
		needUpdate = true;
		this->translation += Vector3(x, y, z);
		return *this;
	}

    ObjectInstance& ObjectInstance::TranslateForward(float dist)
    {
        this->Translate(this->rotation * this->forwardVec * dist);
        return *this;
    }

    ObjectInstance& ObjectInstance::TranslateRight(float dist)
    {
        this->Translate(this->rotation * this->rightVec * dist);
        return *this;
    }

    ObjectInstance& ObjectInstance::TranslateUp(float dist)
    {
        this->Translate(this->rotation * this->upVec * dist);
        return *this;
    }

    ObjectInstance& ObjectInstance::Rotate(float horz, float vert)
    {
        Rotate(horz, this->rotation * this->upVec);
        Rotate(vert, this->rotation * this->rightVec);
        
        return *this;
    }

    void ObjectInstance::SetForwardVector(const Vector3& forward)
    {
        this->forwardVec = forward;
    }

    void ObjectInstance::SetUpVector(const Vector3& up)
    {
        this->upVec = up;
    }

    void ObjectInstance::SetRightVector(const Vector3& right)
    {
        this->rightVec = right;
    }

    const Vector3& ObjectInstance::GetForwardVector() const
    {
        return this->forwardVec;
    }

    const Vector3& ObjectInstance::GetUpVector() const
    {
        return this->upVec;
    }

    const Vector3& ObjectInstance::GetRightVector() const
    {
        return this->rightVec;
    }

    ObjectInstance& ObjectInstance::Translate(const Vector3& dist)
    {
        return this->Translate(dist.x, dist.y, dist.z);
    }

    ObjectInstance& ObjectInstance::TranslateX(float x)
	{
		return Translate(x, 0.0f, 0.0f);
	}

	ObjectInstance& ObjectInstance::TranslateY(float y)
	{
		return Translate(0.0f, y, 0.0f);
	}

	ObjectInstance& ObjectInstance::TranslateZ(float z)
	{
		return Translate(0.0f, 0.0f, z);
	}

	size_t ObjectInstance::GetIterator() const
	{
		return 0;
	}

	bool ObjectInstance::IsLast(size_t iterator) const
	{
		return this->GetBaseObject()->GetRenderObjects().size() == iterator;
	}

	size_t ObjectInstance::GetNext(size_t iterator) const
	{
		return iterator + 1;
	}

	const IRenderable& ObjectInstance::GetCurrent(size_t iterator) const
	{
		return this->GetBaseObject()->GetRenderObjects()[iterator];
	}

	const Matrix4x4& ObjectInstance::GetModel() const
	{
		if (needUpdate)
		{
			auto Translation = MxEngine::Translate(Matrix4x4(1.0f), this->translation);
            auto Rotation = ToMatrix(this->rotation);
			auto Scale = MxEngine::Scale(Matrix4x4(1.0f), this->scale);
			this->Model = Translation * Rotation * Scale;
			needUpdate = false;
		}
		return this->Model;
	}

	bool ObjectInstance::HasShader() const
	{
		return this->Shader != nullptr;
	}

	const MxEngine::Shader& ObjectInstance::GetShader() const
	{
		return *this->Shader;
	}

	bool ObjectInstance::IsDrawable() const
	{
		return this->shouldRender && this->object != nullptr;
	}

	bool ObjectInstance::HasTexture() const
	{
		return this->Texture != nullptr;
	}

	const MxEngine::Texture& ObjectInstance::GetTexture() const
	{
		return *this->Texture;
	}

	size_t ObjectInstance::GetInstanceCount() const
	{
		return this->instanceCount;
	}

	void RenderObject::GenerateMeshIndicies() const
	{
		std::vector<IndexBuffer::IndexType> indicies;
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
        this->IBO = Graphics::Instance()->CreateIndexBuffer(indicies);
		this->meshGenerated = true;
	}

	const VertexArray& RenderObject::GetVAO() const
	{
		return *this->VAO;
	}

	const IndexBuffer& RenderObject::GetMeshIBO() const
	{
		if (!this->meshGenerated) GenerateMeshIndicies();
		return *this->IBO;
	}

	const Material& RenderObject::GetMaterial() const
	{
		return *this->material;
	}

	size_t RenderObject::GetVertexCount() const
	{
		return this->vertexCount;
	}

	bool RenderObject::HasMaterial() const
	{
		return this->material != nullptr;
	}
}