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

	void VertexBufferLayout::PushFloat(size_t count)
	{
		#ifdef _DEBUG
		this->layoutString += TypeToString<float>() + std::to_string(count) + ", ";
		#endif
		this->elements.push_back({ (unsigned int)count, GetGLType<float>(), false });
		this->stride += unsigned int(sizeof(float) * count);
	}
}