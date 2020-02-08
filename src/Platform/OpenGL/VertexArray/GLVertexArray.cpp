#include "GLVertexArray.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

namespace MxEngine
{
	GLVertexArray::GLVertexArray()
	{
		this->id = 0;
	}

	GLVertexArray::~GLVertexArray()
	{
		if (id != 0)
		{
			GLCALL(glDeleteVertexArrays(1, &id));
		}
	}

	GLVertexArray::GLVertexArray(GLVertexArray&& array) noexcept
		: attributeIndex(array.attributeIndex)
	{
		this->id = array.id;
		array.id = 0;
	}

    GLVertexArray& GLVertexArray::operator=(GLVertexArray&& array) noexcept
    {
        this->attributeIndex = array.attributeIndex;
        this->id = array.id;
        array.id = 0;
        return *this;
    }

	void GLVertexArray::Bind() const
	{
		glBindVertexArray(id);
	}

	void GLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void GLVertexArray::AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
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

	void GLVertexArray::AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
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