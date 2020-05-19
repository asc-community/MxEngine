#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <memory>
#include <iomanip>

#include "Core/Macro/Macro.h"

namespace MxEngine
{
    /*
    RandomAllocator class by #Momo
    accepts chunk of memory and its size, do NOT allocate or free memory by itself
    allocates objects of any type, each can be freed in any order
    object constructor is called automatically, object destructor is called on Free()
    */
    class RandomAllocator
    {
    public:
        using DataPointer = uint8_t*;
    private:
        /*!
        aligns address upwards or lefts the same (if align = 4, 4->4, 7->8, 9->12 etc)
        \param address address to align
        \param align required alignment
        \returns aligned address
        */
        uintptr_t AlignAddress(uintptr_t address, size_t align)
        {
            const size_t mask = align - 1;
            MX_ASSERT((align & mask) == 0); // check for the power of 2
            return (address + align - 1) & ~mask;
        }

        /*!
        aligns address upwards (if align = 4, 4->8, 7->8, 9->12 etc)
        \param address address to align
        \param align required alignment
        \returns aligned address
        */
        uintptr_t AlignAddressWithPadding(uintptr_t address, size_t align)
        {
            const size_t mask = align - 1;
            MX_ASSERT((align & mask) == 0); // check for the power of 2
            return (address + align) & ~mask;
        }

        /*!
        aligns pointer upwards (if align = 4, 4->8, 7->8, 9->12 etc)
        \param pointer address to align
        \param align required alignment
        \returns aligned address
        */
        uint8_t* AlignPointerWithPadding(uint8_t* ptr, size_t align)
        {
            return reinterpret_cast<uint8_t*>(AlignAddressWithPadding((uintptr_t)ptr, align));
        }

        /*!
        aligns pointer upwards or lefts the same (if align = 4, 4->4, 7->8, 9->12 etc)
        \param pointer address to align
        \param align required alignment
        \returns aligned address
        */
        uint8_t* AlignPointer(uint8_t* ptr, size_t align)
        {
            return reinterpret_cast<uint8_t*>(AlignAddress((uintptr_t)ptr, align));
        }

        /*!
        Header of each list node. Defines only next pointer, making list forward-only
        */
        struct Header
        {
            /*!
            pointer to next list node. Stores node state in last bit
            */
            uintptr_t next;

            /*!
            getter for next list node, ignoress state of current node
            \returns pointer to next list node
            */
            Header* GetNext()
            {
                return (Header*)(next & ~1);
            }

            /*!
            getter for block size associated with current node
            \returns size of block
            */
            size_t GetSize()
            {
                return (uint8_t*)GetNext() - (uint8_t*)this - sizeof(Header);
            }

            /*!
            getter for state of current node (allocated or not)
            \return true if free, false either
            */
            bool IsFree()
            {
                return !(next & 1);
            }

            /*!
            switches node state to free (deallocated) state
            */
            void MakeFree()
            {
                next &= ~1;
            }

            /*!
            switches node state to busy (allocated) state
            */
            void MakeBusy()
            {
                next |= 1;
            }

            /*!
            getter for list node data block
            \returns pointer to allocated block
            */
            uint8_t* GetData()
            {
                return (uint8_t*)this + sizeof(Header);
            }
        };

        /*!
        pointer to fist list node (and begin of memory block)
        */
        Header* first = nullptr;
        /*!
        pointer to the end of list (end of memory block)
        */
        Header* last = nullptr;

        /*!
        splits node into two if it has enough size (more than sizeof(Header) for block),
        header becomes the first block, pointing to the second, the second points to initial value of header->next
        \param header note to split into two
        \param bytes minimal required bytes in block of header node
        \param align required align for node block (clamped to be at least sizeof(pointer)
        */
        void Split(Header* header, size_t bytes, size_t align)
        {
            if (header->GetSize() < bytes + 2 * sizeof(Header))
                return; // not enough space to create new block

            align = (align < sizeof(uintptr_t)) ? sizeof(uintptr_t) : align;

            Header* next = (Header*)AlignPointer(header->GetData() + bytes, align);
            next->next = (uintptr_t)header->GetNext();
            next->MakeFree();
            header->next = (uintptr_t)next;
        }

