#pragma once
#include <string>
#include <vector>
#include "Core/OpenGL/Texture/Texture.h"
#include "Core/OpenGL/VertexBuffer/VertexBuffer.h"
#include "Core/OpenGL/VertexArray/VertexArray.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Interfaces/IDrawable.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <functional>

namespace MomoEngine
{
	class GLRenderObject : public IRenderable
	{
		bool useTexture = false, useNormal = false;
		mutable bool meshGenerated = false;
		mutable IndexBuffer IBO;
		size_t vertexCount = 0;
		std::vector<VertexBuffer> VBOs;
		VertexArray VAO;
		Ref<Texture> Texture;

		friend class GLObject;
		void GenerateMeshIndicies() const;
	public:
		GLRenderObject() = default;
		GLRenderObject(const GLRenderObject&) = delete;
		GLRenderObject(GLRenderObject&&) noexcept = default;
		virtual const VertexArray& GetVAO() const override;
		virtual const IndexBuffer& GetIBO() const override;
		virtual const MomoEngine::Texture& GetTexture() const override;
		virtual size_t GetVertexCount() const override;
		virtual bool HasTexture() const override;
	};

	class GLObject
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;
		std::vector<GLRenderObject> subObjects;

		void LoadFromFile(const std::string& filepath);
	public:
		// calls with object index as first argument and field [count specified] as second argument
		using GeneratorFunction = std::function<float(int, int)>;

		explicit GLObject() = default;
		explicit GLObject(const std::string& filepath);
		GLObject(GLObject&) = delete;
		GLObject(GLObject&&) = default;

		void Load(const std::string& filepath);
		std::vector<GLRenderObject>& GetRenderObjects();

		void AddInstanceBuffer(const std::vector<float>& buffer, size_t count, UsageType type = UsageType::STATIC_DRAW);
		void AddInstanceBuffer(const GeneratorFunction& generator, size_t componentCount, size_t count, UsageType type = UsageType::STATIC_DRAW);
	};

	class GLInstance : public IDrawable
	{
		mutable glm::mat4x4 Model;
		glm::vec3 translation{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale{ 1.0f };
		mutable bool needUpdate = true;
	protected:
		Ref<GLObject> object;
	public:
		Ref<Shader> Shader;
		GLInstance();
		GLInstance(const Ref<GLObject>& object);

		void Load(const Ref<GLObject>& object);
		Ref<GLObject>& GetGLObject();
		const Ref<GLObject>& GetGLObject() const;

		GLInstance& Scale(float scale);
		GLInstance& Scale(float scaleX, float scaleY, float scaleZ);
		GLInstance& Scale(const glm::vec3& scale);

		GLInstance& Rotate(float angle, const glm::vec3& rotate);
		GLInstance& RotateX(float angle);
		GLInstance& RotateY(float angle);
		GLInstance& RotateZ(float angle);

		GLInstance& Translate(float x, float y, float z);
		GLInstance& TranslateX(float x);
		GLInstance& TranslateY(float y);
		GLInstance& TranslateZ(float z);

		// Inherited via IDrawable
		virtual size_t GetIterator() const override;
		virtual bool IsLast(size_t iterator) const override;
		virtual size_t GetNext(size_t iterator) const override;
		virtual const IRenderable& GetCurrent(size_t iterator) const override;
		virtual const glm::mat4x4& GetModel() const override;
		virtual bool HasShader() const override;
		virtual const MomoEngine::Shader& GetShader() const override;
	};
}