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

#include <utility>
#include <ostream>
#include <random>

#include "Utilities/STL/MxString.h"
#include "Utilities/Random/Random.h"
#include "Core/Macro/Macro.h"

namespace uuids
{
    class uuid;
    template<typename> class basic_uuid_random_generator;
}

namespace MxEngine
{
    class UUID
    {
        std::aligned_storage_t<16> uuidImpl;

        friend class UUIDGenerator;
    public:
        uuids::uuid& GetImpl();
        const uuids::uuid& GetImpl() const;
        size_t GetHashCode() const;
        bool operator==(const UUID& other) const;
        bool operator!=(const UUID& other) const;
        bool operator<(const UUID& other) const;
        bool operator>(const UUID& other) const;
        bool operator<=(const UUID& other) const;
        bool operator>=(const UUID& other) const;

        operator MxString() const;
    };

    std::ostream& operator<<(std::ostream& out, const UUID& uuid);

    struct UUIDGeneratorImpl
    {
        using type = uuids::basic_uuid_random_generator<Random::Generator>;
        std::aligned_storage_t<24> generator;
        type& GetGeneratorImpl();
    };

    class UUIDGenerator
    {
        static inline UUIDGeneratorImpl* storage;
    public:
        static void Init();
        static UUID Get();
        static UUID GetNull();
        static void Clone(UUIDGeneratorImpl* other);
        static UUIDGeneratorImpl* GetImpl();
    };
}

#define HASH_ALGORITHM(NAMESPACE)\
template <>\
struct NAMESPACE::hash<MxEngine::UUID>\
{\
    std::size_t operator()(const MxEngine::UUID& Id) const\
    {\
        return Id.GetHashCode();\
    }\
};

HASH_ALGORITHM(std)
HASH_ALGORITHM(eastl)
#undef HASH_ALGORITHM