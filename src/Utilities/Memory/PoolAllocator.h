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
            offset in memory of next block (counted in blocks)
            */
            size_t next;
        };

        /*!
        begin of memory chunk in use
        */
        Block* storage = nullptr;
        /*!
        first block which is free and can be returned by Alloc
        */
        size_t free = InvalidOffset;
        /*!
        total objects fitted inside pool allocator
        */
        size_t count = 0;

        /*!
        max value of size_t is too much for allocator, so we use it as invalid offset when all blocks are allocated
        */
        constexpr static size_t InvalidOffset = std::numeric_limits<size_t>::max();
    public:
        using DataPointer = uint8_t*;

        /*!
        creates empty pool allocator (nullptr chunk pointer, count = 0)
        */
        PoolAllocator() {};

        /*!
        constructs pool allocator, assigning it memory chunk
        \param data begin of memory chunk
        \param bytes size in bytes of memory chunk
        */
        PoolAllocator(DataPointer data, size_t bytes)
        {
            this->Init(data, bytes);
        }

        /*!
        assigns new memory chunk to pool allocator
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
        destroys pool allocator and all objects stored in it
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