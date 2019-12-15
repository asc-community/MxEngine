#pragma once

#include "GLUtils.h"
#include "IBindable.h"

class VertexBuffer : IBindable
{
public:
	VertexBuffer(void* data, size_t sizeBytes);
	~VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&&) = delete;

	void Bind() const override;
	void Unbind() const override;
};