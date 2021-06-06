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

#include "CharacterController.h"
#include "Core/Components/Camera/CameraController.h"
#include "Core/Components/Camera/InputController.h"
#include "Core/Components/Physics/RigidBody.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Application/Event.h"
#include "Core/Application/Physics.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    bool CharacterController::AreAllComponentsPresent() const
    {
        auto& object = MxObject::GetByComponent(*this);
        if (!object.HasComponent<CameraController>()) return false;
        if (!object.HasComponent<RigidBody>()) return false;
        if (!object.HasComponent<InputController>()) return false;

        return true;
    }

    const Vector3& CharacterController::GetMotionVector() const
    {
        return MxObject::GetByComponent(*this).GetComponent<InputController>()->GetMotionVector();
    }

    bool CheckIfPlayerIsOnGround(const Vector3& playerPosition, const Vector3 maxDistanceToGround)
    {
        float fraction = 0.0f;
        auto rayCastMask = CollisionMask::Mask(CollisionMask::STATIC | CollisionMask::KINEMATIC | CollisionMask::DYNAMIC);
        auto rayCastResult = Physics::RayCast(playerPosition, playerPosition - maxDistanceToGround, fraction, rayCastMask);
        // if we hit object and it is not a trigger, we stand on ground
        return rayCastResult.IsValid() && !rayCastResult->GetComponent<RigidBody>()->IsTrigger();
    }

    void CharacterController::OnUpdate(float dt)
    {
        if (!this->AreAllComponentsPresent()) return;
        auto& self = MxObject::GetByComponent(*this);

        auto& rigidBody = *self.GetComponent<RigidBody>();
        auto& camera = *self.GetComponent<CameraController>();
        
        // verify rigid body parameters
        if (rigidBody.GetAngularForceFactor() != MakeVector3(0.0f)) rigidBody.SetAngularForceFactor(MakeVector3(0.0f));
        if (rigidBody.GetBounceFactor() != 0.0f) rigidBody.SetBounceFactor(0.0f);
        if (rigidBody.GetFriction() != 0.0f) rigidBody.SetFriction(0.0f);
        if (!rigidBody.IsDynamic()) rigidBody.MakeDynamic();
        
        auto colliderSize = VectorMax(rigidBody.GetAABB().Length() * 0.51f, MakeVector3(0.1f));
        this->isGrounded = CheckIfPlayerIsOnGround(self.LocalTransform.GetPosition(), colliderSize * camera.GetUpVector());

        // update rigid body position
        auto motion = this->GetMotionVector();
        auto gravity = rigidBody.GetGravity();
        if (this->IsGrounded())
        {
            auto jumpImpulse = motion * camera.GetUpVector() * this->GetJumpPower();
            motion -= motion * camera.GetUpVector();
            rigidBody.SetLinearVelocity(motion * camera.GetMoveSpeed() + jumpImpulse + gravity * dt);
        }
        else
        {
            auto velocity = rigidBody.GetLinearVelocity(); 
            motion -= motion * camera.GetUpVector();

            rigidBody.SetLinearVelocity(velocity + motion * this->GetJumpSpeed() * dt + gravity * dt);
        }
    }

    void CharacterController::SetJumpPower(float power)
    {
        this->jumpPower = Max(power, 0.0f);
    }

    void CharacterController::SetJumpSpeed(float speed)
    {
        this->jumpSpeed = Max(speed, 0.0f);
    }

    float CharacterController::GetJumpPower() const
    {
        return this->jumpPower;
    }

    float CharacterController::GetJumpSpeed() const
    {
        return this->jumpSpeed;
    }

    bool CharacterController::IsGrounded() const
    {
        return this->isGrounded;
    }

    Vector3 CharacterController::GetCurrentMotion() const
    {
        auto rigidBody = MxObject::GetByComponent(*this).GetComponent<RigidBody>();
        return rigidBody.IsValid() ? rigidBody->GetLinearVelocity() : MakeVector3(0.0f);
    }

    float CharacterController::GetMoveSpeed() const
    {
        auto camera = MxObject::GetByComponent(*this).GetComponent<CameraController>();
        return camera.IsValid() ? camera->GetMoveSpeed() : 0.0f;
    }

    void CharacterController::SetMoveSpeed(float speed)
    {
        auto camera = MxObject::GetByComponent(*this).GetComponent<CameraController>();
        if (camera.IsValid()) camera->SetMoveSpeed(speed);
    }

    float CharacterController::GetRotateSpeed() const
    {
        auto camera = MxObject::GetByComponent(*this).GetComponent<CameraController>();
        return camera.IsValid() ? camera->GetRotateSpeed() : 0.0f;
    }

    float CharacterController::GetMass() const
    {
        auto rigidBody = MxObject::GetByComponent(*this).GetComponent<RigidBody>();
        return rigidBody.IsValid() ? rigidBody->GetMass() : 0.0f;
    }

    void CharacterController::SetRotateSpeed(float speed)
    {
        auto camera = MxObject::GetByComponent(*this).GetComponent<CameraController>();
        if (camera.IsValid()) camera->SetRotateSpeed(speed);
    }

    void CharacterController::SetMass(float mass)
    {
        auto rigidBody = MxObject::GetByComponent(*this).GetComponent<RigidBody>();
        if (rigidBody.IsValid()) rigidBody->SetMass(Max(mass, 0.001f));
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<CharacterController>("CharacterController")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property_readonly("is grounded", &CharacterController::IsGrounded)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("jump power", &CharacterController::GetJumpPower, &CharacterController::SetJumpPower)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("jump speed", &CharacterController::GetJumpSpeed, &CharacterController::SetJumpSpeed)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("mass", &CharacterController::GetMass, &CharacterController::SetMass)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.001f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("move speed", &CharacterController::GetMoveSpeed, &CharacterController::SetMoveSpeed)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("rotate speed", &CharacterController::GetRotateSpeed, &CharacterController::SetRotateSpeed)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );
    }
}