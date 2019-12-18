#include "VertexBuffer.h"

namespace MomoEngine
{
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