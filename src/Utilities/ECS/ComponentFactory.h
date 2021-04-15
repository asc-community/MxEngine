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
#include "Utilities/String/String.h"
#include "Utilities/Factory/Factory.h"
#include "Utilities/ECS/ComponentView.h"

namespace MxEngine
{
    class ComponentFactory
    {
        static constexpr size_t VectorPoolSize = sizeof(VectorPool<char>);
    public:
        using PoolMap = MxHashMap<StringId, std::aligned_storage_t<VectorPoolSize>>;
    private:
        inline static PoolMap* pools = nullptr;
    public:
        template<typename T>
        static auto& GetPool()
        {
            if (pools->find(T::ComponentId) == pools->end())
            {
                 (void)new(&(*pools)[T::ComponentId]) VectorPool<ManagedResource<T>>();
            }
            auto pool = std::launder(reinterpret_cast<VectorPool<ManagedResource<T>>*>(&(*pools)[T::ComponentId]));
            return *pool;
        }

        template<typename T>
        static ComponentView<T> GetView()
        {
            return ComponentView{ GetPool<T>() };
        }

        template<typename T, typename... Args>
        static auto CreateComponent(Args&&... args)
        {
            UUID uuid = UUIDGenerator::Get();
            auto& pool = GetPool<T>();
            size_t index = pool.Allocate(uuid, std::forward<Args>(args)...);
            return Resource<T, ComponentFactory>(uuid, index);
        }

        template<typename T>
        static void Destroy(Resource<T, ComponentFactory>& resource)
        {
            GetPool<T>().Deallocate(resource.GetHandle());
        }

        static void Init()
        {
            pools = new PoolMap(); // static data, so dont care about freeing
        }

        static PoolMap* GetImpl()
        {
            return pools;
        }

        static void Clone(PoolMap* other)
        {
            pools = other;
        }
    };
}