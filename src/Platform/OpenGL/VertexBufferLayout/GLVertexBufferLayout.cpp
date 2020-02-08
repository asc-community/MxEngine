#include "GLVertexBufferLayout.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

namespace MxEngine
{
	const std::vector<GLVertexBufferLayout::VertexBufferElement>& GLVertexBufferLayout::GetElements() const
	{
		return elements;
	}

	GLVertexBufferLayout::StrideType GLVertexBufferLayout::GetStride() const
	{
		return stride;
	}

	void GLVertexBufferLayout::PushFloat(size_t count)
	{
		#ifdef _DEBUG
		this->layoutString += TypeToString<float>() + std::to_string(count) + ", ";
		#endif
		this->elements.push_back({ (unsigned int)count, GetGLType<float>(), false });
		this->stride += unsigned int(sizeof(float) * count);
	}
}