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

#include "CapsuleShape.h"
#include "Bullet3Utils.h"

using btCapsuleShapeY = btCapsuleShape;

namespace MxEngine
{
    CapsuleShape::CapsuleShape(const Capsule& capsule)
    {
        this->orientation = capsule.Orientation;
        switch (capsule.Orientation)
        {
        case Capsule::Axis::X:
            this->CreateShape<btCapsuleShapeX>(capsule.Radius, capsule.Height);
            break;
        case Capsule::Axis::Y:
            this->CreateShape<btCapsuleShapeY>(capsule.Radius, capsule.Height);
            break;
        case Capsule::Axis::Z:
            this->CreateShape<btCapsuleShapeZ>(capsule.Radius, capsule.Height);
            break;
        }
    }

    CapsuleShape::CapsuleShape(CapsuleShape&& other) noexcept
    {
        this->collider = other.collider;
        other.collider = nullptr;
    }

    CapsuleShape& CapsuleShape::operator=(CapsuleShape&& other) noexcept
    {
        this->DestroyShape();
        this->collider = other.collider;
        other.collider = nullptr;
        return *this;
    }

    CapsuleShape::~CapsuleShape()
    {
        this->DestroyShape();
    }

    Capsule CapsuleShape::GetBoundingCapsule(const TransformComponent& transform) const
    {
        auto capsule = this->GetBoundingCapsuleUnchanged();
        auto& scale = transform.GetScale();
        capsule.Center += transform.GetPosition();
        capsule.Rotation = transform.GetRotation();
        switch (capsule.Orientation)
        {
        case Capsule::Axis::X:
            capsule.Height *= scale.x;
            capsule.Radius *= (scale.y + scale.z) * 0.5f;
            break;
        case Capsule::Axis::Y:
            capsule.Height *= scale.y;
            capsule.Radius *= (scale.x + scale.z) * 0.5f;
            break;
        case Capsule::Axis::Z:
            capsule.Height *= scale.z;
            capsule.Radius *= (scale.x + scale.y) * 0.5f;
            break;
        }
        return capsule;
    }

    Capsule CapsuleShape::GetBoundingCapsuleUnchanged() const
    {
        auto box = this->GetAABBUnchanged();
        auto scale = this->GetScale();
        box.Min *= scale;
        box.Max *= scale;
        auto capsule = ToCapsule(box, this->orientation);
        switch (this->orientation)
        {
        case Capsule::Axis::X:
            capsule.Radius /= (scale.y + scale.z) * 0.5f;
            capsule.Height /= scale.x;
            break;
        case Capsule::Axis::Y:
            capsule.Radius /= (scale.x + scale.z) * 0.5f;
            capsule.Height /= scale.y;
            break;
        case Capsule::Axis::Z:
            capsule.Radius /= (scale.x + scale.y) * 0.5f;
            capsule.Height /= scale.z;
            break;
        }
        return capsule;
    }

    Capsule::Axis CapsuleShape::GetOrientation() const
    {
        return this->orientation;
    }
}