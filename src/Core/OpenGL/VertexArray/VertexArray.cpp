#include "VertexArray.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"

namespace MomoEngine
{
	VertexArray::VertexArray()
	{
		this->id = 0;
	}

	VertexArray::~VertexArray()
	{
		if (id != 0)
		{
			GLCALL(glDeleteVertexArrays(1, &id));
		}
	}

	VertexArray::VertexArray(VertexArray&& array) noexcept
		: attributeIndex(array.attributeIndex)
	{
		this->id = array.id;
		array.id = 0;
	}

	VertexArray& VertexArray::operator=(VertexArray&& array) noexcept
	{
		this->attributeIndex = array.attributeIndex;
		this->id = array.id;
		array.id = 0;
		return *this;
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(id);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
	{
		if (id == 0)
		{
			GLCALL(glGenVertexArrays(1, &id));
		}
		Bind();
		buffer.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (const auto& element : elements)
		{
			GLCALL(glEnableVertexAttribArray(this->attributeIndex));
			GLCALL(glVertexAttribPointer(this->attributeIndex, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset));
			offset += element.count * GetGLTypeSize(element.type);
			this->attributeIndex++;
		}
	}

	void VertexArray::AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
	{
		if (id == 0)
		{
			GLCALL(glGenVertexArrays(1, &id));
		}
		Bind();
		buffer.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (const auto& element : elements)
		{
			GLCALL(glEnableVertexAttribArray(this->attributeIndex));
			GLCALL(glVertexAttribPointer(this->attributeIndex, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset));
			GLCALL(glVertexAttribDivisor(this->attributeIndex, 1));
			offset += element.count * GetGLTypeSize(element.type);
			this->attributeIndex++;
		}
	}
}