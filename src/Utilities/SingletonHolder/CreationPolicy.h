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

#include <memory>

// Andrei's Alexandrescu SingletonHolder (see "Modern C++ Design" ch. 6)

namespace MxEngine
{
	/*!
	policy class which allocates singleton object using new operator
	*/
	template<typename T>
	class CreateWithNew
	{
	public:
		static inline T* Create()
		{
			return new T();
		}

		static inline void Destroy(T* obj)
		{
			delete obj;
		}
	};

	/*!
	policy class which allocates singleton object using malloc function
	*/
	template<typename T>
	class CreateWithMalloc
	{
	public:
		static inline T* Create()
		{
			T* ptr = std::malloc(sizeof(T));
			if (ptr == nullptr) 
				return nullptr;
			return new(ptr) T();
		}

		static inline void Destroy(T* obj)
		{
			obj->~T();
			std::free(obj);
		}
	};

	/*!
	policy class which allocates singleton object using inplace new operator
	*/
	template<typename T>
	class CreateStatic
	{
		union MaxAling { char t[sizeof(T)]; long double ld; long long ll; };
	public:
		static inline T* Create()
		{
			static MaxAling staticMemory;
			return new(&staticMemory) T();
		}

		static inline void Destroy(T* obj)
		{
			obj->~T();
		}
	};
}