#pragma once
#include <string>
#include <vector>
#include "Core/Interfaces/GraphicAPI/Texture.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Interfaces/IDrawable.h"
#include "Core/Interfaces/IMovable.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <functional>
#include "Core/Material/Material.h"

namespace MxEngine
{
	class RenderObject : public IRenderable
	{
		bool useTexture = false, useNormal = false;
		mutable bool meshGenerated = false;
		mutable UniqueRef<IndexBuffer> IBO;
		size_t vertexCount = 0;
		std::vector<UniqueRef<VertexBuffer>> VBOs;
		UniqueRef<VertexArray> VAO;
		UniqueRef<Material> material;

		friend class BaseObject;
		friend class ObjectInstance;
		void GenerateMeshIndicies() const;
	public:
		RenderObject() = default;
		RenderObject(const RenderObject&) = delete;
		RenderObject(RenderObject&&) noexcept = default;
		virtual const VertexArray& GetVAO() const override;
		virtual const IndexBuffer& GetMeshIBO() const override;
		virtual const Material& GetMaterial() const override;
		virtual size_t GetVertexCount() const override;
		virtual bool HasMaterial() const override;
	};

	class BaseObject
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;

        Vector3 objectCenter;
		std::vector<RenderObject> subObjects;

		void LoadFromFile(const std::string& filepath);
	public:
		explicit BaseObject() = default;
		explicit BaseObject(const std::string& filepath);
		BaseObject(BaseObject&) = delete;
		BaseObject(BaseObject&&) = default;

		void Load(const std::string& filepath);
		std::vector<RenderObject>& GetRenderObjects();
        const Vector3& GetObjectCenter() const;
	};

	class ObjectInstance : public IDrawable, public IMovable
	{
		mutable Matrix4x4 Model;
		Vector3 translation{ 0.0f };
        Vector3 forwardVec{ 1.0f, 0.0f, 0.0f }, upVec{ 0.0f, 1.0f, 0.0f }, rightVec{ 0.0f, 0.0f, 1.0f };
        Quaternion rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		Vector3 scale{ 1.0f };
		size_t instanceCount = 0;
		mutable bool needUpdate = true;
		bool shouldRender = true;
	protected:
		Ref<BaseObject> object;
	public:
		// calls with object index as first argument and field [count specified] as second argument
		using GeneratorFunction = std::function<float(int, int)>;

		Ref<Shader> Shader;
		Ref<Texture> Texture;
		ObjectInstance() = default;
		ObjectInstance(const Ref<BaseObject>& object);
		ObjectInstance(const ObjectInstance&) = delete;
		ObjectInstance(ObjectInstance&&) = default;

		void Load(const Ref<BaseObject>& object);
		Ref<BaseObject>& GetBaseObject();
		const Ref<BaseObject>& GetBaseObject() const;
		void Hide();
		void Show();

		const Vector3& GetTranslation() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;

		ObjectInstance& Scale(float scale);
		ObjectInstance& Scale(float scaleX, float scaleY, float scaleZ);
		ObjectInstance& Scale(const Vector3& scale);

		ObjectInstance& Rotate(float angle, const Vector3& rotate);
		ObjectInstance& RotateX(float angle);
		ObjectInstance& RotateY(float angle);
		ObjectInstance& RotateZ(float angle);

        ObjectInstance& Translate(const Vector3& dist);
		ObjectInstance& TranslateX(float x);
		ObjectInstance& TranslateY(float y);
		ObjectInstance& TranslateZ(float z);

		void AddInstanceBuffer(const std::vector<float>& buffer, size_t count, UsageType type = UsageType::STATIC_DRAW);
		void AddInstanceBuffer(const GeneratorFunction& generator, size_t componentCount, size_t count, UsageType type = UsageType::STATIC_DRAW);

		// Inherited via IDrawable
		virtual size_t GetIterator() const override;
		virtual bool IsLast(size_t iterator) const override;
		virtual size_t GetNext(size_t iterator) const override;
		virtual const IRenderable& GetCurrent(size_t iterator) const override;
		virtual const Matrix4x4& GetModel() const override;
		virtual bool HasShader() const override;
		virtual const MxEngine::Shader& GetShader() const override;
		virtual bool IsDrawable() const override;
		virtual bool HasTexture() const override;
		virtual const MxEngine::Texture& GetTexture() const override;
		virtual size_t GetInstanceCount() const override;

        // Inherited via IMovable
        virtual ObjectInstance& Translate(float x, float y, float z) override;
        virtual ObjectInstance& TranslateForward(float dist) override;
        virtual ObjectInstance& TranslateRight(float dist) override;
        virtual ObjectInstance& TranslateUp(float dist) override;
        virtual ObjectInstance& Rotate(float horz, float vert) override;
        virtual void SetForwardVector(const Vector3& forward) override;
        virtual void SetUpVector(const Vector3& up) override;
        virtual void SetRightVector(const Vector3& right) override;
        virtual const Vector3& GetForwardVector() const override;
        virtual const Vector3& GetUpVector() const override;
        virtual const Vector3& GetRightVector() const override;
    };
}