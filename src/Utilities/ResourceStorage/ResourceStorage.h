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

#include <unordered_map>
#include <string>

#include "Utilities/Memory/Memory.h"
#include "Utilities/Logger/Logger.h"

namespace MxEngine
{
    /*!
    ResourceStorage is a small generic wrapper around STL container which replace iterator compares to human-readable functions.
    To use it, you probably would like to write something like ResourceStorage<MxObject, ID>, where MxObject is value and ID is a key.
    */
    template<typename T, typename Key = std::string>
    class ResourceStorage
    {
        using BaseStorage = std::unordered_map<Key, UniqueRef<T>>;

        /*!
        inner STL container that stores objects
        */
        BaseStorage storage;
    public:
        /*!
        getter for innet STL container
        \returns storage private member
        */
        BaseStorage& GetStorage() { return storage; }
        /*!
        constant getter for innet STL container
        \returns storage private member
        */
        const BaseStorage& GetStorage() const { return storage; }

        /*!
        getter for object in storage
        \param key unique id to search for
        \returns object pointer if it exists, nullptr either
        */
        T* Get(const Key& key)
        {
            auto it = this->storage.find(key);
            if (it == this->storage.end()) return nullptr;
            return it->second.get();
        }

        /*!
        checks if object is inside generic storage.
        \param key unique id to search for
        \returns true if object with id exists, false either
        */
        bool Exists(const Key& key)
        {
            return this->storage.find(key) != this->storage.end();
        }

        /*!
        adds or replaces object in storage
        \param key unique id of object
        \param value object itself
        \returns pointer to object with key provided
        */
        T* Add(const Key& key, UniqueRef<T> value)
        {
            Logger::Instance().Debug("MxEngine::ResourceStorage", "adding resource: " + key);
            T* ret = value.get();
            this->storage[key] = std::move(value);
            return ret;
        }

        /*!
        removes object in storage if it exists
        \param key unique id of object
        */
        void Delete(const Key& key)
        {
            if (this->Exists(key))
            {
                this->storage.erase(key);
                Logger::Instance().Debug("MxEngine::ResourceStorage", "deleting resource: " + key);
            }
        }

        /*!
        destroying all objects in storage
        */
        void Clear()
        {
            Logger::Instance().Debug("MxEngine::ResourceStorage", "deleting all associated resources: " + (std::string)typeid(T).name());
            this->storage.clear();
        }
    };
}

#include "Utilities/UUID/UUID.h"
#include "Utilities/Memory/PoolAllocator.h"

namespace MxEngine
{
    template<typename T>
    struct ManagedResource
    {
        UUID uuid;
        T value;
        size_t refCount = 0;

        ManagedResource(UUID uuid, T&& value)
            : uuid(uuid), value(std::move(value))
        {
        }

        ~ManagedResource()
        {
            uuid = UUIDGenerator::GetNull();
        }
    };

    template<typename T>
    class ResourceWrapper
    {
        UUID uuid;
        ManagedResource<T>* handle;

        void IncRef()
        {
            if (this->IsValid())
                this->handle->refCount++;
        }

        void DecRef()
        {
            if (this->IsValid())
                this->handle->refCount--;
        }
    public:
        ResourceWrapper(UUID uuid, ManagedResource<T>* handle)
            : uuid(uuid), handle(handle)
        {
            this->IncRef();
        }

        ResourceWrapper(const ResourceWrapper& wrapper)
            : uuid(wrapper.uuid), handle(wrapper.handle)
        {
            this->IncRef();
        }

        ResourceWrapper& operator=(const ResourceWrapper& wrapper)
        {
            this->DecRef();

            this->uuid = wrapper.uuid;
            this->handle = wrapper.handle;
            this->IncRef();
        }

        ResourceWrapper(ResourceWrapper&& wrapper) noexcept
            : uuid(wrapper.uuid), handle(wrapper.handle)
        {
            wrapper.handle = nullptr;
        }

        ResourceWrapper& operator=(ResourceWrapper&& wrapper) noexcept
        {
            this->DecRef();
            this->uuid = wrapper.uuid;
            this->handle = wrapper.handle;
            wrapper.handle = nullptr;
        }

        bool IsValid() const
        {
            return handle != nullptr && handle->uuid == uuid;
        }

        T* operator->()
        {
            if (!this->IsValid()) return nullptr;
            return this->GetUnchecked();
        }

        const T* operator->() const
        {
            if (!this->IsValid()) return nullptr;
            return this->GetUnchecked();
        }

        T* GetUnchecked()
        {
            return &this->handle->value;
        }

        const T* GetUnchecked() const
        {
            return &this->handle->value;
        }

        ManagedResource<T>* GetHandle()
        {
            return this->handle;
        }

        ~ResourceWrapper()
        {
            if (this->IsValid())
                this->handle->refCount--;
        }
    };

    template<typename T>
    class ResourceManager
    {
    public:
        using Allocator = PoolAllocator<ManagedResource<T>>;
    private:
        Allocator allocator;
    public:
        ResourceManager(typename Allocator::DataPointer storage, size_t bytes)
            : allocator(storage, bytes)
        {
        }

        ResourceManager(const ResourceManager&) = delete;

        const Allocator& GetAllocator() const { return this->allocator; }

        template<typename... Args>
        ResourceWrapper<T> Create(Args... args)
        {
            auto resource = allocator.Alloc(UUIDGenerator::Get(), T(std::forward<Args>(args)...));
            return ResourceWrapper<T>(resource->uuid, resource);
        }

        void Destroy(ResourceWrapper<T>& wrapper)
        {
            if (!wrapper.IsValid()) return;
            allocator.Free(wrapper.GetHandle());
        }
    };

    template<typename T, typename... Args>
    class GenericResourceManager
        : public GenericResourceManager<Args...>
    {
        using Base = GenericResourceManager<Args...>;

        ResourceManager<T> manager;
    public:
        template<typename Allocator>
        GenericResourceManager(Allocator& allocator, size_t bytesPerManager)
            : Base(allocator, bytesPerManager),
            manager(allocator.RawAlloc(bytesPerManager, 16), bytesPerManager)
        {
        }

        GenericResourceManager(const GenericResourceManager&) = delete;

        template<typename U>
        auto& Get()
        {
            if constexpr (std::is_same<T, U>::value)
                return this->manager;
            else
                return ((Base*)this)->Get<U>();
        }

        template<typename F>
        void ForEach(F&& func)
        {
            func(this->Get<T>());
            ((Base*)this)->ForEach(std::forward<F>(func));
        }
    };

    template<typename T>
    class GenericResourceManager<T>
    {
        ResourceManager<T> manager{ nullptr, 0 };
    public:
        template<typename Allocator>
        GenericResourceManager(Allocator& allocator, size_t bytesPerManager)
            : manager(allocator.RawAlloc(bytesPerManager, 16), bytesPerManager)
        {
        }

        GenericResourceManager(const GenericResourceManager&) = delete;

        template<typename U>
        auto& Get()
        {
            static_assert(std::is_same<T, U>::value, "cannot find appropriate ResourceManager<T>");
            return this->manager;
        }

        template<typename F>
        void ForEach(F&& func)
        {
            func(this->Get<T>());
        }
    };
}