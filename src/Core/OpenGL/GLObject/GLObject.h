#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../Texture/Texture.h"
#include "../VertexArray/VertexArray.h"

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
		GLObject(const std::string& filepath);
		GLObject(const std::vector<GLfloat>& bufferSource, size_t vertexCount, bool useTexture = false, bool useNormal = false);
		GLObject(GLObject&) = delete;
		GLObject(GLObject&&) = default;

		VertexArray& GetVertexData();
		std::vector<GLuint> GeneratePolygonIndicies() const;
		size_t GetVertexCount() const;
		bool HasTextureData() const;
		bool HasNormalData() const;
	};
}