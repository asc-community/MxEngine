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

#include "Utilities/AbstractFactory/AbstractFactory.h"

namespace MxEngine
{
    struct Component
    {
        static constexpr size_t ResourceSize = sizeof(Resource<char, AbstractFactoryImpl<char>>);
        using Deleter = void (*)(void*);
        std::aligned_storage_t<ResourceSize> resource;
        size_t type;
        Deleter deleter;

        template<typename T>
        Component(size_t type, Resource<T, AbstractFactoryImpl<T>>&& component)
        {
            using ComponentType = Resource<T, AbstractFactoryImpl<T>>;
            static_assert(sizeof(ComponentType) == sizeof(Component::resource), "storage must fit resource size");

            this->type = type;
            this->deleter = [](void* ptr) { AbstractFactoryImpl<T>::Destroy(*(ComponentType*)ptr); };
            auto* replace = new (&resource) ComponentType();
            *replace = std::move(component);
        }
    };

    class ComponentManager
    {
        MxVector<std::aligned_storage_t<sizeof(Component)>> components;
    public:
        template<typename T, typename... Args>
        void AddComponent(Args&&... args)
        {
            if (this->HasComponent<T>()) return;

            using Factory = AbstractFactoryImpl<T>;
            Factory::Init(); // ok if was already inited
            auto component = Factory::CreateT((T*)nullptr, std::forward<Args>(args)...);
            components.emplace_back();
            auto* _ = new (&components.back()) Component(T::ComponentId, std::move(component));
        }

        template<typename T>
        auto GetComponent()
        {
            using ResourceT = Resource<T, AbstractFactoryImpl<T>>;
            for (auto& component : components)
            {
                auto& componentRef = *reinterpret_cast<Component*>(&component);
                if (componentRef.type == T::ComponentId)
                {
                    auto& result = *reinterpret_cast<ResourceT*>(&componentRef.resource);
                    return result;
                }
            }
            return ResourceT{ };
        }

        template<typename T>
        void RemoveComponent()
        {
            using ResourceT = Resource<T, AbstractFactoryImpl<T>>;
            for (auto it = components.begin(); it != components.end(); it++)
            {
                auto& componentRef = *reinterpret_cast<Component*>(&*it);
                if (componentRef.type == T::ComponentId)
                {
                    auto& resource = *reinterpret_cast<ResourceT*>(&componentRef.resource);
                    if (resource.IsValid())
                    {
                        using Factory = AbstractFactoryImpl<T>;
                        Factory::Destroy(resource);
                    }
                    components.erase(it);
                }
            }
        }

        template<typename T>
        bool HasComponent()
        {
            return this->GetComponent<T>().IsValid();
        }

        ~ComponentManager()
        {
            for (auto& component : components)
            {
                auto& componentRef = *reinterpret_cast<Component*>(&component);
                componentRef.deleter((void*)&componentRef.resource);
            }
        }
    };
}