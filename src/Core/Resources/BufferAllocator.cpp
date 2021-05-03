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
// and /or other materials provided wfith the distribution.
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

#include "BufferAllocator.h"
#include "FreeListAllocator.h"

namespace MxEngine
{
    struct BufferAllocatorImpl
    {
        Allocators::FreeListAllocator AllocatorVBO;
        Allocators::FreeListAllocator AllocatorIBO;
        VertexBufferHandle VBO;
        IndexBufferHandle IBO;
    };

    void BufferAllocator::Init()
    {
        impl = new BufferAllocatorImpl();
    }

    void BufferAllocator::Destroy()
    {
        delete impl;
    }

    BufferAllocatorImpl* BufferAllocator::GetImpl()
    {
        return impl;
    }

    void BufferAllocator::Clone(BufferAllocatorImpl* other)
    {
        impl = other;
    }

    void BufferAllocator::AllocateBuffers()
    {
        impl->VBO = Factory<VertexBuffer>::Create(nullptr, 0, UsageType::STATIC_DRAW);
        impl->IBO = Factory<IndexBuffer>::Create(nullptr, 0, UsageType::STATIC_DRAW);
        impl->AllocatorVBO.Init(0, [](size_t newSize)
        {
            auto copyVBO = Factory<VertexBuffer>::Create(nullptr, impl->VBO->GetSize(), UsageType::STREAM_COPY);
            copyVBO->LoadFrom(*impl->VBO);
            impl->VBO->Load(nullptr, newSize, UsageType::STATIC_DRAW);
            impl->VBO->LoadFrom(*copyVBO);
        });
        impl->AllocatorIBO.Init(0, [](size_t newSize)
        {
            auto copyIBO = Factory<IndexBuffer>::Create(nullptr, impl->IBO->GetSize(), UsageType::STREAM_COPY);
            copyIBO->LoadFrom(*impl->IBO);
            impl->IBO->Load(nullptr, newSize, UsageType::STATIC_DRAW);
            impl->IBO->LoadFrom(*copyIBO);
        });
    }

    VertexBufferHandle BufferAllocator::GetVBO()
    {
        return impl->VBO;
    }

    IndexBufferHandle BufferAllocator::GetIBO()
    {
        return impl->IBO;
    }

    BufferAllocation BufferAllocator::AllocateInVBO(size_t sizeInFloats)
    {
        size_t offset = impl->AllocatorVBO.Allocate(sizeInFloats);
        return BufferAllocation{ offset, sizeInFloats };
    }

    BufferAllocation BufferAllocator::AllocateInIBO(size_t sizeInIndices)
    {
        size_t offset = impl->AllocatorIBO.Allocate(sizeInIndices);
        return BufferAllocation{ offset, sizeInIndices };
    }

    void BufferAllocator::DeallocateInVBO(BufferAllocation allocation)
    {
        impl->AllocatorVBO.Deallocate(allocation.Offset);
    }

    void BufferAllocator::DeallocateInIBO(BufferAllocation allocation)
    {
        impl->AllocatorIBO.Deallocate(allocation.Offset);
    }

}