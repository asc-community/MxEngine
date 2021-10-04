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
#include "Utilities/Logging/Logger.h"
#include "Core/Rendering/RenderGraph/SubmissionQueue.h"

namespace MxEngine
{
    using namespace VulkanAbstractionLayer;

    struct BufferAllocatorImpl
    {
        Allocators::FreeListAllocator AllocatorVBO;
        Allocators::FreeListAllocator AllocatorIBO;
        Allocators::FreeListAllocator AllocatorInstanceVBO;
        Allocators::FreeListAllocator AllocatorSSBO;
        BufferHandle VBO;
        BufferHandle IBO;
        BufferHandle InstanceVBO;
        BufferHandle SSBO;
    };

    void BufferAllocator::Init()
    {
        impl = new BufferAllocatorImpl();
    }

    void BufferAllocator::Destroy()
    {
        SubmissionQueue::EndQueue();
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

    BufferUsage::Value VertexBufferUsage = BufferUsage::TRANSFER_DESTINATION | BufferUsage::TRANSFER_SOURCE | BufferUsage::VERTEX_BUFFER;
    BufferUsage::Value IndexBufferUsage = BufferUsage::TRANSFER_DESTINATION | BufferUsage::TRANSFER_SOURCE | BufferUsage::INDEX_BUFFER;
    BufferUsage::Value StorageBufferUsage = BufferUsage::TRANSFER_DESTINATION | BufferUsage::TRANSFER_SOURCE | BufferUsage::STORAGE_BUFFER;

    void BufferAllocator::AllocateBuffers()
    {
        SubmissionQueue::StartQueue();

        constexpr size_t InitialBufferSize = 16 * 1024;

        impl->AllocatorVBO.Init(InitialBufferSize, [](size_t newSize)
        {
            auto oldVBO = impl->VBO;
            impl->VBO = Factory<Buffer>::Create(newSize, VertexBufferUsage, MemoryUsage::GPU_ONLY);
            if (oldVBO.IsValid())
            {
                auto& commandBuffer = SubmissionQueue::GetCommandBuffer();
                commandBuffer.CopyBuffer(BufferInfo{ *oldVBO, 0 }, BufferInfo{ *impl->VBO, 0 }, oldVBO->GetByteSize());
                SubmissionQueue::FlushQueue();
            }
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated vertex buffer storage to new memory with size: " + ToMxString(newSize));
        });
        impl->AllocatorIBO.Init(InitialBufferSize, [](size_t newSize)
        {
            auto oldIBO = impl->IBO;
            impl->IBO = Factory<Buffer>::Create(newSize, IndexBufferUsage, MemoryUsage::GPU_ONLY);
            if (oldIBO.IsValid())
            {
                auto& commandBuffer = SubmissionQueue::GetCommandBuffer();
                commandBuffer.CopyBuffer(BufferInfo{ *oldIBO, 0 }, BufferInfo{ *impl->IBO, 0 }, oldIBO->GetByteSize());
                SubmissionQueue::FlushQueue();
            }
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated index buffer storage to new memory with size: " + ToMxString(newSize));
        });
        impl->AllocatorInstanceVBO.Init(InitialBufferSize, [](size_t newSize)
        {
            auto oldInstanceVBO = impl->InstanceVBO;
            impl->InstanceVBO = Factory<Buffer>::Create(newSize, VertexBufferUsage, MemoryUsage::GPU_ONLY);
            if (oldInstanceVBO.IsValid())
            {
                auto& commandBuffer = SubmissionQueue::GetCommandBuffer();
                commandBuffer.CopyBuffer(BufferInfo{ *oldInstanceVBO, 0 }, BufferInfo{ *impl->InstanceVBO, 0 }, oldInstanceVBO->GetByteSize());
                SubmissionQueue::FlushQueue();
            }
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated instance vertex buffer storage to new memory with size: " + ToMxString(newSize));
        });
        impl->AllocatorSSBO.Init(InitialBufferSize, [](size_t newSize)
        {
            auto oldSSBO = impl->SSBO;
            impl->SSBO = Factory<Buffer>::Create(newSize, StorageBufferUsage, MemoryUsage::GPU_ONLY);
            if (oldSSBO.IsValid())
            {
                auto& commandBuffer = SubmissionQueue::GetCommandBuffer();
                commandBuffer.CopyBuffer(BufferInfo{ *oldSSBO, 0 }, BufferInfo{ *impl->SSBO, 0 }, oldSSBO->GetByteSize());
                SubmissionQueue::FlushQueue();
            }
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated shader storage buffer storage to new memory with size: " + ToMxString(newSize));
        });
        
        struct
        {
            Matrix4x4 field1{ 1.0f };
            Matrix3x3 field2{ 1.0f };
            Vector3 field3{ 1.0f };
        } DefaultInstance;

        auto offset = (uint32_t)impl->AllocatorInstanceVBO.Allocate(sizeof(DefaultInstance));
        SubmissionQueue::CopyToBuffer(&DefaultInstance, *impl->InstanceVBO, offset);
    }

    BufferHandle BufferAllocator::GetVBO()
    {
        return impl->VBO;
    }
    
    BufferHandle BufferAllocator::GetIBO()
    {
        return impl->IBO;
    }
    
    BufferHandle BufferAllocator::GetInstanceVBO()
    {
        return impl->InstanceVBO;
    }
    
    BufferHandle BufferAllocator::GetSSBO()
    {
        return impl->SSBO;
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

    BufferAllocation BufferAllocator::AllocateInInstanceVBO(size_t sizeInInstances)
    {
        size_t offset = impl->AllocatorInstanceVBO.Allocate(sizeInInstances);
        return BufferAllocation{ offset, sizeInInstances };
    }

    BufferAllocation BufferAllocator::AllocateInSSBO(size_t sizeInBytes)
    {
        size_t offset = impl->AllocatorSSBO.Allocate(sizeInBytes);
        return BufferAllocation{ offset, sizeInBytes };
    }

    void BufferAllocator::DeallocateInVBO(BufferAllocation allocation)
    {
        impl->AllocatorVBO.Deallocate(allocation.Offset);
    }

    void BufferAllocator::DeallocateInIBO(BufferAllocation allocation)
    {
        impl->AllocatorIBO.Deallocate(allocation.Offset);
    }

    void BufferAllocator::DeallocateInInstanceVBO(BufferAllocation allocation)
    {
        impl->AllocatorInstanceVBO.Deallocate(allocation.Offset);
    }

    void BufferAllocator::DeallocateInSSBO(BufferAllocation allocation)
    {
        impl->AllocatorSSBO.Deallocate(allocation.Offset);
    }
}