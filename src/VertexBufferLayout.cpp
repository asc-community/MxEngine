#include "VertexBufferLayout.h"

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
}