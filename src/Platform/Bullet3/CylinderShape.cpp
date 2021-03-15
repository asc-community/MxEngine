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

#include "CylinderShape.h"
#include "Bullet3Utils.h"

using btCylinderShapeY = btCylinderShape;

namespace MxEngine
{
    CylinderShape::CylinderShape(const Cylinder& cylinder)
    {
        btVector3 boundings{ 0.0f, 0.0f, 0.0f };
        this->orientation = cylinder.Orientation;
        switch (cylinder.Orientation)
        {
        case Cylinder::Axis::X:
            boundings.setValue(cylinder.Height * 0.5f, cylinder.RadiusX, cylinder.RadiusZ);
            this->CreateShape<btCylinderShapeX>(boundings);
            break;
        case Cylinder::Axis::Y:
            boundings.setValue(cylinder.RadiusX, cylinder.Height * 0.5f, cylinder.RadiusZ);
            this->CreateShape<btCylinderShapeY>(boundings);
            break;
        case Cylinder::Axis::Z:
            boundings.setValue(cylinder.RadiusX, cylinder.RadiusZ, cylinder.Height * 0.5f);
            this->CreateShape<btCylinderShapeZ>(boundings);
            break;
        }
    }

    CylinderShape::CylinderShape(CylinderShape&& other) noexcept
    {
        this->collider = other.collider;
        other.collider = nullptr;
    }

    CylinderShape& CylinderShape::operator=(CylinderShape&& other) noexcept
    {
        this->DestroyShape();
        this->collider = other.collider;
        other.collider = nullptr;
        return *this;
    }

    CylinderShape::~CylinderShape()
    {
        this->DestroyShape();
    }

    Cylinder CylinderShape::GetBoundingCylinderTransformed(const TransformComponent& transform) const
    {
        auto cylinder = this->GetBoundingCylinder();
        auto& scale = transform.GetScale();
        cylinder.Center += transform.GetPosition();
        cylinder.Rotation = transform.GetRotationQuaternion();
        switch (cylinder.Orientation)
        {
        case Cylinder::Axis::X:
            cylinder.Height *= scale.x;
            cylinder.RadiusX *= scale.y;
            cylinder.RadiusZ *= scale.z;
            break;
        case Cylinder::Axis::Y:
            cylinder.Height *= scale.y;
            cylinder.RadiusX *= scale.x;
            cylinder.RadiusZ *= scale.z;
            break;
        case Cylinder::Axis::Z:
            cylinder.Height *= scale.z;
            cylinder.RadiusX *= scale.x;
            cylinder.RadiusZ *= scale.y;
            break;
        }
        return cylinder;
    }

    Cylinder CylinderShape::GetBoundingCylinder() const
    {
        auto box = this->GetAABB();
        auto cylinder = ToCylinder(box, this->orientation);
        return cylinder;
    }

    Cylinder CylinderShape::GetNativeBoundingTransformed(const TransformComponent& transform) const
    {
        return this->GetBoundingCylinderTransformed(transform);
    }

    Cylinder CylinderShape::GetNativeBounding() const
    {
        return this->GetBoundingCylinder();
    }

    Cylinder::Axis CylinderShape::GetOrientation() const
    {
        return this->orientation;
    }
}