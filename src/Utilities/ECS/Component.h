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

#include "Utilities/ECS/ComponentFactory.h"

namespace MxEngine
{
    class ComponentManager;

    struct Component
    {
        using Deleter = void (*)(void*);

        std::aligned_storage_t<sizeof(CResource<char>)> resource;
        size_t type;
        Deleter deleter;

        template<typename T>
        Component(size_t type, CResource<T>&& component)
        {
            static_assert(sizeof(CResource<T>) == sizeof(Component::resource), "storage must fit resource size");

            this->type = type;
            this->deleter = [](void* ptr) { ComponentFactory::Destroy(*static_cast<CResource<T>*>(ptr)); };
            auto* replace = new (&resource) CResource<T>();
            *replace = std::move(component);
        }
    };

    class ComponentManager
    {
        template<typename T>
        using ComponentList = MxVector<T>;

        ComponentList<std::aligned_storage_t<sizeof(Component)>> components;
    public:
        ComponentManager() = default;
        ComponentManager(const ComponentManager&) = delete;
        ComponentManager(ComponentManager&&) noexcept = default;
        ComponentManager& operator=(const ComponentManager&) = delete;
        ComponentManager& operator=(ComponentManager&&) noexcept = default;

        template<typename T, typename... Args>
        CResource<T> AddComponent(Args&&... args)
        {
            this->RemoveComponent<T>();
            
            auto component = ComponentFactory::CreateComponent<T>(std::forward<Args>(args)...);
            auto& data = components.emplace_back();
            Component* result = new (&data) Component(T::ComponentId, std::move(component));
            return *reinterpret_cast<CResource<T>*>(&result->resource);
        }

        template<typename T>
        CResource<T> GetComponent() const
        {
            for (const auto& component : components)
            {
                const auto& componentRef = *reinterpret_cast<const Component*>(&component);
                if (componentRef.type == T::ComponentId)
                {
                    return *reinterpret_cast<const CResource<T>*>(&componentRef.resource);
                }
            }
            return CResource<T>{ };
        }

        template<typename T>
        void RemoveComponent()
        {
            for (auto it = components.begin(); it != components.end(); it++)
            {
                auto& componentRef = *reinterpret_cast<Component*>(&*it);
                if (componentRef.type == T::ComponentId)
                {
                    auto& resource = *reinterpret_cast<CResource<T>*>(&componentRef.resource);
                    if (resource.IsValid())
                    {
                        ComponentFactory::Destroy(resource);
                    }
                    components.erase(it);
                    return;
                }
            }
        }

        template<typename T>
        bool HasComponent() const
        {
            return this->GetComponent<T>().IsValid();
        }

        void RemoveAllComponents()
        {
            for (auto& component : components)
            {
                auto& componentRef = *reinterpret_cast<Component*>(&component);
                componentRef.deleter(static_cast<void*>(&componentRef.resource));
            }
            components.clear();
        }

        ~ComponentManager()
        {
            this->RemoveAllComponents();
        }
    };

#define MAKE_COMPONENT(class_name)\
        public:\
                using Handle = CResource<class_name>;\
        private: static constexpr MxEngine::StringId ComponentId = STRING_ID(#class_name);\
                void* UserData = (void*)std::numeric_limits<uintptr_t>::max();\
                friend class MxObject;\
                friend class MxEngine::ComponentManager; \
                friend class MxEngine::ComponentFactory
}