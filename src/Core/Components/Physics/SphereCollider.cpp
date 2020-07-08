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

#include "SphereCollider.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Components/Instance.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Utilities/Logging/Logger.h"

namespace MxEngine
{
    void SphereCollider::CreateNewShape(const AABB& aabb)
    {
        this->SetColliderChangeFlag(true);
        this->sphereShape = PhysicsFactory::Create<SphereShape>(ToSphere(aabb).GetRedius());
    }

    bool SphereCollider::HasColliderChanged() const
    {
        return this->colliderChanged;
    }

    void SphereCollider::SetColliderChangeFlag(bool value)
    {
        this->colliderChanged = value;
    }

    void SphereCollider::Init()
    {
        this->CreateNewShape(AABB());
    }

    void SphereCollider::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);

        auto instance = self.GetComponent<Instance>();
        auto meshSource = instance.IsValid() ? instance->GetParent()->GetComponent<MeshSource>() : self.GetComponent<MeshSource>();
        
        if (meshSource.IsValid())
        {
            auto& mesh = meshSource->Mesh;
            if (this->savedMeshState != mesh.GetUUID())
            {
                MXLOG_DEBUG("MxEngine::SphereCollider", "updated collider for new mesh");
                auto& meshAABB = mesh->GetAABB();
                this->CreateNewShape(meshAABB);
                this->savedMeshState = mesh.GetUUID();
            }
        }
        else
        {
            this->CreateNewShape(AABB());
        }
    }

    SphereShapeHandle SphereCollider::GetNativeHandle() const
    {
        return this->sphereShape;
    }
}