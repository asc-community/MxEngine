#pragma once

#include "../Core/OpenGL/GLUtils/GLUtils.h"
#include "../Core/Interfaces/IBindable.h"
#include "../Core/OpenGL/VertexBufferLayout/VertexBufferLayout.h"
#include "../Core/OpenGL/VertexBuffer/VertexBuffer.h"

namespace MomoEngine
{
	class VertexArray : IBindable
	{
		int attributeIndex = 0;
	public:
		VertexArray();
		~VertexArray();
		VertexArray(const VertexArray&) = delete;
		VertexArray(VertexArray&& array) noexcept;

		// Inherited via IBindable
		void Bind() const override;
		void Unbind() const override;

		void AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout);
		void AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout);
	};
}