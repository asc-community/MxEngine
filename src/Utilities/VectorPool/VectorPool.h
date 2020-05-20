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

#include "Utilities/STL/MxVector.h"
#include "Utilities/Memory/PoolAllocator.h"

namespace MxEngine
{
    /*!
    VectorPool is an object pool class which is used for fast allocations/deallocations of objects of type T
    objects are accessed by index in array and references should not be stored (as any allocation can potentially invalidate them)
    to check if object is allocated before access, use IsAllocated(index). To allocate use Allocate(args), to deallocate - Deallocate(index)
    */
    template<typename T, template<typename> typename Container = MxVector>
    class VectorPool
    {
        using Allocator = PoolAllocator<T>;
        using Block = typename Allocator::Block;

        /*!
        storage for allocator memory. Unluckly, not debuggable
        */
        Container<uint8_t> memoryStorage;
        /*!
        pool allocator, used with memoryStorage to allocate objects
        */
        Allocator allocator;
        /*!
        number of constructed objects
        */
        size_t allocated = 0;

        Block* GetBlockByIndex(size_t index)
        {
            size_t byteIndex = index * sizeof(Block);
            MX_ASSERT(byteIndex < memoryStorage.size());
            return (Block*)(memoryStorage.data() + byteIndex);
        }

        const Block* GetBlockByIndex(size_t index) const
        {
            size_t byteIndex = index * sizeof(Block);
            MX_ASSERT(byteIndex < memoryStorage.size());
            return (Block*)(memoryStorage.data() + byteIndex);
        }
    public:
        /*!
        constructs default vector pool with zero capacity (no memory request to inner container)
        */
        VectorPool() = default;

        /*!
        constructs vector pool with count elements as capacity
        \count number of preallocated elements (not constructed)
        */
        VectorPool(size_t count)
        {
            this->Resize(count);
        }

        /*!
        increases container size. If new count is less or equal than current, request is ignored
        \param new number of preallocated elements in container (not constructed)
        */
        void Resize(size_t count)
        {
            if (count <= this->Capacity()) return;

            Container<uint8_t> newMemory(count * sizeof(Block));
            allocator.Transfer(newMemory.data(), newMemory.size());
            newMemory.swap(memoryStorage);
        }

        /*!
        gets how many elements are in use (constructed)
        \returns count of currently allocated elements
        */
        size_t Size() const
        {
            return this->allocated;
        }

        /*!
        gets total number of elements in the container
        \returns how many elements can potentially be stored in vector pool
        */
        size_t Capacity() const
        {
            return memoryStorage.size() / sizeof(Block);
        }

        /*!
        reference getter for element. Element must be contructed before call
        \returns reference to element in vector pool
        */
        T& operator[] (size_t index)
        {
            return GetBlockByIndex(index)->data;
        }

        /*!
        const reference getter for element. Element must be contructed before call
        \returns const reference to element in vector pool
        */
        const T& operator[] (size_t index) const
        {
            return GetBlockByIndex(index)->data;
        }

        /*!
        clears container. All constructed elements are destroyed
        */
        void Clear()
        {
            allocator.~PoolAllocator();
            memoryStorage.clear();
        }

        /*!
        checks if element is constructed
        \param index index of element in vector pool
        \returns true if element is constructed, false either
        */
        bool IsAllocated(size_t index)
        {
            return !GetBlockByIndex(index)->IsFree();
        }

        /*!
        destroys element in vector pool
        \param index index of element to destroy
        */
        void Deallocate(size_t index)
        {
            if (IsAllocated(index))
            {
                T& ptr = GetBlockByIndex(index)->data;
                allocator.Free(&ptr);
                this->allocated--;
            }
        }

        /*!
        constructs element in vector pool. If it has not enough space - Resize(bigger size) is called
        \param args arguments for element constructor
        \returns index of element in vector pool
        */
        template<typename... Args>
        size_t Allocate(Args&&... args)
        {
            if (this->allocated == this->Capacity())
            {
                size_t newSize = (this->allocated + 1) * 3 / 2;
                this->Resize(newSize);
            }

            T* obj = allocator.Alloc(std::forward<Args>(args)...);
            this->allocated++;
            return this->IndexOf(*obj);
        }

        /*!
        retrieves index of element in vector pool by reference
        \param obj element of vector pool
        \returns index of element in vector pool
        \warning behaviour is undefined if Allocate() or Resize() were called between reference construction and IndexOf() call
        */
        size_t IndexOf(const T& obj)
        {
            const uint8_t* ptr = (uint8_t*)&obj - offsetof(Block, data);
            MX_ASSERT(memoryStorage.data() <= ptr && ptr < memoryStorage.data() + memoryStorage.size());
            return (Block*)ptr - (Block*)memoryStorage.data();
        }
    };
}