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

#pragma once

#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
    class AbstractPrimitive : public MxObject
    {
    protected:
        static constexpr size_t VertexSize = (3 + 2 + 3);

        ~AbstractPrimitive() = default;

        inline void SubmitData(const std::vector<float>& data)
        {
            auto VBO = Graphics::Instance()->CreateVertexBuffer(data, UsageType::STATIC_DRAW);
            auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
            VBL->PushFloat(3);
            VBL->PushFloat(2);
            VBL->PushFloat(3);
            auto VAO = Graphics::Instance()->CreateVertexArray();
            VAO->AddBuffer(*VBO, *VBL);

            if (this->object != nullptr && !this->object->GetRenderObjects().empty())
            {
                size_t size = object->GetBufferCount();
                for (size_t i = 0; i < size; i++)
                {
                    VAO->AddInstancedBuffer(object->GetBufferByIndex(i), object->GetBufferLayoutByIndex(i));
                }
                auto& base = this->object->GetRenderObjects().front();
                RenderObject sphere(std::move(VBO), std::move(VAO), MakeUnique<Material>(base.GetMaterial()),
                    base.UsesTexture(), base.UsesNormals(), data.size() / VertexSize);
                base = std::move(sphere);
            }
            else
            {
                auto material = MakeUnique<Material>();

                RenderObject object(std::move(VBO), std::move(VAO), std::move(material), true, true, data.size() / VertexSize);
                auto container = MakeRef<RenderObjectContainer>();
                container->GetRenderObjects().push_back(std::move(object));

                this->Load(container);
            }
        }
    };
}