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

#include "RigidBody.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Components/Physics/BoxCollider.h"
#include "Core/Components/Physics/SphereCollider.h"
#include "Core/Components/Physics/CylinderCollider.h"
#include "Core/Components/Physics/CapsuleCollider.h"
#include "Utilities/Logging/Logger.h"
#include "Platform/Bullet3/Bullet3Utils.h"
#include "Core/Application/Physics.h"

namespace MxEngine
{
    void RigidBody::UpdateTransform()
    {
        auto& self = MxObject::GetByComponent(*this);
        auto& selfScale = self.Transform.GetScale();

        if (this->rigidBody->HasTransformUpdate())
        {
            if (this->IsStatic())
            {
                // if body is static, MxObject's Transform component controls its position
                btTransform tr;
                ToBulletTransform(tr, self.Transform);
                this->rigidBody->GetNativeHandle()->setWorldTransform(tr);
            }
            else
            {
                // if body is not static, transform is controlled by physics engine
                FromBulletTransform(self.Transform, this->rigidBody->GetNativeHandle()->getWorldTransform());
            }
            this->rigidBody->SetTransformUpdateFlag(false);
        }

        if (selfScale != this->rigidBody->GetScale())
        {
            this->rigidBody->SetScale(selfScale);
        }
    }

    NativeRigidBodyHandle RigidBody::GetNativeHandle() const
    {
        return this->rigidBody;
    }

    void RigidBody::OnUpdate(float dt)
    {
        this->UpdateCollider();
        this->UpdateTransform();
    }

    template<typename T>
    bool TestCollider(NativeRigidBodyHandle& rigidBody, T collider)
    {
        if (!collider.IsValid()) return false;

        collider->UpdateCollider();
        if (collider->HasColliderChanged())
        {
            MXLOG_DEBUG("MxEngine::NativeRigidBody", "updating collision shape");
            auto shape = collider->GetNativeHandle();
            rigidBody->SetCollisionShape(shape->GetNativeHandle());
            collider->SetColliderChangedFlag(false);
        }
        return true;
    }

    template<typename T>
    void InvalidateCollider(MxObject& self)
    {
        auto collider = self.GetComponent<T>();
        if(collider.IsValid())
            collider->SetColliderChangedFlag(true);
    }

    void RigidBody::Init()
    {
        auto& self = MxObject::GetByComponent(*this);
        this->rigidBody = PhysicsFactory::Create<NativeRigidBody>(self.Transform);

        Physics::SetRigidBodyParent(this->rigidBody->GetNativeHandle(), self);
        
        InvalidateCollider<BoxCollider>(self);
        InvalidateCollider<SphereCollider>(self);
        InvalidateCollider<CylinderCollider>(self);
        InvalidateCollider<CapsuleCollider>(self);
    }

