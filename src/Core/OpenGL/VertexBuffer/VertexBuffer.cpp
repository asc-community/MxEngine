#include "VertexBuffer.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"

namespace MomoEngine
{
	VertexBuffer::VertexBuffer()
	{
		this->id = 0;
	}

	VertexBuffer::VertexBuffer(const std::vector<float>& data, UsageType type)
	{
		Load(data, type);
	}

	VertexBuffer::~VertexBuffer()
	{
		if (this->id != 0)
		{
			GLCALL(glDeleteBuffers(1, &id));
		}
	}

	void VertexBuffer::Load(const std::vector<float>& data, UsageType type)
	{
		GLCALL(glGenBuffers(1, &id));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), (GLenum)type));
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