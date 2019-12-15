#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(unsigned int* data, size_t count)
{
#ifdef _DEBUG
	indicies = data;
#endif
	this->count = count;
	GLCALL(glGenBuffers(1, &id));
	Bind();
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GLCALL(glDeleteBuffers(1, &id));
}

void IndexBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned int IndexBuffer::GetCount() const
{
	return count;
}

unsigned int IndexBuffer::GetIndexType() const
{
	return GL_UNSIGNED_INT;
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}
