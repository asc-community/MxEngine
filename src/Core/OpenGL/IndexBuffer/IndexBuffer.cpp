#include "IndexBuffer.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"

namespace MomoEngine
{
	IndexBuffer::IndexBuffer()
	{
		this->id = 0;
	}

	IndexBuffer::IndexBuffer(const std::vector<IndexType>& data)
	{
		Load(data);
	}

	IndexBuffer::~IndexBuffer()
	{
		if (this->id != 0)
		{
			GLCALL(glDeleteBuffers(1, &id));
		}
	}

	void IndexBuffer::Load(const std::vector<IndexType>& data)
	{
		#ifdef _DEBUG
		indicies = data.data();
		#endif
		this->count = data.size() * sizeof(IndexType);
		GLCALL(glGenBuffers(1, &id));
		Bind();
		GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(IndexType), data.data(), GL_STATIC_DRAW));
	}

	void IndexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	size_t IndexBuffer::GetCount() const
	{
		return count;
	}

	size_t IndexBuffer::GetIndexType() const
	{
		return GL_UNSIGNED_INT;
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
}