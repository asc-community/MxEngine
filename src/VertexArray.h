#pragma once

#include "GLUtils.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace MomoEngine
{
	class VertexArray : IBindable
	{
		int attributeIndex = 0;
	public:
		VertexArray();
		~VertexArray();
		VertexArray(const VertexArray&) = delete;
		VertexArray(VertexArray&&) = delete;

		// Inherited via IBindable
		void Bind() const override;
		void Unbind() const override;

		void AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout);
		void AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout);
	};
}