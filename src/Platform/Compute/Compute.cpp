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

#include "Compute.h"

namespace MxEngine
{
    // constexpr std::array<uint64_t, 16> BarrierMappings = {
    //     GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
    //     GL_ELEMENT_ARRAY_BARRIER_BIT,
    //     GL_UNIFORM_BARRIER_BIT,
    //     GL_TEXTURE_FETCH_BARRIER_BIT,
    //     GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
    //     GL_COMMAND_BARRIER_BIT,
    //     GL_PIXEL_BUFFER_BARRIER_BIT,
    //     GL_TEXTURE_UPDATE_BARRIER_BIT,
    //     GL_BUFFER_UPDATE_BARRIER_BIT,
    //     GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
    //     GL_FRAMEBUFFER_BARRIER_BIT,
    //     GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
    //     GL_ATOMIC_COUNTER_BARRIER_BIT,
    //     GL_SHADER_STORAGE_BARRIER_BIT,
    //     GL_QUERY_BUFFER_BARRIER_BIT,
    //     GL_ALL_BARRIER_BITS,
    // };

    // void Compute::Dispatch(const ComputeShaderHandle& computeShader, size_t x, size_t y, size_t z)
    // {
    //     computeShader->Bind();
    //     GLCALL(glDispatchCompute(x, y, z));
    // }

    void Compute::SetMemoryBarrier(BarrierType::Bits barriers)
    {
        // GLbitfield barrierBits = 0;
        // for (size_t i = 0; i < BarrierMappings.size(); i++)
        // {
        //     bool bit = (uint64_t)barriers & (1ull << i);
        //     barrierBits |= (bit ? BarrierMappings[i] : 0);
        // }
        // GLCALL(glMemoryBarrier(barrierBits));
    }
}