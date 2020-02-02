#pragma once
#include <string>
#include <vector>
#include "Core/Interfaces/GraphicAPI/Texture.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Interfaces/IDrawable.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <functional>
#include "Core/Material/Material.h"

namespace MomoEngine
{
	class GLRenderObject : public IRenderable
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
		GLRenderObject() = default;
		GLRenderObject(const GLRenderObject&) = delete;
		GLRenderObject(GLRenderObject&&) noexcept = default;
		virtual const VertexArray& GetVAO() const override;
		virtual const IndexBuffer& GetIBO() const override;
		virtual const Material& GetMaterial() const override;
		virtual size_t GetVertexCount() const override;
		virtual bool HasMaterial() const override;
	};

	class BaseObject
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;
		std::vector<GLRenderObject> subObjects;

		void LoadFromFile(const std::string& filepath);
	public:
		explicit BaseObject() = default;
		explicit BaseObject(const std::string& filepath);
		BaseObject(BaseObject&) = delete;
		BaseObject(BaseObject&&) = default;

		void Load(const std::string& filepath);
		std::vector<GLRenderObject>& GetRenderObjects();
	};

	class ObjectInstance : public IDrawable
	{
		mutable Matrix4x4 Model;
		Vector3 translation{ 0.0f };
		Vector3 rotation{ 0.0f };
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

		Vector3 GetTranslation() const;
		Vector3 GetRotation() const;
		Vector3 GetScale() const;

		ObjectInstance& Scale(float scale);
		ObjectInstance& Scale(float scaleX, float scaleY, float scaleZ);
		ObjectInstance& Scale(const Vector3& scale);

		ObjectInstance& Rotate(float angle, const Vector3& rotate);
		ObjectInstance& RotateX(float angle);
		ObjectInstance& RotateY(float angle);
		ObjectInstance& RotateZ(float angle);

		ObjectInstance& Translate(float x, float y, float z);
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
		virtual const MomoEngine::Shader& GetShader() const override;
		virtual bool IsDrawable() const override;
		virtual bool HasTexture() const override;
		virtual const MomoEngine::Texture& GetTexture() const override;
		virtual size_t GetInstanceCount() const override;
	};
}