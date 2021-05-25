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

#include "Utilities/UUID/UUID.h"
#include "Utilities/VectorPool/VectorPool.h"

namespace MxEngine
{
    template<typename T>
    struct ManagedResource
    {
        UUID uuid;
        T value;
        size_t refCount = 0;

        template<typename... Args>
        ManagedResource(const UUID& uuid, Args&&... value)
            : uuid(uuid), value(std::forward<Args>(value)...) { }

        ManagedResource(const ManagedResource&) = delete;
        ManagedResource(ManagedResource&&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;
        ManagedResource& operator=(const ManagedResource&) = delete;
        ManagedResource& operator=(ManagedResource&&) noexcept(std::is_nothrow_move_assignable_v<T>) = default;

        ~ManagedResource();
    };

    template<typename T, typename F>
    class Resource
    {
        UUID uuid;
        size_t handle;

        #if defined(MXENGINE_DEBUG)
        mutable ManagedResource<T>* _resourcePtr = nullptr;
        #endif

        static constexpr size_t InvalidHandle = std::numeric_limits<size_t>::max();

        void IncRef();
        void DecRef();
        void DestroyThis(Resource<T, F>& resource);
        [[nodiscard]] ManagedResource<T>& Dereference() const;
        [[nodiscard]]ManagedResource<T>& AccessThis(size_t handle) const;
    public:
        using Type = T;
        using Factory = F;

        Resource();
        Resource(UUID uuid, size_t handle);
        Resource(const Resource& wrapper);
        Resource& operator=(const Resource& wrapper);
        Resource(Resource&& wrapper) noexcept;
        Resource& operator=(Resource&& wrapper) noexcept;
        ~Resource();
        void MakeStatic();
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] T* operator->();
        [[nodiscard]] const T* operator->() const;
        [[nodiscard]] T& operator*();
        [[nodiscard]] const T& operator*() const;
        [[nodiscard]] T* GetUnchecked();
        [[nodiscard]] const T* GetUnchecked() const;
        [[nodiscard]] size_t GetHandle() const;
        [[nodiscard]] const UUID& GetUUID() const;
        [[nodiscard]] bool operator==(const Resource& wrapper) const;
        [[nodiscard]] bool operator!=(const Resource& wrapper) const;
        [[nodiscard]] bool operator<(const Resource& wrapper) const;
        [[nodiscard]] bool operator>(const Resource& wrapper) const;
        [[nodiscard]] bool operator<=(const Resource& wrapper) const;
        [[nodiscard]] bool operator>=(const Resource& wrapper) const;
    };

    template<typename T>
    class Factory
    {
        using FactoryPool = VectorPool<ManagedResource<T>>;
        using ThisType = Factory<T>;

        inline static FactoryPool* Pool = nullptr;

    public:
        [[nodiscard]] static FactoryPool& GetPool();
        [[nodiscard]] static FactoryPool* GetImpl();
        static void Init();
        static void Destroy();
        static void Clone(FactoryPool* other);
        [[nodiscard]] static Resource<T, ThisType> GetHandle(const ManagedResource<T>& object);
        [[nodiscard]] static Resource<T, ThisType> GetHandle(const T& object);
        static void Destroy(Resource<T, ThisType>& resource);

        template<typename U>
        [[nodiscard]] static FactoryPool& GetPool()
        {
            static_assert(std::is_same_v<T, U>, "type mismatch while accessing Factory<T>");
            return *Pool;
        }

        template<typename... Args>
        [[nodiscard]] static Resource<T, typename Factory<T>::ThisType> Create(Args&&... args)
        {
            UUID uuid = UUIDGenerator::Get();
            size_t index = Factory<T>::GetPool().Allocate(uuid, std::forward<Args>(args)...);
            return Resource<T, ThisType>(uuid, index);
        }
    };

    #define MXENGINE_MAKE_FACTORY(name) using name##Factory = Factory<name>; using name##Handle = Resource<name, name##Factory>
}