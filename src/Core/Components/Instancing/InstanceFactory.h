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

#include "Core/Components/Instancing/Instance.h"
#include "Core/Resources/Mesh.h"
#include "Utilities/String/String.h"

namespace MxEngine
{
    class InstanceView
    {
    public:
        using Pool = VectorPool<MxObject::Handle>;

        class InstanceIterator
        {
            using It = Pool::PoolIterator;
            It it;

        public:
            InstanceIterator(It it) : it(it) {}

            InstanceIterator operator++(int)
            {
                InstanceIterator copy = *this;
                it++;
                return copy;
            }

            InstanceIterator operator++()
            {
                ++it;
                return *this;
            }

            InstanceIterator operator--(int)
            {
                InstanceIterator copy = *this;
                it--;
                return copy;
            }

            InstanceIterator operator--()
            {
                --it;
                return *this;
            }

            MxObject::Handle& operator->()
            {
                return *it;
            }

            const MxObject::Handle& operator->() const
            {
                return *it;
            }

            MxObject::Handle& operator*()
            {
                return *it;
            }

            const MxObject::Handle& operator*() const
            {
                return *it;
            }

            bool operator==(const InstanceIterator& other) const
            {
                return it == other.it;
            }

            bool operator!=(const InstanceIterator& other) const
            {
                return it != other.it;
            }
        };
    private:
        Pool& ref;

    public:
        explicit InstanceView(Pool& ref) : ref(ref) {}

        auto begin()
        {
            return InstanceIterator{ ref.begin() };
        }
        
        const auto begin() const
        {
            return InstanceIterator{ ref.begin() };
        }
        
        auto end()
        {
            return InstanceIterator{ ref.end() };
        }
        
        const auto end() const
        {
            return InstanceIterator{ ref.end() };
        }
    };

    class InstanceFactory
    {
        MAKE_COMPONENT(InstanceFactory);
    public:
        struct InstanceData
        {
            Matrix4x4 Model{ 1.0f };
            Matrix3x3 Normal{ 1.0f };
            Vector3 Color{ 1.0f };
        };

        constexpr static size_t InstanceDataSize = sizeof(InstanceData) / sizeof(float);
        using InstancePool = VectorPool<MxObject::Handle>;
    private:
        mutable InstancePool pool;
        MxVector<InstanceData> instances;
        MoveOnlyAllocation instanceAllocation;

        void RemoveDanglingHandles();
        void SendInstancesToGPU();
        void ReserveInstanceAllocation(size_t count);
        void UpdateInstanceCache();

        void FreeInstancePool();
        void FreeInstanceAllocation();
    public:
        InstanceFactory() = default;
        ~InstanceFactory();

        bool IsStatic = false;

        const InstancePool& GetInstancePool() const { return this->pool; }
        InstancePool& GetInstancePool() { return this->pool; };
        size_t GetInstanceCount() const { return this->GetInstancePool().Allocated(); }
        size_t GetInstanceBufferSize() const { return this->instanceAllocation.Size; }
        size_t GetInstanceBufferOffset() const { return this->instanceAllocation.Offset; }
        auto GetInstances() const { return InstanceView{ this->pool }; }

        void OnUpdate(float timeDelta);
        MxObject::Handle Instanciate();
        void SubmitInstances();
        void DestroyInstances();
    };
}