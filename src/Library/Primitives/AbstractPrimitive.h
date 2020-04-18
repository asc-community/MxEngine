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
#include "Utilities/Array/ArrayView.h"
#include "Utilities/Logger/Logger.h"

namespace MxEngine
{
    class AbstractPrimitive : public MxObject
    {
    protected:
        static constexpr size_t VertexSize = (3 + 2 + 3);

<<<<<<< Updated upstream
        // submits vertex data in format [v3, v2, v3] into first RenderObject on MxObject (creates or replaces existing VBO)
        inline void SubmitData(const std::string& resourceName, ArrayView<float> buffer)
=======
        // submits vertex data in format [v3 (pos), v2 (tex), v3 (norm)] into first RenderObject on MxObject (creates or replaces existing VBO)
        inline void SubmitData(const std::string& resourceName, ArrayView<float> vbo, ArrayView<unsigned int> ibo)
>>>>>>> Stashed changes
        {
            // first we create VBO + VAO with data
            auto VBO = Graphics::Instance()->CreateVertexBuffer(vbo.data(), vbo.size(), UsageType::STATIC_DRAW);
            auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
            auto IBO = Graphics::Instance()->CreateIndexBuffer(ibo.data(), ibo.size());
            VBL->PushFloat(3);
            VBL->PushFloat(2);
            VBL->PushFloat(3);
            auto VAO = Graphics::Instance()->CreateVertexArray();
            VAO->AddBuffer(*VBO, *VBL);

            // check if object already has mesh. If so - replace first render object VAO
            if (this->ObjectMesh != nullptr && !this->ObjectMesh->GetRenderObjects().empty())
            {
                size_t buffers = ObjectMesh->GetBufferCount();
                for (size_t i = 0; i < buffers; i++)
                {
                    VAO->AddInstancedBuffer(ObjectMesh->GetBufferByIndex(i), ObjectMesh->GetBufferLayoutByIndex(i));
                }
                // Note that we replace only first render object as we are working with AbstractPrimitive
                auto& base = this->ObjectMesh->GetRenderObjects().front();
                RenderObject newBase(base.GetName(), std::move(VBO), std::move(VAO), std::move(IBO), MakeUnique<Material>(base.GetMaterial()),
                    base.UsesTexture(), base.UsesNormals(), vbo.size() / VertexSize);
                base = std::move(newBase);
            }
            else
            {
                // if no render object exists (it is possible if MxObject is empty) - create one
                auto material = MakeUnique<Material>();

                RenderObject object("main", std::move(VBO), std::move(VAO), std::move(IBO), std::move(material), true, true, vbo.size() / VertexSize);
                auto mesh = MakeUnique<Mesh>();
                mesh->GetRenderObjects().push_back(std::move(object));
                auto& meshManager = Application::Get()->GetCurrentScene().GetResourceManager<Mesh>();
                this->SetMesh(meshManager.Add(resourceName, std::move(mesh)));
            }
        }
    };
}