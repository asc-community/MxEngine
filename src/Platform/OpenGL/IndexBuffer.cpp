// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "IndexBuffer.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Logger/Logger.h"
#include "Core/Macro/Macro.h"

namespace MxEngine
{
	IndexBuffer::IndexBuffer()
	{
		GLCALL(glGenBuffers(1, &id));
		Logger::Instance().Debug("OpenGL::IndexBuffer", "created index buffer with id = " + ToMxString(id));
	}

	IndexBuffer::IndexBuffer(const IndexType* data, size_t count)
		: IndexBuffer()
	{
		Load(data, count);
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& ibo) noexcept
		: count(ibo.count)
	{
		this->id = ibo.id;
		ibo.id = 0;
		ibo.count = 0;
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ibo) noexcept
	{
		this->count = ibo.count;
		this->id = ibo.id;
		ibo.count = 0;
		ibo.id = 0;
		return *this;
	}

	IndexBuffer::~IndexBuffer()
	{
		if (this->id != 0)
		{
			GLCALL(glDeleteBuffers(1, &id));
		}
	}

	void IndexBuffer::Load(const IndexType* data, size_t count)
	{
		this->count = count;
		this->Bind();
		GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(IndexType), data, GL_STATIC_DRAW));
	}

	void IndexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	size_t IndexBuffer::GetCount() const
	{
		return count;
	}

	size_t IndexBuffer::GetIndexTypeId() const
	{
		return GL_UNSIGNED_INT;
	}

	IndexBuffer::BindableId IndexBuffer::GetNativeHandle() const
	{
		return id;
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
}