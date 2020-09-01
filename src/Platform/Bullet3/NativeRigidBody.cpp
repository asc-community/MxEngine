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
        btTransform cachedTransform;
    public:
        MotionStateNotifier(btTransform& tr) : btDefaultMotionState(tr), cachedTransform(tr) { }

        bool TransformUpdated = true;

        virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans) override
        {
            this->TransformUpdated = true;
            this->cachedTransform = centerOfMassWorldTrans;
            btDefaultMotionState::setWorldTransform(centerOfMassWorldTrans);
        }

        virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const override
        {
            centerOfMassWorldTrans = this->cachedTransform;
        }
        
        virtual ~MotionStateNotifier() = default;
    };

    void NativeRigidBody::DestroyBody()
    {
        if (this->body != nullptr)
        {
            Physics::RemoveRigidBody(this->body);

            Free(this->body);
            Free(this->state);
        }
    }

    void NativeRigidBody::ReAddRigidBody()
    {
        Physics::RemoveRigidBody(this->body);
        Physics::AddRigidBody(this->body, this->group, this->mask);
    }

    void NativeRigidBody::UpdateRigidBodyCollider(float mass, btCollisionShape* collider)
    {
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        if (collider != nullptr && mass != 0.0f)
            collider->calculateLocalInertia(mass, inertia);

        this->body->setMassProps(mass, inertia);
        if (collider != this->GetCollisionShape())
        {
            this->body->setCollisionShape(collider);
            this->ReAddRigidBody();
        }
    }

    NativeRigidBody::NativeRigidBody(const TransformComponent& transform)
    {
        btTransform tr;
        ToBulletTransform(tr, transform);
        this->state = Alloc<MotionStateNotifier>(tr);
        this->body = Alloc<btRigidBody>(0.0f, this->state, nullptr);

        Physics::AddRigidBody(this->body, this->group, this->mask);
    }

    NativeRigidBody::NativeRigidBody(NativeRigidBody&& other) noexcept
    {
        this->body = other.body;
        this->state = other.state;
        other.body = nullptr;
        other.state = nullptr;
    }

    NativeRigidBody& NativeRigidBody::operator=(NativeRigidBody&& other) noexcept
    {
        this->DestroyBody();
        this->body = other.body;
        this->state = other.state;
        other.body = nullptr;
        other.state = nullptr;
        return *this;
    }

    NativeRigidBody::~NativeRigidBody()
    {
        this->DestroyBody();
    }

    btRigidBody* NativeRigidBody::GetNativeHandle()
    {
        return this->body;
    }

    const btRigidBody* NativeRigidBody::GetNativeHandle() const
    {
        return this->body;
    }

    btMotionState* NativeRigidBody::GetMotionState()
    {
        return this->state;
    }

    const btMotionState* NativeRigidBody::GetMotionState() const
    {
        return this->state;
    }

    btCollisionShape* NativeRigidBody::GetCollisionShape()
    {
        return this->body->getCollisionShape();
    }

    const btCollisionShape* NativeRigidBody::GetCollisionShape() const
    {
        return this->body->getCollisionShape();
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
        return static_cast<MotionStateNotifier*>(this->state)->TransformUpdated;
    }

    void NativeRigidBody::SetTransformUpdateFlag(bool value)
    {
        static_cast<MotionStateNotifier*>(this->state)->TransformUpdated = value;
    }

    Vector3 NativeRigidBody::GetScale() const
    {
        auto* collider = this->GetCollisionShape();
        if (collider != nullptr)
            return FromBulletVector3(collider->getLocalScaling());
        else
            return MakeVector3(0.0f);
    }

    #undef DISABLE_DEACTIVATION
    #undef ACTIVE_TAG

    void NativeRigidBody::SetKinematicFlag(bool flag)
    {
        if (flag)
        {
            this->body->setCollisionFlags(this->body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            this->SetActivationState(ActivationState::DISABLE_DEACTIVATION);
            this->Activate();
        }
        else
        {
            this->body->setCollisionFlags(this->body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
            this->SetActivationState(ActivationState::ACTIVE_TAG);
        }
    }

    void NativeRigidBody::SetScale(const Vector3& scale)
    {
        auto* collider = this->GetCollisionShape();
        if (collider != nullptr)
            collider->setLocalScaling(ToBulletVector3(scale));
    }

    float NativeRigidBody::GetMass() const
    {
        return this->body->getMass();
    }

    void NativeRigidBody::SetMass(float mass)
    {
        this->UpdateRigidBodyCollider(mass, this->GetCollisionShape());
    }

    void NativeRigidBody::SetActivationState(ActivationState state)
    {
        this->body->forceActivationState((int)state);
        this->Activate();
    }

    ActivationState NativeRigidBody::GetActivationState() const
    {
        return ActivationState(this->body->getActivationState());
    }

    void NativeRigidBody::Activate()
    {
        this->body->activate(true);
    }

    bool NativeRigidBody::IsActive() const
    {
        return this->body->isActive();
    }

    const char* EnumToString(CollisionMask::Mask mask)
    {
        switch (mask)
        {
        case CollisionMask::GHOST:
            return "GHOST";
        case CollisionMask::RAYCAST_ONLY:
            return "RAYCAST_ONLY";
        case CollisionMask::DYNAMIC:
            return "DYNAMIC";
        case CollisionMask::STATIC:
            return "STATIC";
        case CollisionMask::KINEMATIC:
            return "KINEMATIC";
        default:
            return "CUSTOM";
        }
    }

    const char* EnumToString(CollisionGroup::Group group)
    {
        switch (group)
        {
        case CollisionGroup::NONE:
            return "NONE";
        case CollisionGroup::RAYCAST_ONLY:
            return "RAYCAST_ONLY";
        case CollisionGroup::ALL_NO_RAYCAST:
            return "ALL_NO_RAYCAST";
        case CollisionGroup::ALL:
            return "ALL";
        case CollisionGroup::NO_STATIC_COLLISIONS_NO_RAYCAST:
            return "NO_STATIC_COLLISIONS_NO_RAYCAST";
        case CollisionGroup::NO_STATIC_COLLISIONS:
            return "NO_STATIC_COLLISIONS";
        default:
            return "CUSTOM";
        }
    }
}