    void RigidBody::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);

        if(TestCollider(this->rigidBody, self.GetComponent<BoxCollider>()))      return;
        if(TestCollider(this->rigidBody, self.GetComponent<SphereCollider>()))   return;
        if(TestCollider(this->rigidBody, self.GetComponent<CylinderCollider>())) return;
        if(TestCollider(this->rigidBody, self.GetComponent<CapsuleCollider>()))  return;

        this->rigidBody->SetCollisionShape(nullptr); // no collider
    }

    void RigidBody::MakeKinematic()
    {
        this->rigidBody->MakeKinematic();
    }

    bool RigidBody::IsKinematic() const
    {
        return this->rigidBody->IsKinematic();
    }

    bool RigidBody::IsStatic() const
    {
        return this->GetMass() == 0.0f;
    }

    void RigidBody::SetCollisionFilter(uint32_t mask, uint32_t group)
    {
        this->rigidBody->SetCollisionFilter(group, mask);
    }

    void RigidBody::SetCollisionFilter(CollisionMask::Mask mask, CollisionGroup::Group group)
    {
        this->SetCollisionFilter((uint32_t)group, (uint32_t)mask);
    }

    uint32_t RigidBody::GetCollisionGroup() const
    {
        return this->rigidBody->GetCollisionGroup();
    }

    uint32_t RigidBody::GetCollisionMask() const
    {
        return this->rigidBody->GetCollisionGroup();
    }

    void RigidBody::ClearForces()
    {
        this->rigidBody->GetNativeHandle()->clearForces();
    }

    float RigidBody::GetMass() const
    {
        return this->rigidBody->GetMass();
    }

    void RigidBody::SetMass(float mass)
    {
        this->rigidBody->SetMass(mass);
    }

    float RigidBody::GetFriction() const
    {
        return this->rigidBody->GetNativeHandle()->getFriction();
    }

    void RigidBody::SetFriction(float value)
    {
        this->rigidBody->GetNativeHandle()->setFriction(value);
    }

    float RigidBody::GetSpinningFriction() const
    {
        return this->rigidBody->GetNativeHandle()->getSpinningFriction();
    }

    void RigidBody::SetSpinningFriction(float value)
    {
        this->rigidBody->GetNativeHandle()->setSpinningFriction(value);
    }

    float RigidBody::GetRollingFriction() const
    {
        return this->rigidBody->GetNativeHandle()->getRollingFriction();
    }

    void RigidBody::SetRollingFriction(float value)
    {
        return this->rigidBody->GetNativeHandle()->setRollingFriction(value);
    }

    void RigidBody::SetLinearVelocity(const Vector3& velocity)
    {
        this->rigidBody->GetNativeHandle()->setLinearVelocity(ToBulletVector3(velocity));
    }

    float RigidBody::GetBounceFactor() const
    {
        return this->rigidBody->GetNativeHandle()->getRestitution();
    }

    void RigidBody::SetBounceFactor(float value)
    {
        this->rigidBody->GetNativeHandle()->setRestitution(value);
    }

    Vector3 RigidBody::GetGravity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getGravity());
    }

    void RigidBody::SetGravity(const Vector3& gravity)
    {
        this->rigidBody->GetNativeHandle()->setGravity(ToBulletVector3(gravity));
    }

    Vector3 RigidBody::GetLinearVelocity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getLinearVelocity());
    }

    void RigidBody::SetAngularVelocity(const Vector3& velocity)
    {
        this->rigidBody->GetNativeHandle()->setAngularVelocity(ToBulletVector3(velocity));
    }

    Vector3 RigidBody::GetAngularVelocity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getAngularVelocity());
    }

    float RigidBody::GetInverseMass() const
    {
        return this->rigidBody->GetNativeHandle()->getInvMass();
    }

    Vector3 RigidBody::GetAngularForceFactor() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getAngularFactor());
    }

    void RigidBody::SetAngularForceFactor(const Vector3& factor)
    {
        this->rigidBody->GetNativeHandle()->setAngularFactor(ToBulletVector3(factor));
    }

    Vector3 RigidBody::GetLinearForceFactor() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getLinearFactor());
    }

    void RigidBody::SetLinearForceFactor(const Vector3& factor)
    {
        this->rigidBody->GetNativeHandle()->setLinearFactor(ToBulletVector3(factor));
    }

    void RigidBody::SetAngularAirResistance(float value)
    {
        this->rigidBody->GetNativeHandle()->setDamping(this->GetLinearAirResistance(), value);
    }

    float RigidBody::GetAngularAirResistance() const
    {
        return this->rigidBody->GetNativeHandle()->getAngularDamping();
    }

    void RigidBody::SetLinearAirResistance(float value)
    {
        this->rigidBody->GetNativeHandle()->setDamping(value, this->GetAngularAirResistance());
    }

    float RigidBody::GetLinearAirResistance() const
    {
        return this->rigidBody->GetNativeHandle()->getLinearDamping();
    }

    Vector3 RigidBody::GetTotalForce() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getTotalForce());
    }

    Vector3 RigidBody::GetTotalTorque() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getTotalTorque());
    }

    Vector3 RigidBody::GetInertia() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getLocalInertia());
    }

    Vector3 RigidBody::GetVelocityInPoint(const Vector3& relativePosition)
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getVelocityInLocalPoint(ToBulletVector3(relativePosition)));
    }

    Vector3 RigidBody::GetPushVelocityInPoint(const Vector3& relativePosition)
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getPushVelocityInLocalPoint(ToBulletVector3(relativePosition)));
    }

    void RigidBody::ApplyCentralImpulse(const Vector3& impulse)
    {
        this->rigidBody->GetNativeHandle()->applyCentralImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyCentralPushImpulse(const Vector3& impulse)
    {
        this->rigidBody->GetNativeHandle()->applyCentralPushImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyForce(const Vector3& force, const Vector3& relativePosition)
    {
        this->rigidBody->GetNativeHandle()->applyForce(ToBulletVector3(force), ToBulletVector3(relativePosition));
    }

    void RigidBody::ApplyImpulse(const Vector3& impulse, const Vector3& relativePosition)
    {
        this->rigidBody->GetNativeHandle()->applyForce(ToBulletVector3(impulse), ToBulletVector3(relativePosition));
    }

    void RigidBody::ApplyPushImpulse(const Vector3& impulse, const Vector3& relativePosition)
    {
        this->rigidBody->GetNativeHandle()->applyPushImpulse(ToBulletVector3(impulse), ToBulletVector3(relativePosition));
    }

    void RigidBody::ApplyTorque(const Vector3& force)
    {
        this->rigidBody->GetNativeHandle()->applyTorque(ToBulletVector3(force));
    }

    void RigidBody::ApplyTorqueImpulse(const Vector3& impulse)
    {
        this->rigidBody->GetNativeHandle()->applyTorqueImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyTorqueTurnImpulse(const Vector3& impulse)
    {
        this->rigidBody->GetNativeHandle()->applyTorqueTurnImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyCentralForce(const Vector3& force)
    {
        this->rigidBody->GetNativeHandle()->applyCentralForce(ToBulletVector3(force));
    }

    void RigidBody::SetPushVelocity(const Vector3& velocity)
    {
        this->rigidBody->GetNativeHandle()->setPushVelocity(ToBulletVector3(velocity));
    }

    Vector3 RigidBody::GetPushVelocity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getPushVelocity());
    }

    void RigidBody::SetTurnVelocity(const Vector3& velocity)
    {
        this->rigidBody->GetNativeHandle()->setTurnVelocity(ToBulletVector3(velocity));
    }

    Vector3 RigidBody::GetTurnVelocity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getTurnVelocity());
    }

    void RigidBody::SetAnisotropicFriction(const Vector3& friction, AnisotropicFriction mode)
    {
        this->rigidBody->GetNativeHandle()->setAnisotropicFriction(ToBulletVector3(friction), (int)mode);
    }

    bool RigidBody::HasAnisotropicFriction() const
    {
        return this->rigidBody->GetNativeHandle()->hasAnisotropicFriction();
    }

    Vector3 RigidBody::GetAnisotropicFriction() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getAnisotropicFriction());
    }
}