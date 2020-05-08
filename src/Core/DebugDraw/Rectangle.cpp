// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "Rectangle.h"

namespace MxEngine
{
    Rectangle::Rectangle(float halfSize)
    {
        std::array vertecies =
        {
            Vector4(-halfSize, -halfSize, 0.5f, 1.0f),
            Vector4( halfSize, -halfSize, 0.5f, 1.0f),
            Vector4(-halfSize,  halfSize, 0.5f, 1.0f),
            Vector4(-halfSize,  halfSize, 0.5f, 1.0f),
            Vector4( halfSize, -halfSize, 0.5f, 1.0f),
            Vector4( halfSize,  halfSize, 0.5f, 1.0f),
        };

        this->VBO = Graphics::Instance()->CreateVertexBuffer((float*)vertecies.data(), vertecies.size() * sizeof(float), UsageType::STATIC_DRAW);
        auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
        VBL->PushFloat(4);
        this->VAO = Graphics::Instance()->CreateVertexArray();
        VAO->AddBuffer(*VBO, *VBL);
    }
    
    const VertexArray& MxEngine::Rectangle::GetVAO() const
    {
        return *this->VAO;
    }
}