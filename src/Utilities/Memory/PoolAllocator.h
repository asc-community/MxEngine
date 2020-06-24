#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <memory>

#include "Core/Macro/Macro.h"

namespace MxEngine
{
    /*
    PoolAllocator class by #Momo
    accepts chunk of memory and its size, do NOT allocate or free memory by itself
    allocates objects of type T, each can be freed in any order
    object constructor is called automatically, object destructor is called on Free() or allocator destruction
    */
    template<typename T>
    class PoolAllocator
    {
    public:
        /*!
        block inner structure. Just holds object and offset in memory to next block
        */
        struct Block
        {
            /*!
            object data itself
            */
            T data;
            /*!
            offset in memory of next block (counted in blocks). Stores
            */
            size_t next;

            static constexpr size_t LastBit = 1ULL << (8 * sizeof(size_t) - 1);

            void MarkBusy()
            {
                next |= LastBit;
            }

            bool IsFree()
            {
                return !(next & LastBit);
            }
        };

    private:
        /*!
        begin of memory chunk in use
        */
        Block* storage = nullptr;
        /*!
        first block which is free and can be returned by Alloc
        */
        size_t free = InvalidOffset;
        /*!
        total objects fitted inside Pool allocator
        */
        size_t count = 0;

        /*!
        such value of size_t is too much for allocator, so we use it as invalid offset when all blocks are allocated
        */
        constexpr static size_t InvalidOffset = std::numeric_limits<size_t>::max() - Block::LastBit;
    public:
        using DataPointer = uint8_t*;

        /*!
        creates empty Pool allocator (nullptr chunk pointer, count = 0)
        */
        PoolAllocator() {};

        /*!
        constructs Pool allocator, assigning it memory chunk
        \param data begin of memory chunk
        \param bytes size in bytes of memory chunk
        */
        PoolAllocator(DataPointer data, size_t bytes)
        {
            this->Init(data, bytes);
        }

        /*!
        assigns new memory chunk to Pool allocator
        \param data begin of memory chunk
        \param bytes size in bytes of memory chunk
        */
        void Init(DataPointer data, size_t bytes)
        {
            MX_ASSERT(data != nullptr);
            MX_ASSERT(bytes >= sizeof(Block));

            this->storage = reinterpret_cast<Block*>(data);
            this->free = 0;
            this->count = bytes / sizeof(Block);

            Block* last = this->storage + count - 1;
            size_t offset = 1;
            for (Block* begin = this->storage; begin != last; begin++, offset++)
            {
                begin->next = offset;
            }
            last->next = InvalidOffset;
        }

        /*!
        transfers blocks from one memory chunk to another
        \param newData begin of new memory chunk
        \param newBytes size of new memory chunk (must be not less than old size)
        \warning during transfer all pointers to allocated objects are invalidated
        */
        void Transfer(DataPointer newData, size_t newBytes)
        {
            if (this->storage == nullptr)
            {
                this->Init(newData, newBytes);
                return;
            }

            size_t newCount = newBytes / sizeof(Block); // calc new number of blocks
            MX_ASSERT(newData != nullptr);
            MX_ASSERT(this->count <= newCount);
            
            std::memcpy(newData, this->storage, this->count * sizeof(Block)); // copy old blocks to new memory chunk

            // some objects in MxEngine tend to invoke clean up on destruction, even if they are moved.
            // to avoid this we put restriction on pool allocator objects to not to refer to themselves. Its better to use
            // macros like offsetof() than to store raw pointers to data which can be easily invalidated by rellocator

            // for (size_t i = 0; i < this->count; i++)
            // {
            //     auto& oldBlock = *(this->storage + i);
            //     auto* _ = new((Block*)newData + i) Block(std::move(oldBlock)); //-V799
            //     oldBlock.~Block();
            // }

            this->storage = (Block*)newData;
            Block* last = this->storage + newCount - 1; // calculate last block to chain new ones
            size_t offset = this->count + 1;
            for(Block* begin = this->storage + this->count; begin != last; begin++, offset++)
            {
                begin->next = offset;
            }
            last->next = this->free; // make last of new blocks point to the first free block of old
            this->free = this->count; // chain all new blocks to the free list
            this->count = newCount;
        }

        /*!
        destroys Pool allocator and all objects stored in it
        */
        ~PoolAllocator()
        {
            constexpr size_t marked = InvalidOffset - 1;
            size_t offset = this->free;
            while (offset != InvalidOffset)
            {
                Block* block = this->storage + offset;
                offset = block->next;
                block->next = marked;
            }
            for (Block* block = this->storage; block != this->storage + this->count; block++)
            {
                if (block->next != marked)
                {
                    block->data.~T();
                }
            }
        }

        /*!
        memory chunk getter
        \returns pointer to begin of memory chunk
        */
        DataPointer GetBase()
        {
            return this->storage;
        }

        /*!
        constructs object of type T in memory and returns pointer to it
        \param args arguments for object construction
        \returns pointer to object (object destructor is automatically called in ~PoolAllocator())
        */
        template<typename... Args>
        [[nodiscard]] T* Alloc(Args&&... args)
        {
            MX_ASSERT(this->free != InvalidOffset);
            Block* res = this->storage + this->free;
            T* ptr = &res->data;
            this->free = res->next;
            res->MarkBusy();
            return new (ptr) T(std::forward<Args>(args)...);
        }

        /*!
        destroyes object of type T, calling its destructor
        \param value object to destroy
        */
        void Free(T* object)
        {
            object->~T();
            Block* block = reinterpret_cast<Block*>(object);
            MX_ASSERT(block >= this->storage && block < this->storage + this->count);
            block->next = this->free;
            this->free = static_cast<size_t>(block - this->storage);
        }

        /*!
        constructs object of type T
        \param args arguments for object construction
        \returns smart pointer to object with custom deleter
        */
        template<typename... Args>
        [[nodiscard]] auto StackAlloc(Args&&... args)
        {
            auto deleter = [this](T* ptr) { this->Free(ptr); };
            using SmartPtr = std::unique_ptr<T, decltype(deleter)>;
            return SmartPtr(this->Alloc(std::forward<Args>(args)...), std::move(deleter));
        }

        /*!
        prints whole memory chunk byte by byte in hex format into stream provided
        \param out stream to output to
        */
        void Dump(std::ostream& out) const
        {
            for (size_t i = 0; i < this->count * sizeof(Block); i++)
            {
                out << std::hex << (int)((DataPointer)this->storage)[i];
            }
            out << std::dec << "\n --- dumped " << this->count * sizeof(Block) << " bytes --- \n";
        }
    };
}