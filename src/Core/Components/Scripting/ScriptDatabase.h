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

#include "Core/Runtime/Reflection.h"
#include "Utilities/STL/MxHashMap.h"
#include "Utilities/STL/MxString.h"

namespace MxEngine
{
    class ScriptDatabase
    {
    public:
        using GenericType = rttr::variant;

    private:
        MxHashMap<MxString, GenericType> database;

    public:
        const auto& GetDatabase() const { return this->database; }
        void SetDatabase(const MxHashMap<MxString, GenericType>& database) { this->database = database; }

        const GenericType& GetGeneric(const char* name) const;
        const GenericType& GetGeneric(const MxString& name) const;

        void Remove(const char* name);
        void Remove(const MxString& name);

        bool Contains(const char* name) const;
        bool Contains(const MxString& name) const;
        
        template<typename T>
        T Get(const char* name) const
        {
            return this->GetGeneric(name).convert<T>();
        }

        template<typename T>
        T Get(const MxString& name) const
        {
            return this->GetGeneric(name).convert<T>();
        }

        template<typename T>
        void Add(const char* name, T value)
        {
            this->database[name] = std::move(value);
        }

        template<typename T>
        void Add(const MxString& name, T value)
        {
            this->database[name] = std::move(value);
        }
    };
}