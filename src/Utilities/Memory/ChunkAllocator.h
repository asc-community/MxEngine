#pragma once

#include <cstdint>
#include <memory>

namespace MxEngine
{
    /*
    ChunkAllocator class by #Momo
    uses std::malloc to allocate chunks (each 4KB)
    chunks can are freed in any order usind std::free
    */
    class ChunkAllocator
    {
    public:
        /*!
        chunk size, defaults to 4KB
        */
        static constexpr size_t ChunkSize = 4096;

        /*!
        allocates chunks of memory using malloc
        \param chunkCount number of chunk to allocate
        \returns pointer to first chunk
        */
        static uint8_t* RawAlloc(size_t chunkCount)
        {
            return (uint8_t*)std::malloc(chunkCount * ChunkSize);
        }

        /*!
        frees chunk allocated by RawAlloc
        \param chunk pointer to first chunk
        */
        static void Free(uint8_t* chunk)
        {
            std::free(chunk);
        }
    };
}