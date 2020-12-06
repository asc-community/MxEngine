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

#include "UUID.h"
#include "Utilities/Memory/Memory.h"

// do not include system headers or link platform-specific libraries
#undef _WIN32
#undef __linux__
#undef __unix__
#undef __APPLE__
#include <gsl>
#include <uuid.h>

namespace MxEngine
{
    void UUIDGenerator::Init()
    {
        static_assert(sizeof(storage->generator) >= sizeof(uuids::uuid_random_generator),
            "aligned storage must fit implementation size");
        static_assert(AssertEquality<sizeof(std::declval<UUID>().uuidImpl), sizeof(uuids::uuid)>::value,
                "aligned storage must fit implementation size");

        storage = Alloc<UUIDGeneratorImpl>();
        (void)new(&storage->generator) UUIDGeneratorImpl::type(Random::GetImpl());
    }

    UUID UUIDGenerator::Get()
    {
        UUID uuid;
        uuid.GetImpl() = storage->GetGeneratorImpl()();
        return uuid;
    }

    UUID UUIDGenerator::GetNull()
    {
        return UUID{ };
    }

    void UUIDGenerator::Clone(UUIDGeneratorImpl* other)
    {
        storage = other;
    }

    UUIDGeneratorImpl* UUIDGenerator::GetImpl()
    {
        return storage;
    }


    uuids::uuid& UUID::GetImpl()
    {
        return *std::launder(reinterpret_cast<uuids::uuid*>(&uuidImpl));
    }

    const uuids::uuid& UUID::GetImpl() const
    {
        return *std::launder(reinterpret_cast<const uuids::uuid*>(&uuidImpl));
    }

    size_t UUID::GetHashCode() const
    {
        return std::hash<uuids::uuid>{ }(this->GetImpl());
    }

    bool UUID::operator==(const UUID& other) const
    {
        return this->GetImpl() == other.GetImpl();
    }

    bool UUID::operator!=(const UUID& other) const
    {
        return !(*this == other);
    }

    bool UUID::operator<(const UUID& other) const
    {
        auto b1 = this->GetImpl().as_bytes();
        auto b2 = other.GetImpl().as_bytes();
        
        const uint64_t* d1 = std::launder(reinterpret_cast<const uint64_t*>(b1.data()));
        const uint64_t* d2 = std::launder(reinterpret_cast<const uint64_t*>(b2.data()));

        return (d1[0] == d2[0]) ? (d1[1] < d2[1]) : (d1[0] < d2[0]);
    }

    bool UUID::operator>(const UUID& other) const
    {
        return other < *this;
    }

    bool UUID::operator<=(const UUID& other) const
    {
        return !(other < *this);
    }

    bool UUID::operator>=(const UUID& other) const
    {
        return !(*this < other);
    }

    UUID::operator MxString() const
    {
        return ToMxString(uuids::to_string(this->GetImpl()));
    }

    UUIDGeneratorImpl::type& UUIDGeneratorImpl::GetGeneratorImpl()
    {
        return *std::launder(reinterpret_cast<UUIDGeneratorImpl::type*>(&generator));
    }

    std::ostream& operator<<(std::ostream& out, const UUID& uuid)
    {
        return out << uuid.GetImpl();
    }
}
