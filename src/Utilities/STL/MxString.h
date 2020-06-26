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

#include <Vendors/eastl/EASTL/string.h>
#include <Vendors/eastl/EASTL/fixed_string.h>
#include <string>

namespace MxEngine
{
    using MxString = eastl::basic_string<char>;

    template<size_t Nodes, bool overflow = true, typename Allocator = EASTLAllocatorType>
    using MxFixedString = eastl::basic_string<char, eastl::fixed_vector_allocator<sizeof(char), Nodes, EASTL_ALIGN_OF(char), 0, overflow, Allocator>>;

    template<typename T>
    inline auto ToMxString(const T& value)
    {
        return eastl::to_string(value);
    }

    inline auto ToMxString(const std::string& value)
    {
        return MxString{ value.c_str() };
    }
}

namespace EA::StdC
{
     int Vsnprintf(char* EA_RESTRICT s, std::size_t n, char const* EA_RESTRICT format, char* args);
}