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

#pragma once
#include <cstddef>
#include <cstdint>

#include "UsageType.h"

namespace MxEngine
{
	class ShaderStorageBuffer
	{
		using BindableId = unsigned int;
		using BufferDataRead = const uint8_t*;
		using BufferDataWrite = uint8_t*;

		BindableId id = 0;
		size_t size;
		void FreeShaderStorageBuffer();
	public:
		explicit ShaderStorageBuffer();
		explicit ShaderStorageBuffer(BufferDataRead data, size_t byteSize, UsageType type);
		~ShaderStorageBuffer();
		ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
		ShaderStorageBuffer(ShaderStorageBuffer&& ssbo) noexcept;
		ShaderStorageBuffer& operator=(const ShaderStorageBuffer&) = delete;
		ShaderStorageBuffer& operator=(ShaderStorageBuffer&& ssbo) noexcept;

		BindableId GetNativeHandle() const;
		void Bind() const;
		void Unbind() const;
		void BindTarget(size_t index) const;
		void Load(BufferDataRead data, size_t byteSize, UsageType type);
		void BufferSubData(BufferDataRead data, size_t byteSize, size_t byteOffset = 0);
		void BufferDataWithResize(BufferDataRead data, size_t byteSize);
		void GetBufferedData(BufferDataWrite data, size_t byteSize, size_t byteOffset = 0) const;
		size_t GetByteSize() const;

		template<typename T>
		ShaderStorageBuffer(const T* data, size_t elementCount, size_t elementOffset = 0)
			: ShaderStorageBuffer((BufferDataRead)data, elementCount * sizeof(T), elementOffset * sizeof(T))
		{

		}

		template<typename T>
		void BufferSubData(const T* data, size_t elementCount, size_t elementOffset = 0)
		{
			this->BufferSubData((BufferDataRead)data, elementCount * sizeof(T), elementOffset * sizeof(T));
		}

		template<typename T>
		void BufferDataWithResize(const T* data, size_t elementCount)
		{
			this->BufferDataWithResize((BufferDataRead)data, elementCount * sizeof(T));
		}

		template<typename T>
		size_t GetElementCount() const
		{
			return this->GetByteSize() * sizeof(T);
		}

		template<typename T>
		void GetBufferedData(T* data, size_t elementCount, size_t elementOffset)
		{
			return this->GetBufferedData((BufferDataWrite)data, elementCount * sizeof(T), elementOffset * sizeof(T));
		}
	};
}