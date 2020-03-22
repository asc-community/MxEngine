// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "GLVertexBuffer.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

#include <array>

namespace MxEngine
{		
	std::array<GLenum, 9> DataType =
	{
		GL_STREAM_DRAW,
		GL_STREAM_READ,
		GL_STREAM_COPY,
		GL_STATIC_DRAW,
		GL_STATIC_READ,
		GL_STATIC_COPY,
		GL_DYNAMIC_DRAW,
		GL_DYNAMIC_READ,
		GL_DYNAMIC_COPY,
	};

	GLVertexBuffer::GLVertexBuffer()
	{
		this->id = 0;
		this->size = 0;
	}

	GLVertexBuffer::GLVertexBuffer(BufferData data, size_t count, UsageType type)
	{
		Load(data, count, type);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{
		if (this->id != 0)
		{
			GLCALL(glDeleteBuffers(1, &id));
		}
	}

	GLVertexBuffer::GLVertexBuffer(GLVertexBuffer&& vbo) noexcept
	{
		this->id = vbo.id;
		this->size = vbo.size;
		vbo.id = 0;
		vbo.size = 0;
	}

	void GLVertexBuffer::Load(BufferData data, size_t count, UsageType type)
	{
		this->size = count;
		if (id == 0)
		{
			GLCALL(glGenBuffers(1, &id));
		}
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, DataType[(int)type]));
	}

    void GLVertexBuffer::BufferSubData(BufferData data, size_t count, size_t offset)
    {
		this->Bind();
		GLCALL(glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * offset, count * sizeof(float), data));
    }

    size_t GLVertexBuffer::GetSize() const
    {
		return this->size;
    }

	void GLVertexBuffer::Bind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
	}

	void GLVertexBuffer::Unbind() const
	{
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}