#include "GLVertexBuffer.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

namespace MxEngine
{
	GLVertexBuffer::GLVertexBuffer()
	{
		this->id = 0;
	}

	GLVertexBuffer::GLVertexBuffer(BufferData& data, UsageType type)
	{
		Load(data, type);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{
		if (this->id != 0)
		{
			GLCALL(glDeleteBuffers(1, &id));
		}
	}

	GLVertexBuffer::GLVertexBuffer(GLVertexBuffer&& vbo)
	{
		this->id = vbo.id;
		vbo.id = 0;
	}

	void GLVertexBuffer::Load(const BufferData& data, UsageType type)
	{
		GLCALL(glGenBuffers(1, &id));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), (GLenum)type));
	}

	void GLVertexBuffer::Bind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
	}

	void GLVertexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}