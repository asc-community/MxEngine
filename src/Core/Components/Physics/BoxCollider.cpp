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

#include "BoxCollider.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void BoxCollider::CreateNewShape(const BoundingBox& box)
    {
        this->SetColliderChangedFlag(true);
        this->boxShape = PhysicsFactory::Create<BoxShape>(box);
    }

    void BoxCollider::Init()
    {
        this->CreateNewShape(BoundingBox());
        this->UpdateCollider();
    }

    void BoxCollider::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);
        if (this->ShouldUpdateCollider(self))
        {
            auto& aabb = ColliderBase::GetAABB(self);
            this->CreateNewShape(ToBoundingBox(aabb));
        }
    }

    BoxShapeHandle BoxCollider::GetNativeHandle() const
    {
        return this->boxShape;
    }

    AABB BoxCollider::GetAABB() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->boxShape->GetAABBTransformed(transform);
    }

    BoundingBox BoxCollider::GetBoundingBox() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->boxShape->GetBoundingBoxTransformed(transform);
    }

    BoundingSphere BoxCollider::GetBoundingSphere() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->boxShape->GetBoundingSphereTransformed(transform);
    }

    BoundingBox BoxCollider::GetBoundingBoxInternal() const
    {
        return this->GetNativeHandle()->GetBoundingBox();
    }

    void BoxCollider::SetBoundingBox(BoundingBox box)
    {
        this->CreateNewShape(box);
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<BoundingBox>("BoundingBox")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<BoundingBox>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("center", &BoundingBox::Center)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("min", &BoundingBox::Min)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max", &BoundingBox::Max)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("rotation", &BoundingBox::Rotation)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.5f)
            );

        rttr::registration::class_<BoxCollider>("BoxCollider")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("bounding box", &BoxCollider::GetBoundingBoxInternal, &BoxCollider::SetBoundingBox)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}