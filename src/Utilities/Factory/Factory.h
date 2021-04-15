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
        ManagedResource(UUID uuid, Args&&... value)
            : uuid(uuid), value(std::forward<Args>(value)...)
        {
        }

        ManagedResource(const ManagedResource&) = delete;
        ManagedResource(ManagedResource&&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;
        ManagedResource& operator=(const ManagedResource&) = delete;
        ManagedResource& operator=(ManagedResource&&) noexcept(std::is_nothrow_move_assignable_v<T>) = default;

        ~ManagedResource()
        {
            uuid = UUIDGenerator::GetNull();
        }
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

        void IncRef()
        {
            if (this->IsValid())
                ++this->Dereference().refCount;
        }
    
        void DecRef()
        {
            if (this->IsValid())
            {
                auto& resource = this->Dereference();
                if ((--resource.refCount) == 0)
                    DestroyThis(*this);
            }
        }

        [[nodiscard]] ManagedResource<T>& Dereference() const 
        {
            auto& resource = AccessThis(this->handle);

            #if defined(MXENGINE_DEBUG)
            this->_resourcePtr = &resource;
            #endif

            return resource;
        }

        void DestroyThis(Resource<T, F>& resource)
        {
            F::Destroy(resource);
        }

        ManagedResource<T>& AccessThis(size_t handle) const
        {
            return F::template GetPool<T>()[handle];
        }
    public:
        using Type = T;
        using Factory = F;

        Resource()
            : uuid(UUIDGenerator::GetNull()), handle(InvalidHandle)
        {

        }

        Resource(UUID uuid, size_t handle)
            : uuid(uuid), handle(handle)
        {
            this->IncRef();
        }

        Resource(const Resource& wrapper)
            : uuid(wrapper.uuid), handle(wrapper.handle)
        {
            this->IncRef();
            #if defined(MXENGINE_DEBUG)
            this->_resourcePtr = wrapper._resourcePtr;
            #endif
        }

        Resource& operator=(const Resource& wrapper)
        {
            this->DecRef();

            #if defined(MXENGINE_DEBUG)
            this->_resourcePtr = wrapper._resourcePtr;
            #endif

            this->uuid = wrapper.uuid;
            this->handle = wrapper.handle;
            this->IncRef();

            return *this;
        }

        Resource(Resource&& wrapper) noexcept
            : uuid(wrapper.uuid), handle(wrapper.handle)
        {
            #if defined(MXENGINE_DEBUG)
            this->_resourcePtr = wrapper._resourcePtr;
            #endif
            wrapper.handle = InvalidHandle;
        }

        Resource& operator=(Resource&& wrapper) noexcept
        {
            this->DecRef();
            this->uuid = wrapper.uuid;
            this->handle = wrapper.handle;
            wrapper.handle = InvalidHandle;

            #if defined(MXENGINE_DEBUG)
            this->_resourcePtr = wrapper._resourcePtr;
            #endif
            return *this;
        }

        [[nodiscard]] bool IsValid() const
        {
            return handle != InvalidHandle && Dereference().uuid == uuid;
        }

        void MakeStatic()
        {
            this->IncRef();
        }

        T* operator->()
        {
            MX_ASSERT(this->IsValid()); // access of null handle
            #if !defined(MXENGINE_SHIPPING)
            if (!this->IsValid()) return nullptr;
            #endif
            return this->GetUnchecked();
        }

        [[nodiscard]] const T* operator->() const
        {
            MX_ASSERT(this->IsValid()); // access of null handle
            #if !defined(MXENGINE_SHIPPING)
            if (!this->IsValid()) return nullptr;
            #endif
            return this->GetUnchecked();
        }

        [[nodiscard]] T& operator*()
        {
            MX_ASSERT(this->IsValid()); // access of null handle
            return *this->GetUnchecked();
        }

        [[nodiscard]] const T& operator*() const
        {
            MX_ASSERT(this->IsValid()); // access of null handle
            return *this->GetUnchecked();
        }

        [[nodiscard]] T* GetUnchecked()
        {
            return &this->Dereference().value;
        }

        [[nodiscard]] const T* GetUnchecked() const
        {
            return &this->Dereference().value;
        }

        [[nodiscard]] auto GetHandle() const
        {
            return this->handle;
        }

        [[nodiscard]] const auto& GetUUID() const
        {
            return this->uuid;
        }

        [[nodiscard]] bool operator==(const Resource& wrapper) const
        {
            return this->handle == wrapper.handle && this->uuid == wrapper.uuid;
        }

        [[nodiscard]] bool operator!=(const Resource& wrapper) const
        {
            return !(*this == wrapper);
        }

        [[nodiscard]] bool operator<(const Resource& wrapper) const
        {
            return (this->handle != wrapper.handle) ? (this->handle < wrapper.handle) : (this->uuid < wrapper.uuid);
        }

        [[nodiscard]] bool operator>(const Resource& wrapper) const
        {
            return wrapper < *this;
        }

        [[nodiscard]] bool operator<=(const Resource& wrapper) const
        {
            return !(wrapper < *this);
        }

        [[nodiscard]] bool operator>=(const Resource& wrapper) const
        {
            return !(*this < wrapper);
        }

        ~Resource()
        {
            this->DecRef();
        }
    };

    template<typename T>
    class Factory
    {
        using FactoryPool = VectorPool<ManagedResource<T>>;
        using ThisType = Factory<T>;

        inline static FactoryPool* Pool = nullptr;

    public:
        [[nodiscard]] static FactoryPool& GetPool()
        {
            return *Pool;
        }

        template<typename U>
        [[nodiscard]] static FactoryPool& GetPool()
        {
            static_assert(std::is_same_v<T, U>, "wrong factory object type");
            return GetPool();
        }

        [[nodiscard]] static FactoryPool* GetImpl()
        {
            return Pool;
        }

        static void Init()
        {
            if (Pool == nullptr)
                Pool = new FactoryPool(); // not deleted, but its static member, so it does not matter
        }

        static void Destroy()
        {
            MX_ASSERT(Pool != nullptr);
            delete Pool;
            Pool = nullptr;
        }

        static void Clone(FactoryPool* other)
        {
            Pool = other;
        }

        template<typename... Args>
        [[nodiscard]] static Resource<T, ThisType> Create(Args&&... args)
        {
            UUID uuid = UUIDGenerator::Get();
            size_t index = GetPool().Allocate(uuid, std::forward<Args>(args)...);
            return Resource<T, ThisType>(uuid, index);
        }

        [[nodiscard]] static Resource<T, ThisType> GetHandle(const ManagedResource<T>& object)
        {
            auto& pool = GetPool();
            size_t index = pool.IndexOf(object);
            return Resource<T, ThisType>(pool[index].uuid, index);
        }

        [[nodiscard]] static Resource<T, ThisType> GetHandle(const T& object)
        {
            auto ptr = (const uint8_t*)std::addressof(object);
            auto resourcePtr = (const ManagedResource<T>*)(ptr - offsetof(ManagedResource<T>, value));
            return GetHandle(*resourcePtr);
        }

        static void Destroy(Resource<T, ThisType>& resource)
        {
            GetPool().Deallocate(resource.GetHandle());
        }
    };

    #define MXENGINE_MAKE_FACTORY(name) using name##Factory = Factory<name>; using name##Handle = Resource<name, name##Factory>
}