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
#include "Core/Components/Physics/CompoundCollider.h"
#include "Utilities/Logging/Logger.h"
#include "Platform/Bullet3/Bullet3Utils.h"
#include "Core/Application/Physics.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void RigidBody::UpdateTransform()
    {
        auto& self = MxObject::GetByComponent(*this);
        auto& selfScale = self.Transform.GetScale();

        if (this->IsKinematic())
        {
            // if body is kinematic, MxObject's Transform component controls its position
            btTransform tr;
            ToBulletTransform(tr, self.Transform);
            this->rigidBody->GetNativeHandle()->getMotionState()->setWorldTransform(tr);
        }
        else if (this->rigidBody->HasTransformUpdate())
        {
            // if body is not kinematic, transform is controlled by physics engine
            FromBulletTransform(self.Transform, this->rigidBody->GetNativeHandle()->getWorldTransform());
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
        // initialized with a bit of bounce. Just because I like it
        this->SetBounceFactor(0.1f);
        
        InvalidateCollider<BoxCollider>(self);
        InvalidateCollider<SphereCollider>(self);
        InvalidateCollider<CylinderCollider>(self);
        InvalidateCollider<CapsuleCollider>(self);
        InvalidateCollider<CompoundCollider>(self);
    }

    void RigidBody::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);

        if(TestCollider(this->rigidBody, self.GetComponent<BoxCollider>()))      return;
        if(TestCollider(this->rigidBody, self.GetComponent<SphereCollider>()))   return;
        if(TestCollider(this->rigidBody, self.GetComponent<CylinderCollider>())) return;
        if(TestCollider(this->rigidBody, self.GetComponent<CapsuleCollider>()))  return;
        if(TestCollider(this->rigidBody, self.GetComponent<CompoundCollider>()))  return;

        this->rigidBody->SetCollisionShape(nullptr); // no collider
    }

    void RigidBody::InvokeOnCollisionEnterCallback(MxObject& self, MxObject& object)
    {
        if (this->onCollisionEnter)
            this->onCollisionEnter(self, object);
    }

    void RigidBody::InvokeOnCollisionExitCallback(MxObject& self, MxObject& object)
    {
        if (this->onCollisionExit)
            this->onCollisionExit(self, object);
    }

    void RigidBody::InvokeOnCollisionCallback(MxObject& self, MxObject& object)
    {
        if (this->onCollision)
            this->onCollision(self, object);
    }

    void RigidBody::MakeKinematic()
    {
        this->SetMass(0.0f);
        this->SetCollisionFilter(CollisionMask::KINEMATIC, CollisionGroup::NO_STATIC_COLLISIONS);
        this->rigidBody->UnsetAllFlags();
        this->rigidBody->SetKinematicFlag();
        // from bullet3 manual (see https://github.com/bulletphysics/bullet3/blob/master/docs/Bullet_User_Manual.pdf page 22)
    }

    void RigidBody::MakeDynamic()
    {
        if(this->GetMass() == 0.0f) this->SetMass(1.0f);
        this->SetCollisionFilter(CollisionMask::DYNAMIC, CollisionGroup::ALL);
        this->rigidBody->UnsetAllFlags();
    }

    void RigidBody::MakeStatic()
    {
        this->SetMass(0.0f);
        this->SetCollisionFilter(CollisionMask::STATIC, CollisionGroup::NO_STATIC_COLLISIONS);
        this->rigidBody->UnsetAllFlags();
    }

    void RigidBody::MakeTrigger()
    {
        this->SetMass(0.0f);
        this->SetCollisionFilter(CollisionMask::STATIC, CollisionGroup::NO_STATIC_COLLISIONS);
        this->rigidBody->UnsetAllFlags();
        this->rigidBody->SetTriggerFlag();
    }

    bool RigidBody::IsKinematic() const
    {
        return this->GetCollisionMask() & CollisionMask::KINEMATIC;
    }

    bool RigidBody::IsDynamic() const
    {
        return this->GetCollisionMask() & CollisionMask::DYNAMIC;
    }

    bool RigidBody::IsStatic() const
    {
        return this->GetCollisionMask() & CollisionMask::STATIC;
    }

    bool RigidBody::IsTrigger() const
    {
        return !this->rigidBody->HasCollisionResponce();
    }

    bool RigidBody::IsRayCastable() const
    {
        return this->GetCollisionGroup() & CollisionGroup::RAYCAST_ONLY;
    }

    void RigidBody::ToggleRayCasting(bool value)
    {
        if (value)
            this->SetCollisionFilter(this->GetCollisionMask(), this->GetCollisionGroup() | CollisionGroup::RAYCAST_ONLY);
        else
            this->SetCollisionFilter(this->GetCollisionMask(), this->GetCollisionGroup() & ~CollisionGroup::RAYCAST_ONLY);
    }

    bool RigidBody::IsMoving() const
    {
        return this->rigidBody->IsMoving();
    }

    void RigidBody::SetStateDynamic(bool value)
    {
        if (value) this->MakeDynamic();
    }

    void RigidBody::SetStateStatic(bool value)
    {
        if (value) this->MakeStatic();
    }

    void RigidBody::SetStateKinematic(bool value)
    {
        if (value) this->MakeKinematic();
    }

    void RigidBody::SetStateTrigger(bool value)
    {
        if (value) this->MakeTrigger();
    }

    void RigidBody::SetCollisionFilter(uint32_t mask, uint32_t group)
    {
        this->rigidBody->SetCollisionFilter(group, mask);
    }

    void RigidBody::SetCollisionFilter(CollisionMask::Mask mask, CollisionGroup::Group group)
    {
        this->SetCollisionFilter((uint32_t)mask, (uint32_t)group);
    }

    uint32_t RigidBody::GetCollisionGroup() const
    {
        return this->rigidBody->GetCollisionGroup();
    }

    uint32_t RigidBody::GetCollisionMask() const
    {
        return this->rigidBody->GetCollisionMask();
    }

    void RigidBody::ActivateParentIsland()
    {
        Physics::ActiveRigidBodyIsland(this->rigidBody->GetNativeHandle());
    }

    void RigidBody::SetActivationState(ActivationState state)
    {
        this->rigidBody->SetActivationState(state);
    }

    ActivationState RigidBody::GetActivationState() const
    {
        return this->rigidBody->GetActivationState();
    }

    AABB RigidBody::GetAABB() const
    {
        AABB result{ };
        auto collider = this->rigidBody->GetCollisionShape();
        if (collider != nullptr)
        {
            btVector3 min, max;
            auto& tr = this->rigidBody->GetNativeHandle()->getWorldTransform();
            collider->getAabb(tr, min, max);
            result.Min = FromBulletVector3(min);
            result.Max = FromBulletVector3(max);
        }
        return result;
    }

    void RigidBody::Activate()
    {
        this->rigidBody->Activate();
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

    void RigidBody::SetLinearVelocity(Vector3 velocity)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->setLinearVelocity(ToBulletVector3(velocity));
    }

    float RigidBody::GetBounceFactor() const
    {
        return this->rigidBody->GetNativeHandle()->getRestitution();
    }

    void RigidBody::SetBounceFactor(float value)
    {
        this->rigidBody->GetNativeHandle()->setRestitution(Clamp(value, 0.0f, 1.0f));
    }

    Vector3 RigidBody::GetGravity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getGravity());
    }

    void RigidBody::SetGravity(Vector3 gravity)
    {
        this->rigidBody->GetNativeHandle()->setGravity(ToBulletVector3(gravity));
    }

    Vector3 RigidBody::GetLinearVelocity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getLinearVelocity());
    }

    void RigidBody::SetAngularVelocity(Vector3 velocity)
    {
        this->rigidBody->Activate();
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

    void RigidBody::SetAngularForceFactor(Vector3 factor)
    {
        this->rigidBody->GetNativeHandle()->setAngularFactor(ToBulletVector3(factor));
    }

    Vector3 RigidBody::GetLinearForceFactor() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getLinearFactor());
    }

    void RigidBody::SetLinearForceFactor(Vector3 factor)
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

    Vector3 RigidBody::GetTotalForceApplied() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getTotalForce());
    }

    Vector3 RigidBody::GetTotalTorqueApplied() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getTotalTorque());
    }

    Vector3 RigidBody::GetInertia() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getLocalInertia());
    }

    Vector3 RigidBody::GetVelocityInPoint(Vector3 relativePosition)
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getVelocityInLocalPoint(ToBulletVector3(relativePosition)));
    }

    Vector3 RigidBody::GetPushVelocityInPoint(Vector3 relativePosition)
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getPushVelocityInLocalPoint(ToBulletVector3(relativePosition)));
    }

    void RigidBody::ApplyCentralImpulse(Vector3 impulse)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyCentralImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyCentralPushImpulse(Vector3 impulse)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyCentralPushImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyForce(const Vector3& force, const Vector3& relativePosition)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyForce(ToBulletVector3(force), ToBulletVector3(relativePosition));
    }

    void RigidBody::ApplyImpulse(const Vector3& impulse, const Vector3& relativePosition)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyForce(ToBulletVector3(impulse), ToBulletVector3(relativePosition));
    }

    void RigidBody::ApplyPushImpulse(const Vector3& impulse, const Vector3& relativePosition)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyPushImpulse(ToBulletVector3(impulse), ToBulletVector3(relativePosition));
    }

    void RigidBody::ApplyTorque(const Vector3& force)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyTorque(ToBulletVector3(force));
    }

    void RigidBody::ApplyTorqueImpulse(const Vector3& impulse)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyTorqueImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyTorqueTurnImpulse(const Vector3& impulse)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyTorqueTurnImpulse(ToBulletVector3(impulse));
    }

    void RigidBody::ApplyCentralForce(const Vector3& force)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->applyCentralForce(ToBulletVector3(force));
    }

    void RigidBody::SetPushVelocity(Vector3 velocity)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->setPushVelocity(ToBulletVector3(velocity));
    }

    Vector3 RigidBody::GetPushVelocity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getPushVelocity());
    }

    void RigidBody::SetTurnVelocity(Vector3 velocity)
    {
        this->rigidBody->Activate();
        this->rigidBody->GetNativeHandle()->setTurnVelocity(ToBulletVector3(velocity));
    }

    Vector3 RigidBody::GetTurnVelocity() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getTurnVelocity());
    }

    void RigidBody::SetAnisotropicFriction(Vector3 friction, AnisotropicFriction mode)
    {
        this->rigidBody->GetNativeHandle()->setAnisotropicFriction(ToBulletVector3(friction), (int)mode);
    }

    void RigidBody::SetAnisotropicFriction(Vector3 friction)
    {
        this->SetAnisotropicFriction(friction, this->HasAnisotropicFriction() ? AnisotropicFriction::ENABLED : AnisotropicFriction::DISABLED);
    }

    bool RigidBody::HasAnisotropicFriction() const
    {
        return this->rigidBody->GetNativeHandle()->hasAnisotropicFriction();
    }

    void RigidBody::ToggleAnisotropicFriction(bool value)
    {
        this->SetAnisotropicFriction(this->GetAnisotropicFriction(), value ? AnisotropicFriction::ENABLED : AnisotropicFriction::DISABLED);
    }

    Vector3 RigidBody::GetAnisotropicFriction() const
    {
        return FromBulletVector3(this->rigidBody->GetNativeHandle()->getAnisotropicFriction());
    }

    MXENGINE_REFLECT_TYPE
    {
        using SetAnisotropicFrictionFunc = void(RigidBody::*)(Vector3);

        rttr::registration::class_<RigidBody>("RigidBody")
            .constructor<>()
            .method("clear forces", &RigidBody::ClearForces)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .method("activate", &RigidBody::Activate)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("is static", &RigidBody::IsStatic, &RigidBody::SetStateStatic)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is dynamic", &RigidBody::IsDynamic, &RigidBody::SetStateDynamic)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is kinematic", &RigidBody::IsKinematic, &RigidBody::SetStateKinematic)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is trigger", &RigidBody::IsTrigger, &RigidBody::SetStateTrigger)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is ray castable", &RigidBody::IsRayCastable, &RigidBody::ToggleRayCasting)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property_readonly("is moving", &RigidBody::IsMoving)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("total force", &RigidBody::GetTotalForceApplied)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("total torque", &RigidBody::GetTotalTorqueApplied)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("intertia", &RigidBody::GetInertia)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("mass", &RigidBody::GetMass, &RigidBody::SetMass)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.001f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("friction", &RigidBody::GetFriction, &RigidBody::SetFriction)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 1.0f }),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("bounce factor", &RigidBody::GetBounceFactor, &RigidBody::SetBounceFactor)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 1.0f }),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("linear velocity", &RigidBody::GetLinearVelocity, &RigidBody::SetLinearVelocity)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("angular velocity", &RigidBody::GetAngularVelocity, &RigidBody::SetAngularVelocity)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("push velocity", &RigidBody::GetPushVelocity, &RigidBody::SetPushVelocity)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("turn velocity", &RigidBody::GetTurnVelocity, &RigidBody::SetTurnVelocity)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("linear force factor", &RigidBody::GetLinearForceFactor, &RigidBody::SetLinearForceFactor)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("angular force factor", &RigidBody::GetAngularForceFactor, &RigidBody::SetAngularForceFactor)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("linear air resistance", &RigidBody::GetLinearAirResistance, &RigidBody::SetLinearAirResistance)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("angular air resistance", &RigidBody::GetAngularAirResistance, &RigidBody::SetAngularAirResistance)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("has anisotropic friction", &RigidBody::HasAnisotropicFriction, &RigidBody::ToggleAnisotropicFriction)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("anisotropic friction", &RigidBody::GetAnisotropicFriction, (SetAnisotropicFrictionFunc)&RigidBody::SetAnisotropicFriction)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 1.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("gravity", &RigidBody::GetGravity, &RigidBody::SetGravity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("rolling friction", &RigidBody::GetRollingFriction, &RigidBody::SetRollingFriction)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 1.0f }),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("spinning friction", &RigidBody::GetSpinningFriction, &RigidBody::SetSpinningFriction)
            (
                 rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                 rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 1.0f }),
                 rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );
    }
}