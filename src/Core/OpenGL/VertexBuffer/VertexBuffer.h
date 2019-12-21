#pragma once

#include "Core/Interfaces/IBindable.h"
#include <vector>

namespace MomoEngine
{
	class VertexBuffer : IBindable
	{
	public:
		explicit VertexBuffer(const std::vector<float>& data);
		~VertexBuffer();
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;

		// Inherited via IBindable
		void Bind() const override;
		void Unbind() const override;
	};
}