#include "GLIndexBuffer.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

namespace MxEngine
{
	GLIndexBuffer::GLIndexBuffer()
	{
		this->id = 0;
	}

	GLIndexBuffer::GLIndexBuffer(const IndexBufferType& data)
	{
		Load(data);
	}

	GLIndexBuffer::GLIndexBuffer(GLIndexBuffer&& ibo) noexcept
		: count(ibo.count)
	{
		this->id = ibo.id;
		ibo.id = 0;
	}

	GLIndexBuffer::~GLIndexBuffer()
	{
		if (this->id != 0)
		{
			GLCALL(glDeleteBuffers(1, &id));
		}
	}

	void GLIndexBuffer::Load(const IndexBufferType& data)
	{
		#ifdef _DEBUG
		indicies = data.data();
		#endif
		this->count = data.size() * sizeof(IndexType);
		GLCALL(glGenBuffers(1, &id));
		Bind();
		GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(IndexType), data.data(), GL_STATIC_DRAW));
	}

	void GLIndexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	size_t GLIndexBuffer::GetCount() const
	{
		return count;
	}

	size_t GLIndexBuffer::GetIndexTypeId() const
	{
		return GL_UNSIGNED_INT;
	}

	void GLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
}