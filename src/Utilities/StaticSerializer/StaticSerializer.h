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
// and /or other materials provided wfith the distribution.
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

#include <tuple>

namespace MxEngine
{
    template<typename... Args>
    class StaticSerializer
    {
        template<typename T, typename... Other>
        static void InitializeImpl()
        {
            T::Init();
            if constexpr(sizeof...(Other) > 0)
                InitializeImpl<Other...>();
        }

        template<typename T, typename... Other>
        static auto SerializeImpl()
        {
            auto current = std::make_tuple(T::GetImpl());
            if constexpr (sizeof...(Other) > 0)
                return std::tuple_cat(current, SerializeImpl<Other...>());
            else
                return current;
        }

        template<size_t CurrentIndex, typename T, typename... Other, typename Tuple>
        static void DeserializeImpl(Tuple& t)
        {
            T::Clone(std::get<CurrentIndex>(t));
            if constexpr (sizeof...(Other) > 0)
                DeserializeImpl<CurrentIndex + 1, Other...>(t);
        }

    public:
        static void Initialize()
        {
            InitializeImpl<Args...>();
        }

        static auto Serialize()
        {
            return SerializeImpl<Args...>();
        }

        template<typename Tuple>
        static void Deserialize(Tuple& staticData)
        {
            DeserializeImpl<0, Args...>(staticData);
        }

        static void Deserialize(void* erasedTuple)
        {
            using Tuple = decltype(Serialize());
            Deserialize(*((Tuple*)erasedTuple));
        }
    };
}