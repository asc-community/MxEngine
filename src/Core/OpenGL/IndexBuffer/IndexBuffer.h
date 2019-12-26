#pragma once

#include "Core/Interfaces/IBindable.h"
#include <vector>

namespace MomoEngine
{
	class IndexBuffer : IBindable
	{
	public:
		using IndexType = unsigned int;
	private:
		size_t count = 0;
		#ifdef _DEBUG
		const IndexType* indicies = nullptr;
		#endif
	public:
		explicit IndexBuffer();
		explicit IndexBuffer(const std::vector<IndexType>& data);
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&& ibo) noexcept;
		~IndexBuffer();

		void Load(const std::vector<IndexType>& data);

		size_t GetCount() const;
		size_t GetIndexType() const;

		// Inherited via IBindables
		virtual void Bind() const override;
		virtual void Unbind() const override;
	};
}