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
#include "Core/Application/Application.h"

namespace MxEngine
{
    class AbstractPrimitive : public MxObject
    {
    protected:
        static constexpr size_t VertexSize = (3 + 2 + 3 + 3 + 3);

        // submits vertex data of different LODs in format [v3 (pos), v2 (tex), v3 (norm), v3(tan), v3(bitan)] into first RenderObject on MxObject (creates or replaces existing VBO)
        inline void SubmitData(const std::string& resourceName, const AABB& boundingBox, ArrayView<std::vector<float>> vbos, ArrayView<std::vector<unsigned int>> ibos)
        {
            MX_ASSERT(vbos.size() == ibos.size());

            size_t lodCount = vbos.size();
            auto material = MakeRef<Material>();
            auto transform = MakeRef<Transform>();
            auto color = MakeRef<Vector4>(MakeVector4(1.0f));

            if (this->ObjectMesh == nullptr)
            {
                auto& meshManager = Application::Get()->GetCurrentScene().GetResourceManager<Mesh>();
                this->SetMesh(meshManager.Add(resourceName, MakeUnique<Mesh>()));
            }
            this->ObjectMesh->SetAABB(boundingBox);

            size_t lods = this->ObjectMesh->GetLODCount();
            MX_ASSERT(lods <= lodCount);
            while (lods < lodCount)
            {
                this->ObjectMesh->PushEmptyLOD();
                lods++;
            }

            for (size_t lod = 0; lod < lodCount; lod++)
            {
                // first we create VBO + VAO with data
                auto VBO = Graphics::Instance()->CreateVertexBuffer(vbos[lod].data(), vbos[lod].size(), UsageType::STATIC_DRAW);
                auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
                auto IBO = Graphics::Instance()->CreateIndexBuffer(ibos[lod].data(), ibos[lod].size());
                VBL->PushFloat(3);
                VBL->PushFloat(2);
                VBL->PushFloat(3);
                VBL->PushFloat(3);
                VBL->PushFloat(3);
                auto VAO = Graphics::Instance()->CreateVertexArray();
                VAO->AddBuffer(*VBO, *VBL);

                this->GetMesh()->SetLOD(lod);

                // check if object already has mesh.If so - replace first render object VAO
                if (!this->ObjectMesh->GetRenderObjects().empty())
                {
                    size_t buffers = ObjectMesh->GetBufferCount();
                    for (size_t i = 0; i < buffers; i++)
                    {
                        VAO->AddInstancedBuffer(ObjectMesh->GetBufferByIndex(i), ObjectMesh->GetBufferLayoutByIndex(i));
                    }
                    // Note that we replace only first render object as we are working with AbstractPrimitive
                    auto& base = this->ObjectMesh->GetRenderObjects().front();

                    SubMesh newBase(base.GetName(), std::move(VBO), std::move(VAO), std::move(IBO),
                        MakeRef<Material>(base.GetMaterial()), MakeRef<Vector4>(base.GetRenderColor()), MakeRef<Transform>(base.GetTransform()), 
                        base.UsesTexture(), base.UsesNormals(), vbos[lod].size() / VertexSize);
                    base = std::move(newBase);
                }
                else
                {
                    SubMesh object("main", std::move(VBO), std::move(VAO), std::move(IBO),
                        material, color, transform, true, true, vbos[lod].size() / VertexSize);
                    this->ObjectMesh->GetRenderObjects().push_back(std::move(object));
                }
            }
        }
    };
}