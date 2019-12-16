#pragma once

#include "GLUtils.h"
#include "IBindable.h"
#include <vector>

namespace MomoEngine
{
	class VertexBuffer : IBindable
	{
	public:
		template<typename T>
		VertexBuffer(const std::vector<T>& data);
		~VertexBuffer();
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;

		// Inherited via IBindable
		void Bind() const override;
		void Unbind() const override;
	};

	template<typename T>
	inline VertexBuffer::VertexBuffer(const std::vector<T>& data)
	{
		GLCALL(glGenBuffers(1, &id));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW));
	}
}