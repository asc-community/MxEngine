#pragma once

#include "GLUtils.h"
#include "IBindable.h"
#include <vector>

namespace MomoEngine
{
	class IndexBuffer : IBindable
	{
	public:
		using IndexType = GLuint;
	private:
		size_t count;
		#ifdef _DEBUG
		const IndexType* indicies = nullptr;
		#endif
	public:
		IndexBuffer(const std::vector<IndexType>& data);
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&&) = delete;
		~IndexBuffer();
		size_t GetCount() const;
		size_t GetIndexType() const;

		// Inherited via IBindables
		virtual void Bind() const override;
		virtual void Unbind() const override;
	};
}