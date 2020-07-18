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

#include "ShapeBase.h"
#include "Bullet3Utils.h"

namespace MxEngine
{
    void ShapeBase::DestroyShape()
    {
        if (this->collider != nullptr)
        {
            Free(this->collider);
        }
    }

    void ShapeBase::SetScale(const Vector3& scale)
    {
        this->collider->setLocalScaling(ToBulletVector3(scale));
    }

    Vector3 ShapeBase::GetScale() const
    {
        return FromBulletVector3(this->collider->getLocalScaling());
    }

    btCollisionShape* ShapeBase::GetNativeHandle()
    {
        return this->collider;
    }

    AABB ShapeBase::GetAABB(const Transform& transform) const
    {
        btVector3 min, max;
        btTransform tr;
        ToBulletTransform(tr, transform);
        this->collider->getAabb(tr, min, max);
        return AABB{ FromBulletVector3(min), FromBulletVector3(max) };
    }

    AABB ShapeBase::GetAABBUnchanged() const
    {
        auto scale = this->GetScale();
        btVector3 min, max;
        btTransform tr;
        tr.setIdentity();
        this->collider->getAabb(tr, min, max);
        if (scale == MakeVector3(0.0f))
            scale = MakeVector3(1.0f);
        return AABB{ FromBulletVector3(min) / scale, FromBulletVector3(max) / scale };
    }

    BoundingSphere ShapeBase::GetBoundingSphere(const Transform& transform) const
    {
        btVector3 center;
        float r = 0.0f;
        this->collider->getBoundingSphere(center, r);
        return BoundingSphere(FromBulletVector3(center) + transform.GetPosition(), r / RootThree<float>());
    }

    BoundingSphere ShapeBase::GetBoundingSphereUnchanged() const
    {
        auto scale = this->GetScale();
        btVector3 center;
        float r = 0.0f;
        this->collider->getBoundingSphere(center, r);
        if (scale == MakeVector3(0.0f))
            scale = MakeVector3(1.0f);
        r /= ComponentMax(scale);
        return BoundingSphere(FromBulletVector3(center), r);
    }
}