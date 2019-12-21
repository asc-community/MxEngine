#pragma once

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

		void PushFloat(unsigned int count);
	};
}