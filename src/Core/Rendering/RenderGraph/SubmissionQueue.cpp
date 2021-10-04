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

#include "SubmissionQueue.h"

namespace MxEngine
{
    void SubmissionQueue::StartQueue()
    {
        SubmissionQueue::GetCommandBuffer().Begin();
    }

    void SubmissionQueue::EndQueue()
    {
        auto& commandBuffer = SubmissionQueue::GetCommandBuffer();
        auto& stageBuffer = SubmissionQueue::GetStageBuffer();

        stageBuffer.Flush();
        commandBuffer.End();

        VulkanAbstractionLayer::GetCurrentVulkanContext().SubmitCommandsImmediate(commandBuffer);
    }

    VulkanAbstractionLayer::CommandBuffer& SubmissionQueue::GetCommandBuffer()
    {
        return VulkanAbstractionLayer::GetCurrentVulkanContext().GetImmediateCommandBuffer();
    }

    VulkanAbstractionLayer::StageBuffer& SubmissionQueue::GetStageBuffer()
    {
        return VulkanAbstractionLayer::GetCurrentVulkanContext().GetCurrentStageBuffer();
    }

    void SubmissionQueue::RecordAllocation(size_t byteSize)
    {
        auto& stageBuffer = SubmissionQueue::GetStageBuffer();
        if (stageBuffer.GetCurrentOffset() + byteSize > stageBuffer.GetBuffer().GetByteSize())
        {
            SubmissionQueue::FlushQueue();
        }
    }

    void SubmissionQueue::FlushQueue()
    {
        auto& commandBuffer = SubmissionQueue::GetCommandBuffer();
        auto& stageBuffer = SubmissionQueue::GetStageBuffer();

        stageBuffer.Flush();
        commandBuffer.End();

        VulkanAbstractionLayer::GetCurrentVulkanContext().SubmitCommandsImmediate(commandBuffer);

        commandBuffer.Begin();
        stageBuffer.Reset();
    }

    void SubmissionQueue::CopyToBuffer(const uint8_t* data, size_t byteSize, const VulkanAbstractionLayer::Buffer& buffer, size_t offset)
    {
        SubmissionQueue::RecordAllocation(byteSize);
        auto& stageBuffer = SubmissionQueue::GetStageBuffer();
        auto& commandBuffer = SubmissionQueue::GetCommandBuffer();

        auto stageAlloc = stageBuffer.Submit(data, byteSize);

        commandBuffer.CopyBuffer(
            VulkanAbstractionLayer::BufferInfo{ stageBuffer.GetBuffer(), stageAlloc.Offset },
            VulkanAbstractionLayer::BufferInfo{ buffer, (uint32_t)offset }, 
            stageAlloc.Size
        );
    }

    void SubmissionQueue::CopyFromBuffer(uint8_t* data, size_t byteSize, const VulkanAbstractionLayer::Buffer& buffer, size_t offset)
    {
        SubmissionQueue::RecordAllocation(byteSize);
        auto& stageBuffer = SubmissionQueue::GetStageBuffer();
        auto& commandBuffer = SubmissionQueue::GetCommandBuffer();

        auto stageAlloc = stageBuffer.Submit(nullptr, byteSize);

        commandBuffer.CopyBuffer(
            VulkanAbstractionLayer::BufferInfo{ buffer, (uint32_t)offset },
            VulkanAbstractionLayer::BufferInfo{ stageBuffer.GetBuffer(), stageAlloc.Offset },
            stageAlloc.Size
        );

        SubmissionQueue::FlushQueue();

        uint8_t* mapped = stageBuffer.GetBuffer().MapMemory();
        std::memcpy(data, mapped, byteSize);
    }
}