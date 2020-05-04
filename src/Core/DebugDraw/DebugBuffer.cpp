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

#include "DebugBuffer.h"

namespace MxEngine
{
    DebugBuffer::DebugBuffer()
    {
        auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
        VBL->PushFloat(3); // position
        VBL->PushFloat(4); // color

        this->VBO = Graphics::Instance()->CreateVertexBuffer(nullptr, 0, UsageType::DYNAMIC_DRAW);
        this->VAO = Graphics::Instance()->CreateVertexArray();
        VAO->AddBuffer(*this->VBO, *VBL);

        this->shader = Graphics::Instance()->CreateShader();
        this->shader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(debug_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(debug_fragment)
        );
    }

    void DebugBuffer::SubmitAABB(const AABB& box, const Vector4& color)
    {
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
    }

    void DebugBuffer::ClearBuffer()
    {
        this->storage.clear();
    }

    void DebugBuffer::SubmitBuffer()
    {
        size_t size = this->GetSize();
        if (size <= this->VBO->GetSize())
            this->VBO->BufferSubData((float*)this->storage.data(), size);
        else
            this->VBO->Load((float*)this->storage.data(), size, UsageType::DYNAMIC_DRAW);
    }

    size_t DebugBuffer::GetSize() const
    {
        return this->storage.size() * sizeof(Point) / sizeof(float);
    }

    const VertexArray& DebugBuffer::GetVAO() const
    {
        return *this->VAO;
    }

    const Shader& DebugBuffer::GetShader() const
    {
        return *this->shader;
    }
}