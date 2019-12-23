#pragma once
#include <string>
#include <vector>
#include "Core/OpenGL/Texture/Texture.h"
#include "Core/OpenGL/VertexBuffer/VertexBuffer.h"
#include "Core/OpenGL/VertexArray/VertexArray.h"
#include "Utilities/Memory/Memory.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace MomoEngine
{
	class GLObject
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;

		bool useTexture, useNormal;
		size_t vertexCount;
		#ifdef _DEBUG
		std::vector<GLfloat> buffer;
		#endif
		UniqueRef<VertexBuffer> VBO;
		VertexArray VAO;

		VertexArray LoadFromFile(const std::string& filepath);
		VertexArray LoadFromBuffer(const std::vector<GLfloat>& buffer, bool useTexture, bool useNormal);
	public:
		explicit GLObject();
		explicit GLObject(const std::string& filepath);
		explicit GLObject(const std::vector<GLfloat>& bufferSource, size_t vertexCount, bool useTexture = false, bool useNormal = false);
		GLObject(GLObject&) = delete;
		GLObject(GLObject&&) = default;

		void Load(const std::string& filepath);
		void Load(const std::vector<GLfloat>& buffer, size_t vertexCount, bool useTexture = false, bool useNormal = false);

		VertexArray& GetVertexData();
		std::vector<GLuint> GeneratePolygonIndicies() const;
		size_t GetVertexCount() const;
		bool HasTextureData() const;
		bool HasNormalData() const;
	};

	class GLInstance
	{
	public:
		glm::mat4x4 Model;
		Ref<Texture> Texture;
	protected:
		Ref<GLObject> object;
	public:
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
	};
}