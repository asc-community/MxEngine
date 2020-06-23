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
            : value(std::forward<Args>(value)...), uuid(uuid)
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

    template<typename T, typename Factory>
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

        void DestroyThis(Resource<T, Factory>& resource)
        {
            Factory::Destroy(resource);
        }

        ManagedResource<T>& AccessThis(size_t handle) const
        {
            return Factory::template Get<T>()[handle];
        }
    public:
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
            MX_ASSERT(this->IsValid());
            if (!this->IsValid()) return nullptr;
            return this->GetUnchecked();
        }

        [[nodiscard]] const T* operator->() const
        {
            MX_ASSERT(this->IsValid());
            if (!this->IsValid()) return nullptr;
            return this->GetUnchecked();
        }

        [[nodiscard]] T& operator*()
        {
            MX_ASSERT(this->IsValid());
            return *this->GetUnchecked();
        }

        [[nodiscard]] const T& operator*() const
        {
            MX_ASSERT(this->IsValid());
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

        ~Resource()
        {
            this->DecRef();
        }
    };

    template<typename T, typename Factory>
    class LocalResource
    {
        UUID uuid;
        size_t handle;
        mutable Factory* factory;

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

        void DestroyThis(LocalResource<T, Factory>& resource)
        {
            this->factory->Destroy(resource);
        }

        ManagedResource<T>& AccessThis(size_t handle) const
        {
            return this->factory->template Get<T>()[handle];
        }
    public:
        LocalResource()
            : uuid(UUIDGenerator::GetNull()), handle(InvalidHandle), factory(nullptr)
        {

        }

        LocalResource(UUID uuid, size_t handle, Factory* factory)
            : uuid(uuid), handle(handle), factory(factory)
        {
            this->IncRef();
        }

        LocalResource(const LocalResource& wrapper)
            : uuid(wrapper.uuid), handle(wrapper.handle), factory(wrapper.factory)
        {
            this->IncRef();
        }

        LocalResource& operator=(const LocalResource& wrapper)
        {
            this->DecRef();

            this->uuid = wrapper.uuid;
            this->handle = wrapper.handle;
            this->factory = wrapper.factory;
            this->IncRef();

            return *this;
        }

        LocalResource(LocalResource&& wrapper) noexcept
            : uuid(wrapper.uuid), handle(wrapper.handle), factory(wrapper.factory)
        {
            wrapper.handle = InvalidHandle;
        }

        LocalResource& operator=(LocalResource&& wrapper) noexcept
        {
            this->DecRef();
            this->uuid = wrapper.uuid;
            this->handle = wrapper.handle;
            this->factory = wrapper.factory;
            wrapper.handle = InvalidHandle;

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
            MX_ASSERT(this->IsValid());
            if (!this->IsValid()) return nullptr;
            return this->GetUnchecked();
        }

        [[nodiscard]] const T* operator->() const
        {
            MX_ASSERT(this->IsValid());
            if (!this->IsValid()) return nullptr;
            return this->GetUnchecked();
        }

        [[nodiscard]] T& operator*()
        {
            MX_ASSERT(this->IsValid());
            return *this->GetUnchecked();
        }

        [[nodiscard]] const T& operator*() const
        {
            MX_ASSERT(this->IsValid());
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

        ~LocalResource()
        {
            this->DecRef();
        }
    };

    template<typename T, typename... Args>
    struct FactoryImpl : FactoryImpl<Args...>
    {
        using Base = FactoryImpl<Args...>;
        using Pool = VectorPool<ManagedResource<T>>;
        Pool pool;

        template<typename U>
        auto& GetPool()
        {
            if constexpr (std::is_same<T, U>::value)
                return this->pool;
            else
                return static_cast<Base*>(this)->template GetPool<U>();
        }

        template<typename F>
        void ForEach(F&& func)
        {
            func(this->pool);
            static_cast<Base*>(this)->ForEach(std::forward<F>(func));
        }
    };

    template<typename T>
    struct FactoryImpl<T>
    {
        using FactoryPool = VectorPool<ManagedResource<T>>;
        FactoryPool Pool;

        template<typename U>
        auto& GetPool()
        {
            static_assert(std::is_same<T, U>::value, "cannot find appropriate Factory<T>");
            return this->Pool;
        }

        template<typename F>
        void ForEach(F&& func)
        {
            func(this->Pool);
        }
    };

    template<typename... Args>
    class AbstractFactoryImpl
    {
    public:
        using Factory = FactoryImpl<Args...>;
        using ThisType = AbstractFactoryImpl<Args...>;
    private:
        inline static Factory* factory = nullptr;
    public:

        static Factory* GetImpl()
        {
            return factory;
        }

        static void Init()
        {
            if (factory == nullptr)
                factory = new Factory(); // not deleted, but its static member, so it does not matter
        }

        static void Clone(Factory* other)
        {
            factory = other;
        }

        template<typename U>
        static auto& Get()
        {
            return factory->template GetPool<U>();
        }

        template<typename T, typename... ConstructArgs>
        static Resource<T, ThisType> Create(ConstructArgs&&... args)
        {
            MX_ASSERT(factory != nullptr);
            UUID uuid = UUIDGenerator::Get();
            auto& pool = factory->template GetPool<T>();
            size_t index = pool.Allocate(uuid, std::forward<ConstructArgs>(args)...);
            return Resource<T, ThisType>(uuid, index);
        }

        template<typename T>
        static void Destroy(Resource<T, ThisType>& resource)
        {
            factory->template GetPool<T>().Deallocate(resource.GetHandle());
        }
    };
}