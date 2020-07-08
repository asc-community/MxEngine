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

#include "BulletRigidBody.h"
#include "Bullet3Utils.h"
#include "Utilities/Memory/Memory.h"

namespace MxEngine
{
    class MotionStateNotifier : public btDefaultMotionState
    {
    public:
        MotionStateNotifier(btTransform& tr) : btDefaultMotionState(tr) { }

        bool TransformUpdated = true;

        virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans)
        {
            this->TransformUpdated = true;
            btDefaultMotionState::setWorldTransform(centerOfMassWorldTrans);
        }
    };

    void BulletRigidBody::DestroyBody()
    {
        if (this->body != nullptr)
        {
            Free(this->body);
            Free(this->state);
        }
    }

    BulletRigidBody::BulletRigidBody()
    {
        btTransform tr;
        this->state = Alloc<MotionStateNotifier>(tr);
        this->body = Alloc<btRigidBody>(0.0f, this->state, nullptr);
    }

    BulletRigidBody::BulletRigidBody(BulletRigidBody&& other) noexcept
    {
        this->body = other.body;
        this->state = other.state;
        other.body = nullptr;
        other.state = nullptr;
    }

    BulletRigidBody& BulletRigidBody::operator=(BulletRigidBody&& other) noexcept
    {
        this->DestroyBody();
        this->body = other.body;
        this->state = other.state;
        other.body = nullptr;
        other.state = nullptr;
        return *this;
    }

    BulletRigidBody::~BulletRigidBody()
    {
        this->DestroyBody();
    }

    btRigidBody* BulletRigidBody::GetNativeHandle()
    {
        return this->body;
    }

    const btRigidBody* BulletRigidBody::GetNativeHandle() const
    {
        return this->body;
    }

    btMotionState* BulletRigidBody::GetMotionState()
    {
        return this->state;
    }

    const btMotionState* BulletRigidBody::GetMotionState() const
    {
        return this->state;
    }

    btCollisionShape* BulletRigidBody::GetCollisionShape()
    {
        return this->body->getCollisionShape();
    }

    const btCollisionShape* BulletRigidBody::GetCollisionShape() const
    {
        return this->body->getCollisionShape();
    }

    void BulletRigidBody::SetCollisionShape(btCollisionShape* shape)
    {
        this->body->setCollisionShape(shape);
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        float mass = this->body->getMass();

        if (mass != 0.0f) shape->calculateLocalInertia(mass, inertia);
        this->body->setMassProps(mass, inertia);
    }

    bool BulletRigidBody::HasTransformUpdate() const
    {
        return static_cast<MotionStateNotifier*>(this->state)->TransformUpdated;
    }

    void BulletRigidBody::SetTransformUpdateFlag(bool value)
    {
        static_cast<MotionStateNotifier*>(this->state)->TransformUpdated = value;
    }

    Vector3 BulletRigidBody::GetScale() const
    {
        auto* collider = this->GetCollisionShape();
        if (collider != nullptr)
            return FromBulletVector3(collider->getLocalScaling());
        else
            return MakeVector3(0.0f);
    }

    void BulletRigidBody::SetScale(const Vector3& scale)
    {
        auto* collider = this->GetCollisionShape();
        if (collider != nullptr)
            collider->setLocalScaling(ToBulletVector3(scale));
    }

    float BulletRigidBody::GetMass() const
    {
        return this->body->getMass();
    }

    void BulletRigidBody::SetMass(float mass)
    {
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        auto* shape = this->GetCollisionShape();

        if (shape != nullptr && mass != 0.0f)
            shape->calculateLocalInertia(mass, inertia);

        this->body->setMassProps(mass, inertia);
    }

    void BulletRigidBody::MakeKinematic()
    {
        this->body->setCollisionFlags(this->body->getCollisionFlags() | btRigidBody::CF_KINEMATIC_OBJECT);
        this->body->setActivationState(DISABLE_DEACTIVATION);
    }

    bool BulletRigidBody::IsKinematic() const
    {
        return this->body->isKinematicObject();
    }

    void BulletRigidBody::SetActivationState(ActivationState state)
    {
        this->body->setActivationState((int)state);
    }

    ActivationState BulletRigidBody::GetActivationState() const
    {
        return ActivationState(this->body->getActivationState());
    }

    bool BulletRigidBody::IsActive() const
    {
        return this->body->isActive();
    }
}