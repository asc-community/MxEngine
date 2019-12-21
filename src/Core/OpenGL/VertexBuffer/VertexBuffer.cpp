#include "VertexBuffer.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"

namespace MomoEngine
{
	VertexBuffer::VertexBuffer(const std::vector<float>& data)
	{
		GLCALL(glGenBuffers(1, &id));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW));
	}

	VertexBuffer::~VertexBuffer()
	{
		GLCALL(glDeleteBuffers(1, &id));
	}

	void VertexBuffer::Bind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
	}

	void VertexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}