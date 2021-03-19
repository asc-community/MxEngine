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

#include "CylinderCollider.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void CylinderCollider::CreateNewShape(const Cylinder& cylinder)
    {
        this->SetColliderChangedFlag(true);
        this->cylinderShape = PhysicsFactory::Create<CylinderShape>(cylinder);
    }

    void CylinderCollider::Init()
    {
        this->CreateNewShape(Cylinder());
        this->UpdateCollider();
    }

    void CylinderCollider::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);
        if (this->ShouldUpdateCollider(self))
        {
            auto& aabb = ColliderBase::GetAABB(self);
            this->CreateNewShape(ToCylinder(aabb, this->GetOrientation()));
        }
    }

    CylinderShapeHandle CylinderCollider::GetNativeHandle() const
    {
        return this->cylinderShape;
    }

    void CylinderCollider::SetOrientation(Cylinder::Axis axis)
    {
        auto& self = MxObject::GetByComponent(*this);
        auto& aabb = ColliderBase::GetAABB(self);
        this->CreateNewShape(ToCylinder(aabb, axis));
    }

    Cylinder::Axis CylinderCollider::GetOrientation() const
    {
        return this->cylinderShape->GetOrientation();
    }

    AABB CylinderCollider::GetAABB() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->cylinderShape->GetAABBTransformed(transform);
    }

    BoundingSphere CylinderCollider::GetBoundingSphere() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->cylinderShape->GetBoundingSphereTransformed(transform);
    }

    Cylinder CylinderCollider::GetBoundingCylinder() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->cylinderShape->GetBoundingCylinderTransformed(transform);
    }

    Cylinder CylinderCollider::GetBoundingCylinderInternal() const
    {
        return this->GetNativeHandle()->GetBoundingCylinder();
    }

    void CylinderCollider::SetBoundingCylinder(Cylinder cylinder)
    {
        this->CreateNewShape(cylinder);
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::enumeration<Cylinder::Axis>("CylinderAxis")
        (
            rttr::value("X", Cylinder::Axis::X),
            rttr::value("Y", Cylinder::Axis::Y),
            rttr::value("Z", Cylinder::Axis::Z)
        );

        rttr::registration::class_<Cylinder>("Cylinder")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<Cylinder>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("orientation", &Cylinder::Orientation)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("center", &Cylinder::Center)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("height", &Cylinder::Height)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("radius x", &Cylinder::RadiusX)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("radius z", &Cylinder::RadiusZ)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("rotation", &Cylinder::Rotation)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.5f)
            );

        rttr::registration::class_<CylinderCollider>("CylinderCollider")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("bounding cylinder", &CylinderCollider::GetBoundingCylinderInternal, &CylinderCollider::SetBoundingCylinder)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}