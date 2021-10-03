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

namespace MxEngine
{
    struct BufferAllocatorImpl
    {
        Allocators::FreeListAllocator AllocatorVBO;
        Allocators::FreeListAllocator AllocatorIBO;
        Allocators::FreeListAllocator AllocatorInstanceVBO;
        Allocators::FreeListAllocator AllocatorSSBO;
        // VertexBufferHandle VBO;
        // IndexBufferHandle IBO;
        // VertexBufferHandle InstanceVBO;
        // ShaderStorageBufferHandle SSBO;
        // VertexArrayHandle VAO;
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
        // impl->VBO = Factory<VertexBuffer>::Create(nullptr, 0, UsageType::DYNAMIC_COPY);
        // impl->IBO = Factory<IndexBuffer>::Create(nullptr, 0, UsageType::DYNAMIC_COPY);
        // impl->InstanceVBO = Factory<VertexBuffer>::Create(nullptr, 0, UsageType::DYNAMIC_COPY);
        // impl->SSBO = Factory<ShaderStorageBuffer>::Create((uint8_t*)nullptr, 0, UsageType::DYNAMIC_COPY);
        // impl->VAO = Factory<VertexArray>::Create();
        
        impl->AllocatorVBO.Init(0, [](size_t newSize)
        {
            // auto copyVBO = Factory<VertexBuffer>::Create(nullptr, impl->VBO->GetSize(), UsageType::STREAM_COPY);
            // copyVBO->LoadFrom(*impl->VBO);
            // impl->VBO->Load(nullptr, newSize, UsageType::DYNAMIC_COPY);
            // impl->VBO->LoadFrom(*copyVBO);
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated vertex buffer storage to new memory with size: " + ToMxString(newSize));
        });
        impl->AllocatorIBO.Init(0, [](size_t newSize)
        {
            // auto copyIBO = Factory<IndexBuffer>::Create(nullptr, impl->IBO->GetSize(), UsageType::STREAM_COPY);
            // copyIBO->LoadFrom(*impl->IBO);
            // impl->IBO->Load(nullptr, newSize, UsageType::DYNAMIC_COPY);
            // impl->IBO->LoadFrom(*copyIBO);
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated index buffer storage to new memory with size: " + ToMxString(newSize));
        });
        impl->AllocatorInstanceVBO.Init(0, [](size_t newSize)
        {
            // auto copyInstanceVBO = Factory<VertexBuffer>::Create(nullptr, impl->InstanceVBO->GetSize(), UsageType::STREAM_COPY);
            // copyInstanceVBO->LoadFrom(*impl->InstanceVBO);
            // impl->InstanceVBO->Load(nullptr, newSize, UsageType::DYNAMIC_COPY);
            // impl->InstanceVBO->LoadFrom(*copyInstanceVBO);
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated instance vertex buffer storage to new memory with size: " + ToMxString(newSize));
        });
        impl->AllocatorSSBO.Init(0, [](size_t newSize)
        {
            // auto copySSBO = Factory<ShaderStorageBuffer>::Create((uint8_t*)nullptr, impl->SSBO->GetByteSize(), UsageType::STREAM_COPY);
            // copySSBO->LoadFrom(*impl->SSBO);
            // impl->SSBO->Load<uint8_t>(nullptr, newSize, UsageType::DYNAMIC_COPY);
            // impl->SSBO->LoadFrom(*copySSBO);
            MXLOG_DEBUG("MxEngine::BufferAllocator", "relocated shader storage buffer storage to new memory with size: " + ToMxString(newSize));
        });
        
        // std::array vertexLayout = {
        //     VertexAttribute::Entry<Vector3>(), // position
        //     VertexAttribute::Entry<Vector2>(), // texture uv
        //     VertexAttribute::Entry<Vector3>(), // normal
        //     VertexAttribute::Entry<Vector3>(), // tangent
        //     VertexAttribute::Entry<Vector3>(), // bitangent
        // };
        // std::array instanceLayout = {
        //     VertexAttribute::Entry<Matrix4x4>(), // model
        //     VertexAttribute::Entry<Matrix3x3>(), // normal
        //     VertexAttribute::Entry<Vector3>(),   // color
        // };
        // 
        // impl->VAO->AddVertexLayout(*impl->VBO, vertexLayout, VertexAttributeInputRate::PER_VERTEX);
        // impl->VAO->AddVertexLayout(*impl->InstanceVBO, instanceLayout, VertexAttributeInputRate::PER_INSTANCE);
        // impl->VAO->LinkIndexBuffer(*impl->IBO);
        // 
        // struct
        // {
        //     Matrix4x4 field1{ 1.0f };
        //     Matrix3x3 field2{ 1.0f };
        //     Vector3   field3{ 1.0f };
        // } DefaultInstance;
        // 
        // // assume first allocation is with offset = 0
        // (void)impl->AllocatorInstanceVBO.Allocate(sizeof(DefaultInstance) / sizeof(float));
        // impl->InstanceVBO->BufferSubData((float*)&DefaultInstance, sizeof(DefaultInstance) / sizeof(float));
    }

    // VertexBufferHandle BufferAllocator::GetVBO()
    // {
    //     return impl->VBO;
    // }
    // 
    // IndexBufferHandle BufferAllocator::GetIBO()
    // {
    //     return impl->IBO;
    // }
    // 
    // VertexBufferHandle BufferAllocator::GetInstanceVBO()
    // {
    //     return impl->InstanceVBO;
    // }
    // 
    // VertexArrayHandle BufferAllocator::GetVAO()
    // {
    //     return impl->VAO;
    // }
    // 
    // ShaderStorageBufferHandle BufferAllocator::GetSSBO()
    // {
    //     return impl->SSBO;
    // }

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