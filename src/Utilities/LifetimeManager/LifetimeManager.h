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

#include "Utilities/STL/MxHashMap.h"
#include "Utilities/STL/MxVector.h"
#include "Utilities/STL/MxString.h"

#include "Utilities/Logger/Logger.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
    /*!
    lifetime manager is a generic class which stores elements using their unique ids.
    It supports lazy invokation (i.e. deleting K elements event from vector with N elements will still be O(N), not (K*N))
    Note that before getting element it should be checked for Existance using Exist() or call Update() to invalidate cache.
    For performance reasons functions like GetElements does not invalidate cache, so be careful with them and do not suppose recently added object is already in list
    In general, you want to write something like LifetimeManager<GameObject, UUID> to create manager, but you also can specify storage and cache containers as well
    */
    template<
        typename ValueType, 
        typename Id = MxString, 
        typename ValueStorage = MxHashMap<Id, ValueType>,
        typename IdStorage = MxVector<Id>
        >
    class LifetimeManager
    {
        /*!
        all objects are stored in this container
        */
        ValueStorage storage;
        /*!
        recently added objects are cached in this container before moving to storage
        */
        ValueStorage toAdd;
        /*!
        recently removed object unique ids are stored in this container before removal from storage
        */
        IdStorage toRemove;

        /*!
        specialization for vector/list. Adds new object to the back with O(1) complexity
        \param container vector/list-based container to add to
        \param key unique object identifier
        \param value object itself
        */
        template<typename T, typename IdT, typename ValueT>
        auto AddImpl(T& container, IdT&& key, ValueT&& value, int) ->
            decltype(std::declval<T>().emplace_back(std::declval<IdT>(), std::declval<ValueT>()), void())
        {
            container.emplace_back(std::forward<IdT>(key), std::forward<ValueT>(value));
        }

        /*!
        specialization for map/hash-map. Adds new object to the container with best possible complexity
        \param container tree/hashtable-based container to add to
        \param key unique object identifier
        \param value object itself
        */
        template<typename T, typename IdT, typename ValueT>
        auto AddImpl(T& container, IdT&& key, ValueT&& value, long) ->
            decltype(std::declval<T>().emplace(std::declval<IdT>(), std::declval<ValueT>()), void())
        {
            container.emplace(std::forward<IdT>(key), std::forward<ValueT>(value));
        }

        /*!
        specialization for map/hash-map. searches for object with passed id with best possible complexity
        \param container tree/hashtable-based container to search in
        \param key unique object identifier to search for
        \returns true if object with id exists, false either
        */
        template<typename T, typename IdT>
        auto ExistsImpl(T& container, IdT&& key, int) ->
            decltype(std::declval<T>().find(key) != std::declval<T>().end(), bool())
        {
            return container.find(std::forward<IdT>(key)) != container.end();
        }

        /*!
        specialization for vector/list. searches for object with passed id with O(n) complexity
        \param container vector/list-based container to search in
        \param key unique object identifier to search for
        \returns true if object with id exists, false either
        */
        template<typename T, typename IdT>
        auto ExistsImpl(T& container, IdT&& key, long) ->
            decltype(bool())
        {
            auto it = std::find_if(container.begin(), container.end(),
                [&key](const auto& p)
                {
                    return p.first == key;
                });
            return it != container.end();
        }

        /*!
        invalidating remove cache, deleting requested objects. Specialization for map/hastable, performs removal with best possible complexity
        \param container container to erase from
        \returns void
        */
        template<typename T>
        auto FreeRemoveQueueImpl(T& container, int) ->
            decltype(std::declval<T>().erase(std::declval<Id>()), void())
        {
            for (const auto& id : this->toRemove)
            {
                Logger::Instance().Debug("MxEngine::LifetimeManager", MxFormat("deleting object: {0}", id));
                this->storage.erase(id);
            }
            this->toRemove.clear();
        }

        /*!
        invalidating remove cache, deleting requested objects. Specialization for vector/list, performs removal with O(N*K) complexity
        \param container container to erase from
        \returns void
        */
        template<typename T>
        auto FreeRemoveQueueImpl(T& container, long) ->
            decltype(void())
        {
            for (const auto& id : this->toRemove)
            {
                auto it = std::find_if(this->storage.begin(), this->storage.end(),
                    [&id](const auto& p)
                    {
                        return p.first == id;
                    });
                if (it != this->storage.end())
                {
                    Logger::Instance().Debug("MxEngine::LifetimeManager", Format(FMT_STRING("deleting object: {0}"), id));
                    this->storage.erase(it);
                }
            }
            this->toRemove.clear();
        }

        /*!
        invalidating add cache, adding requested objects. Specialization for vector/list, performs addition with O(K) complexity
        \param container container to add to
        \returns void
        */
        template<typename T>
        auto FreeAddQueueImpl(T& container, int) ->
            decltype(std::declval<T>().insert(
                std::declval<T>().end(), 
                std::make_move_iterator(std::declval<ValueStorage>().begin()),
                std::make_move_iterator(std::declval<ValueStorage>().end())
            ), void())
        {
            if (!this->toAdd.empty())
            {
                this->storage.insert(
                    this->storage.end(),
                    std::make_move_iterator(this->toAdd.begin()),
                    std::make_move_iterator(this->toAdd.end())
                );
                this->toAdd.clear();
            }
        }

        /*!
        invalidating add cache, adding requested objects. Specialization for map/hashtable, performs addition with best possible complexity
        \param container container to add to
        \returns void
        */
        template<typename T>
        auto FreeAddQueueImpl(T& container, long) ->
            decltype(std::declval<T>().insert(
                std::make_move_iterator(std::declval<ValueStorage>().begin()),
                std::make_move_iterator(std::declval<ValueStorage>().end())
            ), void())
        {
            if (!this->toAdd.empty())
            {
                this->storage.insert(
                    std::make_move_iterator(this->toAdd.begin()),
                    std::make_move_iterator(this->toAdd.end())
                );
                this->toAdd.clear();
            }
        }

    public:
        using Value = ValueType;
        using Key = Id;
        using Storage = ValueStorage;

        /*!
        adds new object to the storage. If object with same id already present, it will be replaced. 
        Note that all additions are stored in cache until Update() method is being called
        \param key unique object identifier
        \param value object itself
        */
        template<typename IdT, typename ValueT>
        void Add(IdT&& key, ValueT&& value)
        {
            this->AddImpl(this->toAdd, std::forward<IdT>(key), std::forward<ValueT>(value), 0);
        }

        /*!
        get inner storage. This method does not force cache invalidation. Probably you want to call Update() before it
        \returns inner storage of object manager
        */
        ValueStorage& GetElements()
        {
            return this->storage;
        }

        /*!
        get inner storage. This method does not force cache invalidation. Probably you want to call Update() before it
        \returns inner storage of object manager
        */
        const ValueStorage& GetElements() const
        {
            return this->storage;
        }

        /*!
        removes object from the storage by id. If object with that id does not exists, no action is performed
        Note that all removals are stored in cache until Update() method is being called
        \param key unique object identifier to remove
        */
        template<typename IdT>
        void Remove(IdT&& key)
        {
            this->toRemove.emplace_back(std::forward<IdT>(key));
        }

        /*!
        checks if object with id exists in storage. This method DOES force cache invalidation (see Update())
        \param id to search for
        \returns true if object with id is present, false either
        */
        template<typename IdT>
        bool Exists(IdT&& key)
        {
            this->Update();
            return this->ExistsImpl(this->storage, std::forward<IdT>(key), 0);
        }

        /*!
        invalidates caches, resulting in new object addition and deleting of requested once
        if object with same ID is added and deleted before update, firstly manager will try to delete id from storage, then add new object
        */
        void Update()
        {
            this->FreeRemoveQueueImpl(this->storage, 0);
            this->FreeAddQueueImpl(this->storage, 0);
        }
    };
}