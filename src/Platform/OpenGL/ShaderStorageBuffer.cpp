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

#include "ShaderStorageBuffer.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Logging/Logger.h"

namespace MxEngine
{
    void ShaderStorageBuffer::FreeShaderStorageBuffer()
    {
        if (this->id != 0)
        {
            GLCALL(glDeleteBuffers(1, &this->id));
            MXLOG_DEBUG("OpenGL::ShaderBuffer", "deleted shader storage buffer with id = " + ToMxString(this->id));
        }
        this->id = 0;
        this->size = 0;
    }

    ShaderStorageBuffer::ShaderStorageBuffer()
    {
        this->size = 0;
        GLCALL(glGenBuffers(1, &this->id));
        MXLOG_DEBUG("OpenGL::VertexBuffer", "created shader storage buffer with id = " + ToMxString(this->id));
    }

    ShaderStorageBuffer::ShaderStorageBuffer(BufferDataRead data, size_t byteSize, UsageType type)
        : ShaderStorageBuffer()
    {
        this->Load(data, byteSize, type);
    }

    ShaderStorageBuffer::~ShaderStorageBuffer()
    {
        this->FreeShaderStorageBuffer();
    }

    ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer&& ssbo) noexcept
    {
        this->id = ssbo.id;
        this->size = ssbo.size;
        ssbo.id = 0;
        ssbo.size = 0;
    }

    ShaderStorageBuffer& ShaderStorageBuffer::operator=(ShaderStorageBuffer&& ssbo) noexcept
    {
        this->id = ssbo.id;
        this->size = ssbo.size;
        ssbo.id = 0;
        ssbo.size = 0;

        return *this;
    }


    ShaderStorageBuffer::BindableId ShaderStorageBuffer::GetNativeHandle() const
    {
        return this->id;
    }

    void ShaderStorageBuffer::Bind() const
    {
        GLCALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->id));
    }

    void ShaderStorageBuffer::BindTarget(size_t index) const
    {
        GLCALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, this->id));
    }

    void ShaderStorageBuffer::Unbind() const
    {
        GLCALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
    }

    void ShaderStorageBuffer::Load(BufferDataRead data, size_t byteSize, UsageType type)
    {
        this->size = byteSize;
        this->Bind();
        GLCALL(glBufferData(GL_SHADER_STORAGE_BUFFER, byteSize, (const void*)data, (GLenum)UsageTypeToNative(type)));
    }

    void ShaderStorageBuffer::BufferSubData(BufferDataRead data, size_t byteSize, size_t byteOffset)
    {
        this->Bind();
        GLCALL(glBufferSubData(GL_SHADER_STORAGE_BUFFER, byteOffset, byteSize, (const void*)data));
    }

    void ShaderStorageBuffer::BufferDataWithResize(BufferDataRead data, size_t byteSize)
    {
        if (this->GetByteSize() < byteSize)
            this->Load(data, byteSize, UsageType::DYNAMIC_DRAW);
        else
            this->BufferSubData(data, byteSize);
    }

    void ShaderStorageBuffer::GetBufferedData(BufferDataWrite data, size_t byteSize, size_t byteOffset) const
    {
        this->Bind();
        GLCALL(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, byteOffset, byteSize, (void*)data));
    }

    size_t ShaderStorageBuffer::GetByteSize() const
    {
        return this->size;
    }

}