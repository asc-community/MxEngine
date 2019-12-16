#pragma once

#include "GLUtils.h"
#include <vector>
#include <string>

namespace MomoEngine
{
	class VertexBufferLayout
	{
		struct VertexBufferElement
		{
			unsigned int count;
			unsigned int type;
			unsigned char normalized;
		};
		#ifdef _DEBUG
		std::string layoutString;
		#endif

		std::vector<VertexBufferElement> elements;
		unsigned int stride = 0;
	public:
		VertexBufferLayout() = default;

		const std::vector<VertexBufferElement>& GetElements() const;

		unsigned int GetStride() const;

		template<typename T>
		void Push(unsigned int count)
		{
			#ifdef _DEBUG
			this->layoutString += TypeToString<T>() + std::to_string(count) + ", ";
			#endif
			this->elements.push_back({ count, GetGLType<T>(), GL_FALSE });
			this->stride += sizeof(T) * count;
		}
	};
}