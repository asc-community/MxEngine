#pragma once

#include "GLUtils.h"
#include <vector>

class VertexBufferLayout
{
	struct VertexBufferElement
	{
		unsigned int count;
		unsigned int type;
		unsigned char normalized;
	};

	std::vector<VertexBufferElement> elements;
	unsigned int stride = 0;
public:
	VertexBufferLayout() = default;

	const std::vector<VertexBufferElement>& GetElements() const;

	unsigned int GetStride() const;

	template<typename T>
	void Push(unsigned int count)
	{
		elements.push_back({ count, GetGLType<T>(), GL_FALSE });
		stride += sizeof(T) * count;
	}
};