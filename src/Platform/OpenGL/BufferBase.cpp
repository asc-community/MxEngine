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

#include "BufferBase.h"
#include "GLUtilities.h"
#include "Utilities/Logging/Logger.h"

namespace MxEngine
{
    GLenum BufferTypeToEnum[] = {
        GL_FALSE, // unknown
        GL_ARRAY_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,
        GL_SHADER_STORAGE_BUFFER,
    };

    GLenum UsageTypeToEnum[] = {
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

    void BufferBase::FreeBuffer()
    {
        if (this->id != 0)
        {
            GLCALL(glDeleteBuffers(1, &this->id));
            MXLOG_DEBUG("OpenGL::Buffer", "deleted buffer with id = " + ToMxString(this->id));
            this->id = 0;
        }
    }

    BufferBase::BufferBase()
    {
        GLCALL(glGenBuffers(1, &this->id));
        MXLOG_INFO("OpenGL::Buffer", "created buffer with id = " + ToMxString(this->id));
    }

    BufferBase::~BufferBase()
    {
        this->FreeBuffer();
    }

    BufferBase::BufferBase(BufferBase&& other) noexcept
    {
        this->id = other.id;
        this->byteSize = other.byteSize;
        this->type = other.type;
        this->usage = other.usage;

        other.id = 0;
        other.byteSize = 0;
        other.type = BufferType::UNKNOWN;
        other.usage = UsageType::STATIC_DRAW;
    }

    BufferBase& BufferBase::operator=(BufferBase&& other) noexcept
    {
        this->FreeBuffer();

        this->id = other.id;
        this->byteSize = other.byteSize;
        this->type = other.type;
        this->usage = other.usage;

        other.id = 0;
        other.byteSize = 0;
        other.type = BufferType::UNKNOWN;
        other.usage = UsageType::STATIC_DRAW;

        return *this;
    }

    void BufferBase::Bind() const
    {
        GLCALL(glBindBuffer(BufferTypeToEnum[(size_t)this->type], this->id));
    }

    void BufferBase::Unbind() const
    {
        GLCALL(glBindBuffer(BufferTypeToEnum[(size_t)this->type], 0));
    }

    void BufferBase::BindBase(size_t index) const
    {
        GLCALL(glBindBufferBase(BufferTypeToEnum[(size_t)this->type], index, this->id));
    }

    BufferBase::BindableId BufferBase::GetNativeHandle() const
    {
        return this->id;
    }

    BufferType BufferBase::GetBufferType() const
    {
        return this->type;
    }

    UsageType BufferBase::GetUsageType() const
    {
        return this->usage;
    }

    size_t BufferBase::GetByteSize() const
    {
        return this->byteSize;
    }

    void BufferBase::SetUsageType(UsageType usage)
    {
        this->usage = usage;
    }

    void BufferBase::Load(BufferType type, const uint8_t* byteData, size_t byteSize, UsageType usage)
    {
        this->type = type;
        this->byteSize = byteSize;
        this->usage = usage;
        this->Bind();
        GLCALL(glBufferData(BufferTypeToEnum[(size_t)this->type], this->byteSize, byteData, UsageTypeToEnum[(size_t)this->usage]));
    }

    void BufferBase::BufferSubData(const uint8_t* byteData, size_t byteSize, size_t offset)
    {
        MX_ASSERT(byteSize + offset <= this->byteSize);
        this->Bind();
        GLCALL(glBufferSubData(BufferTypeToEnum[(size_t)this->type], offset, byteSize, byteData));
    }

    void BufferBase::BufferDataWithResize(const uint8_t* byteData, size_t byteSize)
    {
        if (this->byteSize < byteSize)
            this->Load(this->type, byteData, byteSize, this->usage);
        else
            this->BufferSubData(byteData, byteSize);
    }

    void BufferBase::GetBufferData(uint8_t* byteData, size_t byteSize, size_t offset) const
    {
        MX_ASSERT(byteSize + offset <= this->byteSize);
        this->Bind();
        GLCALL(glGetBufferSubData(BufferTypeToEnum[(size_t)this->type], offset, byteSize, byteData));
    }
}