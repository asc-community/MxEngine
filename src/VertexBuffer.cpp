#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(void* data, size_t sizeBytes)
{
	GLCALL(glGenBuffers(1, &id));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, GL_STATIC_DRAW));
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