        /*!
        collapses header node with its next node if it exists and is free
        \param header note to collapse with its next
        \returns true if collapse happened, false either
        */
        bool Collapse(Header* header)
        {
            Header* next = header->GetNext();
            if (next != last && next->IsFree())
            {
                header->next = (uintptr_t)next->GetNext();
                #if !defined(MXENGINE_DEBUG) // remove pointer entry in block to make allocator easier to debug
                next->next = 0;
                #endif
                return true;
            }
            return false;
        }

    public:
        /*!
        creates empty random allocator (first node == last node == nullptr)
        */
        RandomAllocator() {}

        /*!
        assigns new memory chunk to random allocator
        \param data begin of memory chunk
        \param bytes size in bytes of memory chunk
        */
        void Init(DataPointer data, size_t bytes)
        {
            MX_ASSERT(bytes > sizeof(Header));
            first = (Header*)data;
            last = (Header*)(data + bytes);

            first->next = (uintptr_t)last;
            first->MakeFree();
        }

        /*!
        constructs random allocator, assigning it memory chunk
        \param data begin of memory chunk
        \param bytes size in bytes of memory chunk
        */
        RandomAllocator(DataPointer data, size_t bytes)
        {
            this->Init(data, bytes);
        }

        /*!
        memory chunk getter
        \returns pointer to begin of memory chunk
        */
        DataPointer GetBase()
        {
            return (DataPointer)first;
        }

        /*!
        returns pointer to raw allocated memory
        \param bytes minimal requested block size
        \param align minimal alignment of pointer (defaults to 1)
        \returns pointer to memory
        */
        [[nodiscard]] DataPointer RawAlloc(size_t bytes, size_t align = 1)
        {
            MX_ASSERT(this->first != nullptr);

            Header* current = first;
            while (current != last)
            {
                if (current->IsFree())
                {
                    while (this->Collapse(current));
                    uint8_t* data = current->GetData();
                    size_t available = current->GetSize();
                    if (available >= bytes)
                    {
                        this->Split(current, bytes, align);
                        current->MakeBusy();
                        return data;
                    }
                }
                current = current->GetNext();
            }
            return nullptr;
        }

        /*!
        frees raw allocated memory by pointer to its begin
        \param ptr pointer to deallocate
        */
        void RawFree(uint8_t* ptr)
        {
            MX_ASSERT(ptr > (uint8_t*)first && ptr < (uint8_t*)last);
            Header* current = (Header*)(ptr - sizeof(Header));
            while (this->Collapse(current));
            current->MakeFree();
        }

        /*!
        constructs object of type T in memory and returns pointer to it
        \param args arguments for object construction
        \returns pointer to unmanaged object
        */
        template<typename T, typename... Args>
        [[nodiscard]] T* Alloc(Args&&... args)
        {
            DataPointer ptr = RawAlloc(sizeof(T), alignof(T));
            return new (ptr) T(std::forward<Args>(args)...);
        }

        /*!
        constructs object of type T
        \param args arguments for object construction
        \returns smart pointer to object with custom deleter
        */
        template<typename T, typename... Args>
        [[nodiscard]] auto StackAlloc(Args&&... args)
        {
            auto deleter = [this](T* ptr) { this->Free(ptr); };
            using SmartPtr = std::unique_ptr<T, decltype(deleter)>;
            return SmartPtr(this->Alloc<T>(std::forward<Args>(args)...), std::move(deleter));
        }

        /*!
        destroyes object of type T, calling its destructor
        \param value object to destroy
        */
        template<typename T>
        void Free(T* ptr)
        {
            ptr->~T();
            RawFree(reinterpret_cast<uint8_t*>(ptr));
        }

        /*!
        prints whole memory chunk byte by byte in hex format into stream provided
        \param out stream to output to
        */
        void Dump(std::ostream& out) const
        {
            uint8_t* base = (uint8_t*)first;
            size_t size = (uint8_t*)last - base;
            for (size_t i = 0; i < size; i++)
            {
                if (i % sizeof(uintptr_t) == 0) out << ' ';
                out << std::setfill('0') << std::setw(2);
                out << std::hex << (int)base[i];
            }
            out << std::dec << "\n --- dumped " << size << " bytes --- \n";
        }
    };
}