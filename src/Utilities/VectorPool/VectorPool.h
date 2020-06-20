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

#include "Utilities/STL/MxVector.h"
#include "Utilities/Memory/PoolAllocator.h"

namespace MxEngine
{
    /*!
    VectorPool is an object Pool class which is used for fast allocations/deallocations of objects of type T
    objects are accessed by index in array and references should not be stored (as any allocation can potentially invalidate them)
    to check if object is allocated before access, use IsAllocated(index). To allocate use Allocate(args), to deallocate - Deallocate(index)
    */
    template<typename T, template<typename> typename Container = MxVector>
    class VectorPool
    {
    public:
        /*!
        iterator for VectorPool class. supports increment, compare and decrement.
        Ignores not allocated objects, allowing user to iterate over all objects in Pool without check for IsAllocated(index)
        */
        class PoolIterator
        {
            /*!
            current index of object in vector Pool
            */
            size_t index = 0;
            /*!
            reference to vector Pool. This means that vector Pool must not be moved/deleted until iterator exists
            */
            VectorPool<T, Container>& poolRef;
        public:
            size_t GetBase() const
            {
                return index;
            }

            VectorPool<T, Container>& GetPoolRef()
            {
                return poolRef;
            }

            /*!
            construtc new iterator of vector Pool
            \param index to the element of vector Pool (0 for begin(), Capacity() for end() methods)
            \param poolRef reference to vector Pool
            */
            PoolIterator(size_t index, VectorPool<T, Container>& ref)
                : index(index), poolRef(ref)
            {
                while (this->index < this->poolRef.Capacity() && !poolRef.IsAllocated(this->index))
                {
                    this->index++; // 0 element may not exists, so we should skip it until find any allocated
                }
            }

            /*!
            increments iterator until reaches end or finds next allocated object
            \returns iterator to the object before increment
            */
            PoolIterator operator++(int)
            {
                PoolIterator copy = *this;
                ++(*this);
                return copy;
            }

            /*!
            increments iterator until reaches end or finds next allocated object
            \return iterator to the object after increment
            */
            PoolIterator operator++()
            {
                do { index++; } while (index < poolRef.Capacity() && !poolRef.IsAllocated(index));
                return *this;
            }

            /*!
            decrements iterator until reaches begin or finds previous allocated object
            \returns iterator to the object before increment
            */
            PoolIterator operator--(int)
            {
                PoolIterator copy = *this;
                --(*this);
                return copy;
            }

            /*!
            decrements iterator until reaches begin or finds previous allocated object
            \return iterator to the object after increment
            */
            PoolIterator operator--()
            {
                do { index--; } while (index < poolRef.Capacity() && !poolRef.IsAllocated(index));
                return *this;
            }

            /*!
            dereferences iterator, accessing vector Pool
            \returns pointer to vector Pool element
            */
            T* operator->()
            {
                return &poolRef[index];
            }

            /*!
            dereferences iterator, accessing vector Pool
            \returns const pointer to vector Pool element
            */
            const T* operator->() const
            {
                return &poolRef[index];
            }

            /*!
            dereferences iterator, accessing vector Pool
            \returns reference to vector Pool element
            */
            T& operator*()
            {
                return poolRef[index];
            }

            /*!
            dereferences iterator, accessing vector Pool
            \returns const reference to vector Pool element
            */
            const T& operator*() const
            {
                return poolRef[index];
            }

            /*!
            compares two iterator for equality, returns false if they point to different vector pools
            \returns true if iterators point to same object, false either
            */
            bool operator==(const PoolIterator& it) const
            {
                return (index == it.index) && (&poolRef == &it.poolRef);
            }

            /*!
            compares wto iterator for inequality, returns true if they point to different vector pools
            \returns false if iterators point to same object, true either
            */
            bool operator!=(const PoolIterator& it) const
            {
                return !(*this == it);
            }
        };

        using Allocator = PoolAllocator<T>;
        using Block = typename Allocator::Block;
    private:
        /*!
        storage for allocator memory. Unluckly, not debuggable
        */
        Container<uint8_t> memoryStorage;
        /*!
        Pool allocator, used with memoryStorage to allocate objects
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
        constructs default vector Pool with zero capacity (no memory request to inner container)
        */
        VectorPool() = default;

        /*!
        constructs vector Pool with count elements as capacity
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
            memoryStorage = std::move(newMemory);
        }

        /*!
        gets how many elements are in use (constructed)
        \returns count of currently allocated elements
        */
        size_t Allocated() const
        {
            return this->allocated;
        }

        /*!
        gets total number of elements in the container
        \returns how many elements can potentially be stored in vector Pool
        */
        size_t Capacity() const
        {
            return memoryStorage.size() / sizeof(Block);
        }

        /*!
        reference getter for element. Element must be contructed before call
        \returns reference to element in vector Pool
        */
        T& operator[] (size_t index)
        {
            return GetBlockByIndex(index)->data;
        }

        /*!
        const reference getter for element. Element must be contructed before call
        \returns const reference to element in vector Pool
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
        \param index index of element in vector Pool
        \returns true if element is constructed, false either
        */
        bool IsAllocated(size_t index)
        {
            return !GetBlockByIndex(index)->IsFree();
        }

        /*!
        destroys element in vector Pool
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

        void Deallocate(const PoolIterator& it)
        {
            this->Deallocate(it.GetBase());
        }

        /*!
        constructs element in vector Pool. If it has not enough space - Resize(bigger size) is called
        \param args arguments for element constructor
        \returns index of element in vector Pool
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
        retrieves index of element in vector Pool by reference
        \param obj element of vector Pool
        \returns index of element in vector Pool
        \warning behaviour is undefined if Allocate() or Resize() were called between reference construction and IndexOf() call
        */
        size_t IndexOf(const T& obj)
        {
            const uint8_t* ptr = (uint8_t*)&obj - offsetof(Block, data);
            MX_ASSERT(memoryStorage.data() <= ptr && ptr < memoryStorage.data() + memoryStorage.size());
            return (Block*)ptr - (Block*)memoryStorage.data();
        }

        /*!
        begin of vector Pool container
        \returns iterator to first allocated element or end iterator
        */
        PoolIterator begin()
        {
            return PoolIterator{ 0, *this };
        }

        /*!
        begin of vector Pool container
        \returns const iterator to first allocated element or const end iterator
        */
        const PoolIterator begin() const
        {
            return PoolIterator{ 0, *this };
        }

        /*!
        end of vector Pool container
        \returns iterator to the end of vector container
        */
        PoolIterator end()
        {
            return PoolIterator{ this->Capacity(), *this };
        }

        /*!
        end of vector Pool container
        \returns const iterator to the end of vector container
        */
        const PoolIterator end() const
        {
            return PoolIterator{ this->Capacity(), *this };
        }
    };
}