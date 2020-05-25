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

#include "Utilities/STL/MxHashMap.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/Logger/Logger.h"

namespace MxEngine
{
    /*!
    ResourceStorage is a small generic wrapper around STL container which replace iterator compares to human-readable functions.
    To use it, you probably would like to write something like ResourceStorage<MxObject, ID>, where MxObject is value and ID is a key.
    */
    template<typename T, typename Key = MxString>
    class ResourceStorage
    {
        using BaseStorage = MxHashMap<Key, UniqueRef<T>>;

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
            Logger::Instance().Debug("MxEngine::ResourceStorage", "deleting all associated resources: " + (MxString)typeid(T).name());
            this->storage.clear();
        }
    };
}