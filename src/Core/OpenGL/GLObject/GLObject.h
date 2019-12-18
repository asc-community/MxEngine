#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "../Texture/Texture.h"
#include "../VertexBuffer/VertexBuffer.h"

namespace MomoEngine
{
	class GLObject
	{
		typedef unsigned int GLuint;
		typedef float GLfloat;

		size_t vertexCount;
		#ifdef _DEBUG
		std::vector<GLfloat> buffer;
		#endif
		VertexBuffer VBO;

		std::vector<GLfloat> LoadFromFile(const std::string& filepath);
	public:
		GLObject(const std::string& filepath);
		GLObject(const std::vector<GLfloat>& bufferSource, size_t vertexCount);
		GLObject(GLObject&) = delete;
		GLObject(GLObject&&) = default;

		VertexBuffer& GetBuffer();
		std::vector<GLuint> GeneratePolygonIndicies() const;
		size_t GetVertexCount() const;
	};
}