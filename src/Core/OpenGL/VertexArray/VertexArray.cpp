#include "VertexArray.h"

namespace MomoEngine
{
	VertexArray::VertexArray()
	{
		GLCALL(glGenVertexArrays(1, &id));
	}

	VertexArray::~VertexArray()
	{
		GLCALL(glDeleteVertexArrays(1, &id));
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