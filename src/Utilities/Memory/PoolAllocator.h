#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <memory>

#include "Core/Macro/Macro.h"

/*
PoolAllocator class by #Momo
accepts chunk of memory and its size, do NOT allocate or free memory by itself
allocates objects of type T, each can be freed in any order
object constructor is called automatically, object destructor is called on Free() or allocator destruction
*/
template<typename T>
class PoolAllocator
{
    struct Block
    {
        T data;
        size_t next;
    };

    Block* storage = nullptr;
    size_t free = InvalidOffset;
    size_t count = 0;

    constexpr static size_t InvalidOffset = std::numeric_limits<size_t>::max();
public:
    using DataPointer = uint8_t*;

    PoolAllocator() {};

    PoolAllocator(DataPointer data, size_t bytes)
    {
        this->Init(data, bytes);
    }

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

    template<typename... Args>
    [[nodiscard]] T* Alloc(Args&&... args)
    {
        MX_ASSERT(this->free != InvalidOffset);
        Block* res = this->storage + this->free;
        T* ptr = &res->data;
        this->free = res->next;
        return new (ptr) T(std::forward<Args>(args)...);
    }

    void Free(T* object)
    {
        object->~T();
        Block* block = reinterpret_cast<Block*>(object);
        MX_ASSERT(block >= this->storage && block < this->storage + this->count);
        block->next = this->free;
        this->free = static_cast<size_t>(block - this->storage);
    }

    template<typename... Args>
    [[nodiscard]] auto StackAlloc(Args&&... args)
    {
        auto deleter = [this](T* ptr) { this->Free(ptr); };
        using SmartPtr = std::unique_ptr<T, decltype(deleter)>;
        return SmartPtr(this->Alloc(std::forward<Args>(args)...), std::move(deleter));
    }

    void Dump(std::ostream& out) const
    {
        for (size_t i = 0; i < this->count * sizeof(Block); i++)
        {
            out << std::hex << (int)((DataPointer)this->storage)[i];
        }
        out << std::dec << "\n --- dumped " << this->count * sizeof(Block) << " bytes --- \n";
    }
};