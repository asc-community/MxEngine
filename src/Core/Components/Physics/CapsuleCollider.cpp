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

#include "CapsuleCollider.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Instancing/Instance.h"

namespace MxEngine
{
    void CapsuleCollider::CreateNewShape(const Capsule& capsule)
    {
        this->SetColliderChangedFlag(true);
        this->capsuleShape = PhysicsFactory::Create<CapsuleShape>(capsule);
    }

    void CapsuleCollider::Init()
    {
        this->CreateNewShape(Capsule());
        this->UpdateCollider();
    }

    void CapsuleCollider::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);
        if (this->ShouldUpdateCollider(self))
        {
            auto& aabb = ColliderBase::GetBoundingBox(self);
            this->CreateNewShape(ToCapsule(aabb, this->GetOrientation()));
        }
    }

    CapsuleShapeHandle CapsuleCollider::GetNativeHandle() const
    {
        return this->capsuleShape;
    }

    void CapsuleCollider::SetOrientation(Capsule::Axis axis)
    {
        auto& self = MxObject::GetByComponent(*this);
        auto& aabb = ColliderBase::GetBoundingBox(self);
        this->CreateNewShape(ToCapsule(aabb, axis));
    }

    Capsule::Axis CapsuleCollider::GetOrientation() const
    {
        return this->capsuleShape->GetOrientation();
    }

    AABB CapsuleCollider::GetBoundingBox() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->capsuleShape->GetBoundingBox(transform);
    }

    BoundingSphere CapsuleCollider::GetBoundingSphere() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->capsuleShape->GetBoundingSphere(transform);
    }

    Capsule CapsuleCollider::GetBoundingCapsule() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->capsuleShape->GetBoundingCapsule(transform);
    }

    void CapsuleCollider::SetBoundingCapsule(const Capsule& cylinder)
    {
        this->CreateNewShape(cylinder);
    }
}