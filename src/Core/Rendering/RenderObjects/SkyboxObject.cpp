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

#include "SkyboxObject.h"

namespace MxEngine
{
    void SkyboxObject::Init()
    {
        constexpr float size = 1.0f;
        constexpr std::array vertices = {
            Vector3(-size, -size, -size),
            Vector3(-size, -size,  size),
            Vector3(-size,  size, -size),
            Vector3(-size,  size,  size),
            Vector3( size, -size, -size),
            Vector3( size, -size,  size),
            Vector3( size,  size, -size),
            Vector3( size,  size,  size),
        };
        constexpr std::array indices = {
            1u, 7u, 5u,
            7u, 1u, 3u,
            4u, 2u, 0u,
            2u, 4u, 6u,
            4u, 7u, 6u,
            7u, 4u, 5u,
            2u, 1u, 0u,
            1u, 2u, 3u,
            0u, 5u, 4u,
            5u, 0u, 1u,
            6u, 3u, 2u,
            3u, 6u, 7u,
        };

        // this->VBO = Factory<VertexBuffer>::Create(
        //     (float*)vertices.data(),
        //     vertices.size() * sizeof(Vector3),
        //     UsageType::STATIC_DRAW
        // );
        // 
        // this->IBO = Factory<IndexBuffer>::Create(
        //     indices.data(),
        //     indices.size(),
        //     UsageType::STATIC_DRAW
        // );
        // 
        // std::array vertexLayout = {
        //     VertexAttribute::Entry<Vector3>()
        // };
        // this->VAO = Factory<VertexArray>::Create();
        // VAO->AddVertexLayout(*this->VBO, vertexLayout, VertexAttributeInputRate::PER_VERTEX);
        // VAO->LinkIndexBuffer(*this->IBO);
    }

    // const VertexArray& SkyboxObject::GetVAO() const
    // {
    //     return *this->VAO;
    // }
}