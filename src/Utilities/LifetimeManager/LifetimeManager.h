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

#include <string>
#include <unordered_map>
#include <vector>

#include "Utilities/Logger/Logger.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
    template<
        typename ValueType, 
        typename Id = std::string, 
        typename ValueStorage = std::unordered_map<Id, ValueType>,
        typename IdStorage = std::vector<Id>
        >
    class LifetimeManager
    {
        ValueStorage storage;
        ValueStorage toAdd;
        IdStorage toRemove;

        template<typename T, typename IdT, typename ValueT>
        auto AddImpl(T& container, IdT&& key, ValueT&& value, int) ->
            decltype(std::declval<T>().emplace_back(std::declval<IdT>(), std::declval<ValueT>()), void())
        {
            container.emplace_back(std::forward<IdT>(key), std::forward<ValueT>(value));
        }

        template<typename T, typename IdT, typename ValueT>
        auto AddImpl(T& container, IdT&& key, ValueT&& value, long) ->
            decltype(std::declval<T>().emplace(std::declval<IdT>(), std::declval<ValueT>()), void())
        {
            container.emplace(std::forward<IdT>(key), std::forward<ValueT>(value));
        }

        template<typename T, typename IdT>
        auto ExistsImpl(T& container, IdT&& key, int) ->
            decltype(std::declval<T>().find(key) != std::declval<T>().end(), bool())
        {
            return container.find(std::forward<IdT>(key)) != container.end();
        }

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

        template<typename T>
        auto FreeRemoveQueueImpl(T& container, int) ->
            decltype(std::declval<T>().erase(std::declval<Id>()), void())
        {
            for (const auto& id : this->toRemove)
            {
                Logger::Instance().Debug("MxEngine::LifetimeManager", Format(FMT_STRING("deleting object: {0}"), id));
                this->storage.erase(id);
            }
            this->toRemove.clear();
        }

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

        template<typename IdT, typename ValueT>
        void Add(IdT&& key, ValueT&& value)
        {
            this->AddImpl(this->toAdd, std::forward<IdT>(key), std::forward<ValueT>(value), 0);
        }

        ValueStorage& Get()
        {
            return this->storage;
        }

        const ValueStorage& Get() const
        {
            return this->storage;
        }

        template<typename IdT>
        void Remove(IdT&& key)
        {
            this->toRemove.emplace_back(std::forward<IdT>(key));
        }

        template<typename IdT>
        bool Exists(IdT&& key)
        {
            this->Update();
            return this->ExistsImpl(this->storage, std::forward<IdT>(key), 0);
        }

        void Update()
        {
            this->FreeRemoveQueueImpl(this->storage, 0);
            this->FreeAddQueueImpl(this->storage, 0);
        }
    };
}