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

#include "Factory.h"

namespace MxEngine
{
    template<typename T>
    ManagedResource<T>::~ManagedResource()
    {
        uuid = UUIDGenerator::GetNull();
    }
    
    template<typename T, typename F>
    void Resource<T, F>::IncRef()
    {
        if (this->IsValid())
            ++this->Dereference().refCount;
    }

    template<typename T, typename F>
    void Resource<T, F>::DecRef()
    {
        if (this->IsValid())
        {
            auto& resource = this->Dereference();
            if ((--resource.refCount) == 0)
                DestroyThis(*this);
        }
    }

    template<typename T, typename F>
    [[nodiscard]] ManagedResource<T>& Resource<T, F>::Dereference() const
    {
        auto& resource = AccessThis(this->handle);

        #if defined(MXENGINE_DEBUG)
        this->_resourcePtr = &resource;
        #endif

        return resource;
    }

    template<typename T, typename F>
    void Resource<T, F>::DestroyThis(Resource<T, F>& resource)
    {
        F::Destroy(resource);
    }

    template<typename T, typename F>
    ManagedResource<T>& Resource<T, F>::AccessThis(size_t handle) const
    {
        return F::template GetPool<T>()[handle];
    }

    template<typename T, typename F>
    Resource<T, F>::Resource()
        : uuid(UUIDGenerator::GetNull()), handle(Resource<T, F>::InvalidHandle) { }

    template<typename T, typename F>
    Resource<T, F>::Resource(UUID uuid, size_t handle)
        : uuid(uuid), handle(handle)
    {
        this->IncRef();
    }

    template<typename T, typename F>
    Resource<T, F>::Resource(const Resource<T, F>& wrapper)
        : uuid(wrapper.uuid), handle(wrapper.handle)
    {
        this->IncRef();
        #if defined(MXENGINE_DEBUG)
        this->_resourcePtr = wrapper._resourcePtr;
        #endif
    }

    template<typename T, typename F>
    Resource<T, F>& Resource<T, F>::operator=(const Resource<T, F>& wrapper)
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

    template<typename T, typename F>
    Resource<T, F>::Resource(Resource<T, F>&& wrapper) noexcept
        : uuid(wrapper.uuid), handle(wrapper.handle)
    {
        #if defined(MXENGINE_DEBUG)
        this->_resourcePtr = wrapper._resourcePtr;
        #endif

        wrapper.handle = InvalidHandle;
    }

    template<typename T, typename F>
    Resource<T, F>& Resource<T, F>::operator=(Resource<T, F>&& wrapper) noexcept
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

    template<typename T, typename F>
    [[nodiscard]] bool Resource<T, F>::IsValid() const
    {
        return handle != InvalidHandle && Dereference().uuid == uuid;
    }

    template<typename T, typename F>
    void Resource<T, F>::MakeStatic()
    {
        this->IncRef();
    }

    template<typename T, typename F>
    T* Resource<T, F>::operator->()
    {
        MX_ASSERT(this->IsValid()); // access of null handle

        return this->GetUnchecked();
    }

    template<typename T, typename F>
    [[nodiscard]] const T* Resource<T, F>::operator->() const
    {
        MX_ASSERT(this->IsValid()); // access of null handle

        return this->GetUnchecked();
    }

    template<typename T, typename F>
    [[nodiscard]] T& Resource<T, F>::operator*()
    {
        MX_ASSERT(this->IsValid()); // access of null handle
        return *this->GetUnchecked();
    }

    template<typename T, typename F>
    [[nodiscard]] const T& Resource<T, F>::operator*() const
    {
        MX_ASSERT(this->IsValid()); // access of null handle
        return *this->GetUnchecked();
    }

    template<typename T, typename F>
    [[nodiscard]] T* Resource<T, F>::GetUnchecked()
    {
        return &this->Dereference().value;
    }

    template<typename T, typename F>
    [[nodiscard]] const T* Resource<T, F>::GetUnchecked() const
    {
        return &this->Dereference().value;
    }

    template<typename T, typename F>
    [[nodiscard]] size_t Resource<T, F>::GetHandle() const
    {
        return this->handle;
    }

    template<typename T, typename F>
    [[nodiscard]] const UUID& Resource<T, F>::GetUUID() const
    {
        return this->uuid;
    }

    template<typename T, typename F>
    [[nodiscard]] bool Resource<T, F>::operator==(const Resource<T, F>& wrapper) const
    {
        return this->handle == wrapper.handle && this->uuid == wrapper.uuid;
    }

    template<typename T, typename F>
    [[nodiscard]] bool Resource<T, F>::operator!=(const Resource<T, F>& wrapper) const
    {
        return !(*this == wrapper);
    }

    template<typename T, typename F>
    [[nodiscard]] bool Resource<T, F>::operator<(const Resource<T, F>& wrapper) const
    {
        return (this->handle != wrapper.handle) ? (this->handle < wrapper.handle) : (this->uuid < wrapper.uuid);
    }

    template<typename T, typename F>
    [[nodiscard]] bool Resource<T, F>::operator>(const Resource<T, F>& wrapper) const
    {
        return wrapper < *this;
    }

    template<typename T, typename F>
    [[nodiscard]] bool Resource<T, F>::operator<=(const Resource<T, F>& wrapper) const
    {
        return !(wrapper < *this);
    }

    template<typename T, typename F>
    [[nodiscard]] bool Resource<T, F>::operator>=(const Resource<T, F>& wrapper) const
    {
        return !(*this < wrapper);
    }

    template<typename T, typename F>
    Resource<T, F>::~Resource()
    {
        this->DecRef();
    }

    template<typename T>
    [[nodiscard]] typename Factory<T>::FactoryPool& Factory<T>::GetPool()
    {
        return *Pool;
    }
    
    template<typename T>
    [[nodiscard]] typename Factory<T>::FactoryPool* Factory<T>::GetImpl()
    {
        return Pool;
    }
    
    template<typename T>
    void Factory<T>::Init()
    {
        if (Pool == nullptr)
            Pool = new FactoryPool(); // not deleted, but its static member, so it does not matter
    }
    
    template<typename T>
    void Factory<T>::Destroy()
    {
        MX_ASSERT(Pool != nullptr);
        delete Pool;
        Pool = nullptr;
    }
    
    template<typename T>
    void Factory<T>::Clone(FactoryPool* other)
    {
        Pool = other;
    }
    
    template<typename T>
    [[nodiscard]] Resource<T, typename Factory<T>::ThisType> Factory<T>::GetHandle(const ManagedResource<T>& object)
    {
        auto& pool = Factory<T>::GetPool();
        size_t index = pool.IndexOf(object);
        return Resource<T, ThisType>(pool[index].uuid, index);
    }
    
    template<typename T>
    [[nodiscard]] Resource<T, typename Factory<T>::ThisType> Factory<T>::GetHandle(const T& object)
    {
        auto ptr = (const uint8_t*)std::addressof(object);
        auto resourcePtr = (const ManagedResource<T>*)(ptr - sizeof(UUID));
        return Factory<T>::GetHandle(*resourcePtr);
    }
    
    template<typename T>
    void Factory<T>::Destroy(Resource<T, typename Factory<T>::ThisType>& resource)
    {
        Factory<T>::GetPool().Deallocate(resource.GetHandle());
    }
}