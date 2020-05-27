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

#include "Core/Macro/Macro.h"

namespace MxEngine
{
    constexpr size_t KB = 1024;
    constexpr size_t MB = 1024 * KB;
    constexpr size_t GB = 1024 * MB;

    template<typename T>
    using UniqueRef = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;

    /*!
    wrapper around unique_ptr
    \params args arguments for construction of T object
    \returns unique_ptr<T>
    */
    template<typename T, typename... Args>
    inline UniqueRef<T> MakeUnique(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    /*!
    wrapper around shared_ptr
    \params args arguments for construction of T object
    \returns shared_ptr<T>
    */
    template<typename T, typename... Args>
    inline Ref<T> MakeRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    /*!
    wrapper around new operator
    \params args arguments for construction of T object
    \returns new object of type T
    */
    template<typename T, typename... Args>
    inline T* Alloc(Args&&... args) { return new T(std::forward<Args>(args)...); }

    /*!
    wrapper around delete operator
    \param value value to delete
    */
    template<typename T>
    inline void Free(T* value) { delete value; }
}

void* operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line);

void* operator new[](size_t size, size_t align, size_t offset, const char* name, int flags, unsigned int debugFlags, const char* file, int line);