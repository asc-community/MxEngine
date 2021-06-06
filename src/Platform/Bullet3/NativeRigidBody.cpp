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

#include "NativeRigidBody.h"
#include "Bullet3Utils.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/Logging/Logger.h"
#include "Core/Application/Physics.h"

namespace MxEngine
{
    class MotionStateNotifier : public btDefaultMotionState
    {
    public:
        MotionStateNotifier(btTransform& tr) : btDefaultMotionState(tr) { }

        bool TransformUpdated = true;

        virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans) override
        {
            this->TransformUpdated = true;
            btDefaultMotionState::setWorldTransform(centerOfMassWorldTrans);
        }
    };

    void NativeRigidBody::DestroyBody()
    {
        if (this->bodyAllocation != nullptr)
        {
            auto body = this->GetNativeHandle();

            Physics::ActiveRigidBodyIsland(body);
            Physics::RemoveRigidBody(body);

            ((MotionStateNotifier*)body->getMotionState())->~MotionStateNotifier();
            body->~btRigidBody();

            delete this->bodyAllocation;
        }
    }

    void NativeRigidBody::ReAddRigidBody()
    {
        Physics::RemoveRigidBody(this->GetNativeHandle());
        Physics::AddRigidBody(this->GetNativeHandle(), this->group, this->mask);
    }

    void NativeRigidBody::UpdateRigidBodyCollider(float mass, btCollisionShape* collider)
    {
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        if (collider != nullptr && mass != 0.0f)
            collider->calculateLocalInertia(mass, inertia);

        this->GetNativeHandle()->setMassProps(mass, inertia);
        if (collider != this->GetCollisionShape())
        {
            this->GetNativeHandle()->setCollisionShape(collider);
            this->ReAddRigidBody();
        }
    }

    NativeRigidBody::NativeRigidBody(const Transform& transform)
    {
        btTransform tr;
        ToBulletTransform(tr, transform);

        this->bodyAllocation = new uint8_t[sizeof(btRigidBody) + sizeof(MotionStateNotifier)];
        auto state = new(this->bodyAllocation + sizeof(btRigidBody)) MotionStateNotifier(tr);
        auto body = new(this->bodyAllocation) btRigidBody(0.0f, state, nullptr);

        Physics::AddRigidBody(body, this->group, this->mask);
    }

    NativeRigidBody::NativeRigidBody(NativeRigidBody&& other) noexcept
    {
        this->bodyAllocation = other.bodyAllocation;
        other.bodyAllocation = nullptr;
    }

    NativeRigidBody& NativeRigidBody::operator=(NativeRigidBody&& other) noexcept
    {
        this->DestroyBody();
        this->bodyAllocation = other.bodyAllocation;
        other.bodyAllocation = nullptr;
        return *this;
    }

    NativeRigidBody::~NativeRigidBody()
    {
        this->DestroyBody();
    }

    btRigidBody* NativeRigidBody::GetNativeHandle()
    {
        return reinterpret_cast<btRigidBody*>(this->bodyAllocation);
    }

    const btRigidBody* NativeRigidBody::GetNativeHandle() const
    {
        return reinterpret_cast<const btRigidBody*>(this->bodyAllocation);
    }

    btMotionState* NativeRigidBody::GetMotionState()
    {
        return this->GetNativeHandle()->getMotionState();
    }

    const btMotionState* NativeRigidBody::GetMotionState() const
    {
        return this->GetNativeHandle()->getMotionState();
    }

    btCollisionShape* NativeRigidBody::GetCollisionShape()
    {
        return this->GetNativeHandle()->getCollisionShape();
    }

    const btCollisionShape* NativeRigidBody::GetCollisionShape() const
    {
        return this->GetNativeHandle()->getCollisionShape();
    }

    void NativeRigidBody::SetCollisionShape(btCollisionShape* shape)
    {
        this->UpdateRigidBodyCollider(this->GetMass(), shape);
    }

    void NativeRigidBody::SetCollisionFilter(uint32_t group, uint32_t mask)
    {
        this->mask = mask;
        this->group = group;
        this->ReAddRigidBody();
    }

    uint32_t NativeRigidBody::GetCollisionGroup() const
    {
        return this->group;
    }

    uint32_t NativeRigidBody::GetCollisionMask() const
    {
        return this->mask;
    }

    bool NativeRigidBody::HasTransformUpdate() const
    {
        return static_cast<const MotionStateNotifier*>(this->GetMotionState())->TransformUpdated;
    }

    void NativeRigidBody::SetTransformUpdateFlag(bool value)
    {
        static_cast<MotionStateNotifier*>(this->GetMotionState())->TransformUpdated = value;
    }

    Vector3 NativeRigidBody::GetScale() const
    {
        auto* collider = this->GetCollisionShape();
        if (collider != nullptr)
            return FromBulletVector3(collider->getLocalScaling());
        else
            return MakeVector3(0.0f);
    }

    bool NativeRigidBody::IsMoving() const
    {
        auto collider = this->GetCollisionShape();
        return collider != nullptr && !collider->isNonMoving();
    }

    bool NativeRigidBody::HasCollisionResponce() const
    {
        return !(this->GetNativeHandle()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    #undef DISABLE_DEACTIVATION
    #undef ACTIVE_TAG

    void NativeRigidBody::SetKinematicFlag()
    {
        auto body = this->GetNativeHandle();
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        this->SetActivationState(ActivationState::DISABLE_DEACTIVATION);
    }

    void NativeRigidBody::UnsetKinematicFlag()
    {
        auto body = this->GetNativeHandle();
        body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        this->SetActivationState(ActivationState::ACTIVE_TAG);
    }

    void NativeRigidBody::SetTriggerFlag()
    {
        auto body = this->GetNativeHandle();
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    void NativeRigidBody::UnsetTriggerFlag()
    {
        auto body = this->GetNativeHandle();
        body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    void NativeRigidBody::UnsetAllFlags()
    {
        this->UnsetTriggerFlag();
        this->UnsetKinematicFlag();
    }

    void NativeRigidBody::SetScale(const Vector3& scale)
    {
        auto* collider = this->GetCollisionShape();
        if (collider != nullptr)
            collider->setLocalScaling(ToBulletVector3(scale));
    }

    float NativeRigidBody::GetMass() const
    {
        return this->GetNativeHandle()->getMass();
    }

    void NativeRigidBody::SetMass(float mass)
    {
        this->UpdateRigidBodyCollider(mass, this->GetCollisionShape());
    }

    void NativeRigidBody::SetActivationState(ActivationState state)
    {
        this->GetNativeHandle()->forceActivationState((int)state);
    }

    ActivationState NativeRigidBody::GetActivationState() const
    {
        return ActivationState(this->GetNativeHandle()->getActivationState());
    }

    void NativeRigidBody::Activate()
    {
        this->GetNativeHandle()->activate(true);
    }

    bool NativeRigidBody::IsActive() const
    {
        return this->GetNativeHandle()->isActive();
    }
}