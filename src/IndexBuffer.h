#pragma once

#include "GLUtils.h"
#include "IBindable.h"

class IndexBuffer : IBindable
{
	size_t count;
#ifdef _DEBUG
	unsigned int* indicies = nullptr;
#endif
public:
	IndexBuffer(unsigned int* data, size_t count);
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer(IndexBuffer&&) = delete;
	~IndexBuffer();
	unsigned int GetCount() const;
	unsigned int GetIndexType() const;

	// Унаследовано через Bindable
	virtual void Bind() const override;
	virtual void Unbind() const override;
};