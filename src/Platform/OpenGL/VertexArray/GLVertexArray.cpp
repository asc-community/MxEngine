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

#include "GLVertexArray.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

namespace MxEngine
{
	GLVertexArray::GLVertexArray()
	{
		this->id = 0;
	}

	GLVertexArray::~GLVertexArray()
	{
		if (id != 0)
		{
			GLCALL(glDeleteVertexArrays(1, &id));
		}
	}

	GLVertexArray::GLVertexArray(GLVertexArray&& array) noexcept
		: attributeIndex(array.attributeIndex)
	{
		this->id = array.id;
		array.id = 0;
	}

	GLVertexArray& GLVertexArray::operator=(GLVertexArray&& array) noexcept
	{
		this->attributeIndex = array.attributeIndex;
		this->id = array.id;
		array.id = 0;
		return *this;
	}

	void GLVertexArray::Bind() const
	{
		glBindVertexArray(id);
	}

	void GLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void GLVertexArray::AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
	{
		if (id == 0)
		{
			GLCALL(glGenVertexArrays(1, &id));
		}
		Bind();
		buffer.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (const auto& element : elements)
		{
			GLCALL(glEnableVertexAttribArray(this->attributeIndex));
			GLCALL(glVertexAttribPointer(this->attributeIndex, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset));
			offset += element.count * GetGLTypeSize(element.type);
			this->attributeIndex++;
		}
	}

	void GLVertexArray::AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
	{
		if (id == 0)
		{
			GLCALL(glGenVertexArrays(1, &id));
		}
		Bind();
		buffer.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (const auto& element : elements)
		{
			GLCALL(glEnableVertexAttribArray(this->attributeIndex));
			GLCALL(glVertexAttribPointer(this->attributeIndex, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset));
			GLCALL(glVertexAttribDivisor(this->attributeIndex, 1));
			offset += element.count * GetGLTypeSize(element.type);
			this->attributeIndex++;
		}
	}
}