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

#pragma once

#include "Utilities/Math/Math.h"
#include "Core/Components/Transform.h"

class btRigidBody;
class btCollisionShape;
class btMotionState;

namespace MxEngine
{
    namespace CollisionMask
    {
        enum Mask : uint32_t
        {
            GHOST = 0,
            STATIC = 1,
            KINEMATIC = 1 << 1,
            DYNAMIC  = 1 << 2,
            RAYCAST_ONLY = 1 << 3,
        };
    }

    namespace CollisionGroup
    {
        enum Group : uint32_t
        {  
            NONE = 0,
            RAYCAST_ONLY = CollisionMask::RAYCAST_ONLY,
            DYNAMIC_STATIC_KINEMATIC = CollisionMask::DYNAMIC | CollisionMask::STATIC | CollisionMask::KINEMATIC,
            ALL_NO_RAYCAST = DYNAMIC_STATIC_KINEMATIC,
            ALL = ALL_NO_RAYCAST | CollisionGroup::RAYCAST_ONLY,
            NO_STATIC_COLLISIONS = ALL & ~CollisionMask::STATIC & ~CollisionMask::KINEMATIC,
            NO_STATIC_COLLISIONS_NO_RAYCAST = NO_STATIC_COLLISIONS & ~CollisionMask::RAYCAST_ONLY,
        };
    }

    enum class ActivationState
    {
        ACTIVE_TAG = 1,
        ISLAND_SLEEPING = 2,
        WANTS_DEACTIVATION = 3,
        DISABLE_DEACTIVATION = 4,
        DISABLE_SIMULATION = 5,
    };

    class NativeRigidBody
    {
        struct RigidBodyAllocation;
        RigidBodyAllocation* bodyAllocation = nullptr;
        
        uint32_t group = CollisionGroup::NO_STATIC_COLLISIONS;
        uint32_t mask = CollisionMask::STATIC;

        void DestroyBody();
        void ReAddRigidBody();
        void UpdateRigidBodyCollider(float mass, btCollisionShape* collider);
    public:
        NativeRigidBody(const Transform& transform);
        NativeRigidBody(const NativeRigidBody&) = delete;
        NativeRigidBody(NativeRigidBody&&) noexcept;
        NativeRigidBody& operator=(const NativeRigidBody&) = delete;
        NativeRigidBody& operator=(NativeRigidBody&&) noexcept;
        ~NativeRigidBody();

        btRigidBody* GetNativeHandle();
        const btRigidBody* GetNativeHandle() const;
        btMotionState* GetMotionState();
        const btMotionState* GetMotionState() const;
        bool HasTransformUpdate() const;
        void SetTransformUpdateFlag(bool value);

        btCollisionShape* GetCollisionShape();
        const btCollisionShape* GetCollisionShape() const;
        void SetCollisionShape(btCollisionShape* shape);
        void SetCollisionFilter(uint32_t group, uint32_t mask);
        uint32_t GetCollisionGroup() const;
        uint32_t GetCollisionMask() const;
        Vector3 GetScale() const;
        bool IsMoving() const;
        bool HasCollisionResponce() const;
        void SetKinematicFlag();
        void UnsetKinematicFlag();
        void SetTriggerFlag();
        void UnsetTriggerFlag();
        void UnsetAllFlags();
        void SetScale(const Vector3& scale);
        float GetMass() const;
        void SetMass(float mass);
        void SetActivationState(ActivationState state);
        ActivationState GetActivationState() const;
        void Activate();
        bool IsActive() const;
    };
}