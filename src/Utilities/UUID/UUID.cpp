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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace MxEngine
{
    void UUIDGenerator::Init()
    {
        static_assert(sizeof(storage->generator) == sizeof(boost::uuids::random_generator), "aligned storage must fit implementation size");
        static_assert(sizeof(std::declval<UUID>().uuidImpl) == sizeof(boost::uuids::uuid), "aligned storage must fit implementation size");

        storage = Alloc<UUIDGeneratorImpl>();
        new(&storage->generator) boost::uuids::random_generator();
    }

    UUID UUIDGenerator::Get()
    {
        UUID uuid;
        uuid.GetImpl() = storage->GetGeneratorImpl()();
        return uuid;
    }

    UUID UUIDGenerator::GetNull()
    {
        UUID uuid;
        boost::uuids::nil_generator nullGenerator;
        uuid.GetImpl() = nullGenerator();
        return uuid;
    }

    void UUIDGenerator::Clone(UUIDGeneratorImpl* other)
    {
        storage = other;
    }

    UUIDGeneratorImpl* UUIDGenerator::GetImpl()
    {
        return storage;
    }


    boost::uuids::uuid& UUID::GetImpl()
    {
        return *reinterpret_cast<boost::uuids::uuid*>(&uuidImpl);
    }

    const boost::uuids::uuid& UUID::GetImpl() const
    {
        return *reinterpret_cast<const boost::uuids::uuid*>(&uuidImpl);
    }

    size_t UUID::GetHashCode() const
    {
        return boost::uuids::hash_value(this->GetImpl());
    }

    bool UUID::operator==(const UUID& other) const
    {
        return this->GetImpl() == other.GetImpl();
    }

    bool UUID::operator<(const UUID& other) const
    {
        return this->GetImpl() < other.GetImpl();
    }

    bool UUID::operator!=(const UUID& other) const
    {
        return this->GetImpl() != other.GetImpl();
    }

    bool UUID::operator>(const UUID& other) const
    {
        return this->GetImpl() > other.GetImpl();
    }

    bool UUID::operator<=(const UUID& other) const
    {
        return this->GetImpl() <= other.GetImpl();
    }

    bool UUID::operator>=(const UUID& other) const
    {
        return this->GetImpl() >= other.GetImpl();
    }

    UUID::operator MxString() const
    {
        // copied from boost::uuids::to_string
        MxString result;
        result.reserve(36);

        std::size_t i = 0;
        for (auto it = this->GetImpl().begin(); it != this->GetImpl().end(); ++it, ++i) {
            const size_t hi = ((*it) >> 4) & 0x0F;
            result += boost::uuids::detail::to_char(hi);

            const size_t lo = (*it) & 0x0F;
            result += boost::uuids::detail::to_char(lo);

            if (i == 3 || i == 5 || i == 7 || i == 9) {
                result += '-';
            }
        }
        return result;
    }

    boost::uuids::random_generator_pure& UUIDGeneratorImpl::GetGeneratorImpl()
    {
        return *reinterpret_cast<boost::uuids::random_generator*>(&generator);
    }

    std::ostream& operator<<(std::ostream& out, const UUID& uuid)
    {
        return out << uuid.GetImpl();
    }

    std::istream& operator>>(std::istream& in, UUID& uuid)
    {
        return in >> uuid.GetImpl();
    }
}