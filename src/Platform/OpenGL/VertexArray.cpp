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

#include "VertexArray.h"
#include "GLUtilities.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexAttribute.h"
#include "Utilities/Logging/Logger.h"

namespace MxEngine
{
	void VertexArray::FreeVertexArray()
	{
		if (this->id != 0)
		{
			GLCALL(glDeleteVertexArrays(1, &this->id));
			MXLOG_DEBUG("OpenGL::VertexArray", "deleted vertex array with id = " + ToMxString(id));
		}
		this->id = 0;
	}

	VertexArray::VertexArray()
	{
		GLCALL(glGenVertexArrays(1, &this->id));
		MXLOG_DEBUG("OpenGL::VertexArray", "created vertex array with id = " + ToMxString(id));
	}

	VertexArray::~VertexArray()
	{
		this->FreeVertexArray();
	}

	VertexArray::VertexArray(VertexArray&& array) noexcept
	{
		this->attributeIndex = array.attributeIndex;
		this->id = array.id;
		array.id = 0;
		array.attributeIndex = 0;
	}

	VertexArray& VertexArray::operator=(VertexArray&& array) noexcept
	{
		this->FreeVertexArray();
		this->attributeIndex = array.attributeIndex;
		this->id = array.id;
		array.id = 0;
		array.attributeIndex = 0;
		return *this;
	}

	VertexArray::BindableId VertexArray::GetNativeHandle() const
    {
		return id;
    }

    void VertexArray::Bind() const
	{
		glBindVertexArray(this->id);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexLayout(const VertexBuffer& buffer, ArrayView<VertexAttribute> layout, VertexAttributeInputRate inputRate)
	{
		this->Bind();
		buffer.Bind();
		size_t offset = 0;
		size_t stride = 0;

		for (const auto& element : layout)
			stride += element.byteSize;

		for (const auto& element : layout)
		{
			for (size_t i = 0; i < element.entries; i++)
			{
				// TODO: handle integer case with glVertexAttribIPointer
				GLCALL(glEnableVertexAttribArray(this->attributeIndex));
				GLCALL(glVertexAttribPointer(this->attributeIndex, element.components, (GLenum)element.type, GL_FALSE, stride, (void*)offset));
				if (inputRate == VertexAttributeInputRate::PER_INSTANCE)
				{
					GLCALL(glVertexAttribDivisor(this->attributeIndex, 1));
				}

				offset += element.byteSize / element.entries;
				this->attributeIndex++;
			}
		}
		this->Unbind();
	}

	void VertexArray::RemoveVertexLayout(ArrayView<VertexAttribute> layout)
	{
		MX_ASSERT(this->attributeIndex > layout.size());

		this->Bind();
		for (const auto& element : layout)
		{
			for (size_t i = 0; i < element.entries; i++)
			{
				this->attributeIndex--;
				GLCALL(glDisableVertexAttribArray(this->attributeIndex));
			}
		}
		this->Unbind();
	}

	void VertexArray::LinkIndexBuffer(const IndexBuffer& buffer)
	{
		this->Bind();
		buffer.Bind();
		this->Unbind();
	}

    int VertexArray::GetAttributeCount() const
    {
		return this->attributeIndex;
    }
}