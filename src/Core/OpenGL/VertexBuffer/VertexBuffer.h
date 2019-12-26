#pragma once

#include "Core/Interfaces/IBindable.h"
#include <vector>

namespace MomoEngine
{
	enum class UsageType
	{
		STREAM_DRAW  = 0x88E0,
		STREAM_READ  = 0x88E1,
		STREAM_COPY  = 0x88E2,
		STATIC_DRAW  = 0x88E4,
		STATIC_READ  = 0x88E5,
		STATIC_COPY  = 0x88E6,
		DYNAMIC_DRAW = 0x88E8,
		DYNAMIC_READ = 0x88E9,
		DYNAMIC_COPY = 0x88EA,
	};

	class VertexBuffer : IBindable
	{
	public:
		explicit VertexBuffer();
		explicit VertexBuffer(const std::vector<float>& data, UsageType type);
		~VertexBuffer();
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& vbo);

		void Load(const std::vector<float>& data, UsageType type);

		// Inherited via IBindable
		void Bind() const override;
		void Unbind() const override;
	};
}