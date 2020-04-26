// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "Utilities/Memory/Memory.h"

#include <vector>

namespace MxEngine
{
    template<template<typename> typename Storage, typename... Args>
    class GenericStorage;

    template<template<typename> typename Storage>
    class GenericStorage<Storage>
    {
    public:
        template<typename Type, typename U>
        U Get() { static_assert(false, "no suitable type in generic storage"); }

        void Clear() { }
    };

    template<template<typename> typename Storage, typename T, typename... Args>
    class GenericStorage<Storage, T, Args...> : public GenericStorage<Storage, Args...>
    {
        using Base = GenericStorage<Storage, Args...>;

        Storage<T> resources;

    public:
        template<typename Resource>
        using ConcreteStorage = Storage<Resource>;

        template<typename Type>
        auto Get() -> typename std::enable_if<std::is_same<T, Type>::value, Storage<Type>&>::type
        {
            return resources;
        }

        template<typename Type>
        auto Get() -> typename std::enable_if<!std::is_same<T, Type>::value, Storage<Type>&>::type
        {
            return static_cast<Base*>(this)->Get<Type>();
        }

        void Clear()
        {
            this->resources.Clear();
            static_cast<Base*>(this)->Clear();
        }
    };
}