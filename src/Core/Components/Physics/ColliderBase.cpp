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

#include "ColliderBase.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Components/Instancing/Instance.h"
#include "Core/Components/Rendering/MeshSource.h"

namespace MxEngine
{
    MeshSource::Handle GetCurrentlyUsedMesh(MxObject& self)
    {
        auto instance = self.GetComponent<Instance>();
        auto meshSource = instance.IsValid() ? instance->GetParent()->GetComponent<MeshSource>() : self.GetComponent<MeshSource>();
        return meshSource;
    }

    bool ColliderBase::ShouldUpdateCollider(MxObject& self)
    {
        auto meshSource = GetCurrentlyUsedMesh(self);
        if (meshSource.IsValid())
        {
            auto& mesh = meshSource->Mesh;
            auto uuid = mesh.GetUUID();
            if (this->savedMeshState != uuid)
            {
                auto& meshAABB = mesh->GetBoundingBox();
                this->savedMeshState = uuid;
                return true; // ask to update collider to new meshAABB
            }
        }
        else if(this->savedMeshState != UUIDGenerator::GetNull())
        {
            this->savedMeshState = UUIDGenerator::GetNull();
            return true; // ask to update collider to empty AABB
        }

        return false;
    }

    const AABB& ColliderBase::GetBoundingBox(MxObject& self)
    {
        auto meshSource = GetCurrentlyUsedMesh(self); 
        return meshSource->Mesh->GetBoundingBox();
    }

    const BoundingSphere& ColliderBase::GetBoundingSphere(MxObject& self)
    {
        auto meshSource = GetCurrentlyUsedMesh(self);
        return meshSource->Mesh->GetBoundingSphere();
    }

    void ColliderBase::SetColliderChangedFlag(bool value)
    {
        this->colliderChangedFlag = value;
    }

    bool ColliderBase::HasColliderChanged() const
    {
        return this->colliderChangedFlag;
    }
}