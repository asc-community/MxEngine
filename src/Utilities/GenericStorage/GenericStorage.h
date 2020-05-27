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

#include "Utilities/Memory/Memory.h"

#include <vector>

namespace MxEngine
{
    /*!
    multi storage is a template class which is used to store objects of different types
    usually you want to write something like GenericStorage<std::vector, int, char, float>,
    which results in object which contains std::vector<int>, std::vector<float>, std::vector<char>
    */
    template<template<typename> typename Storage, typename... Args>
    class MultiStorage;

    template<template<typename> typename Storage>
    class MultiStorage<Storage>
    {
    public:
        template<typename Type, typename U>
        U& Get() { static_assert(false, "no suitable type in generic storage"); }

        void Clear() { }
    };

    template<template<typename> typename Storage, typename T, typename... Args>
    class MultiStorage<Storage, T, Args...> : public MultiStorage<Storage, Args...>
    {
        using Base = MultiStorage<Storage, Args...>;

        /*!
        underlying storage. use Get() method to retrieve it
        */
        Storage<T> resources;

    public:
        template<typename Resource>
        using ConcreteStorage = Storage<Resource>;

        /*!
        returns storage of objects with type = Type
        if it is not in MultiStorage, static_assert is triggered
        */
        template<typename Type>
        auto& Get()
        {
            if constexpr (std::is_same<Type, T>::value)
                return this->resources;
            else
                return static_cast<Base*>(this)->Get<Type>();
        }

        /*!
        clears all underlying storages of multi storage
        */
        void Clear()
        {
            this->resources.Clear();
            static_cast<Base*>(this)->Clear();
        }
    };
}