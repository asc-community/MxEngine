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

#include <Vendors/eastl/EASTL/hash_map.h>
#include <Vendors/eastl/EASTL/fixed_hash_map.h>

namespace MxEngine
{
    template<typename T, typename U, typename Hash = eastl::hash<T>, typename Predicate = eastl::equal_to<T>, typename Allocator = EASTLAllocatorType >
    using MxHashMap = eastl::hash_map<T, U, Hash, Predicate, Allocator>;

    template<typename T, typename U, size_t Nodes, bool overflow = true, typename Allocator = EASTLAllocatorType, typename Hash = eastl::hash<T>, typename Predicate = eastl::equal_to<T>>
    using MxFixedHashMap = eastl::fixed_hash_map<T, U, Nodes, Nodes + 1, overflow, Hash, Predicate, false, Allocator>;

    template<typename T, typename U, typename Hash = eastl::hash<T>, typename Predicate = eastl::equal_to<T>, typename Allocator = EASTLAllocatorType >
    using MxHashMultiMap = eastl::hash_multimap<T, U, Hash, Predicate, Allocator>;

    template<typename T, typename U, size_t Nodes, bool overflow = true, typename Allocator = EASTLAllocatorType, typename Hash = eastl::hash<T>, typename Predicate = eastl::equal_to<T>>
    using MxFixedHashMultiMap = eastl::fixed_hash_multimap<T, U, Nodes, Nodes + 1, overflow, Hash, Predicate, false, Allocator>;
}