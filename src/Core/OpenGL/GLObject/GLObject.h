#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Core/OpenGL/Texture/Texture.h"
#include "Core/OpenGL/VertexBuffer/VertexBuffer.h"
#include "Core/OpenGL/VertexArray/VertexArray.h"

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
		std::unique_ptr<VertexBuffer> VBO;
		VertexArray VAO;

		VertexArray LoadFromFile(const std::string& filepath);
		VertexArray LoadFromBuffer(const std::vector<GLfloat>& buffer, bool useTexture, bool useNormal);
	public:
		GLObject();
		GLObject(const std::string& filepath);
		GLObject(const std::vector<GLfloat>& bufferSource, size_t vertexCount, bool useTexture = false, bool useNormal = false);
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
}