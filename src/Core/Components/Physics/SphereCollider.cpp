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
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void SphereCollider::CreateNewShape(const BoundingSphere& sphere)
    {
        this->SetColliderChangedFlag(true);
        this->sphereShape = PhysicsFactory::Create<SphereShape>(sphere.Radius);
    }

    void SphereCollider::Init()
    {
        this->CreateNewShape(BoundingSphere());
        this->UpdateCollider();
    }

    void SphereCollider::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);
        if (this->ShouldUpdateCollider(self))
        {
            auto& sphere = ColliderBase::GetBoundingSphere(self);
            this->CreateNewShape(sphere);
        }
    }

    SphereShapeHandle SphereCollider::GetNativeHandle() const
    {
        return this->sphereShape;
    }

    AABB SphereCollider::GetAABB() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->sphereShape->GetAABB(transform);
    }

    BoundingSphere SphereCollider::GetBoundingSphere() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->sphereShape->GetBoundingSphere(transform);
    }

    BoundingSphere SphereCollider::GetBoundingSphereInternal() const
    {
        return this->GetNativeHandle()->GetBoundingSphereUnchanged();
    }

    void SphereCollider::SetBoundingSphere(BoundingSphere sphere)
    {
        this->CreateNewShape(sphere);
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<BoundingSphere>("BoundingSphere")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<BoundingSphere>)
            )
            .constructor<>()
            .property("center", &BoundingSphere::Center)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("radius", &BoundingSphere::Radius)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );

        rttr::registration::class_<SphereCollider>("SphereCollider")
            .constructor<>()
            .property("bounding sphere", &SphereCollider::GetBoundingSphereInternal, &SphereCollider::SetBoundingSphere)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}