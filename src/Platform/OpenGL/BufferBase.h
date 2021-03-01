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

#include <cstdint>
#include <cstddef>

namespace MxEngine
{
	enum class UsageType : uint8_t
	{
		STREAM_DRAW,
		STREAM_READ,
		STREAM_COPY,
		STATIC_DRAW,
		STATIC_READ,
		STATIC_COPY,
		DYNAMIC_DRAW,
		DYNAMIC_READ,
		DYNAMIC_COPY,
	};

	enum class BufferType : uint8_t
	{
		UNKNOWN = 0,
		ARRAY,
		ELEMENT_ARRAY,
		SHADER_STORAGE,
	};

	class BufferBase
	{
		using BindableId = unsigned int;

		BindableId id = 0;
		BufferType type = BufferType::UNKNOWN;
		UsageType usage = UsageType::STATIC_DRAW;
		size_t byteSize = 0;

		void FreeBuffer();
	public:
		BufferBase();
		~BufferBase();
		BufferBase(const BufferBase&) = delete;
		BufferBase(BufferBase&&) noexcept;
		BufferBase& operator=(const BufferBase&) = delete;
		BufferBase& operator=(BufferBase&&) noexcept;

		void Bind() const;
		void Unbind() const;
		void BindBase(size_t index) const;
		BindableId GetNativeHandle() const;
		BufferType GetBufferType() const;
		UsageType GetUsageType() const;
		size_t GetByteSize() const;
		void SetUsageType(UsageType usage);

	protected:
		void Load(BufferType type, const uint8_t* byteData, size_t byteSize, UsageType usage);
		void BufferSubData(const uint8_t* byteData, size_t byteSize, size_t offset = 0);
		void BufferDataWithResize(const uint8_t* byteData, size_t byteSize);
		void GetBufferData(uint8_t* byteData, size_t byteSize, size_t offset = 0) const;
	};
}