#include "VertexBufferLayout.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"

namespace MomoEngine
{
	const std::vector<VertexBufferLayout::VertexBufferElement>& VertexBufferLayout::GetElements() const
	{
		return elements;
	}

	unsigned int VertexBufferLayout::GetStride() const
	{
		return stride;
	}

	void VertexBufferLayout::PushFloat(unsigned int count)
	{
		#ifdef _DEBUG
		this->layoutString += TypeToString<float>() + std::to_string(count) + ", ";
		#endif
		this->elements.push_back({ count, GetGLType<float>(), false });
		this->stride += sizeof(float) * count;
	}
}