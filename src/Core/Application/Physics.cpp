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

#include "Physics.h"
#include "Core/MxObject/MxObject.h"
#include "Utilities/Logging/Logger.h"
#include "Platform/Modules/PhysicsModule.h"
#include "Platform/Bullet3/Bullet3Utils.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
    #define WORLD PhysicsModule::GetImpl()->World

    struct CustomRayCastCallback : public btCollisionWorld::ClosestRayResultCallback
    {
        CustomRayCastCallback(const btVector3& from, const btVector3& to)
            : btCollisionWorld::ClosestRayResultCallback(from, to)
        {
            this->m_collisionFilterGroup = CollisionGroup::DEFAULT;
            this->m_collisionFilterMask = CollisionMask::ALL;
        }

        MxObject::Handle GetResult() const
        {
            if (!this->hasHit()) return MxObject::Handle{ };
            return Physics::GetRigidBodyParent(this->m_collisionObject);
        }

        float GetRayLength() const
        {
            return this->m_closestHitFraction * (this->m_rayFromWorld - this->m_rayToWorld).length();
        }
    };

    void Physics::AddRigidBody(void* body) //-V813
    {
        WORLD->addRigidBody((btRigidBody*)body);
    }

    void Physics::AddRigidBody(void* body, int group, int mask)
    {
        WORLD->addRigidBody((btRigidBody*)body, group, mask);
    }

    void Physics::RemoveRigidBody(void* body) //-V813
    {
        WORLD->removeRigidBody((btRigidBody*)body);
    }

    void Physics::SetRigidBodyParent(void* body, MxObject& parent)
    {
        auto objectHandle = parent.GetNativeHandle();
        ((btRigidBody*)body)->setUserPointer(reinterpret_cast<void*>(objectHandle));
    }

    MxObject::Handle Physics::GetRigidBodyParent(const void* body)
    {
        auto objectHandle = reinterpret_cast<MxObject::EngineHandle>(((btRigidBody*)body)->getUserPointer());
        return MxObject::GetByHandle(objectHandle);
    }

    MxObject::Handle Physics::RayCast(const Vector3& from, const Vector3& to)
    {
        float rayDistance = 0.0f;
        return Physics::RayCast(from, to, rayDistance);
    }

    MxObject::Handle Physics::RayCast(const Vector3& from, const Vector3& to, float& rayDistance)
    {
        auto btFrom = ToBulletVector3(from);
        auto btTo = ToBulletVector3(to);

        CustomRayCastCallback callback(btFrom, btTo);

        WORLD->rayTest(btFrom, btTo, callback);
        rayDistance = callback.GetRayLength();
        return callback.GetResult();
    }

    void Physics::SetGravity(const Vector3& gravity)
    {
        WORLD->setGravity(ToBulletVector3(gravity));
    }

    void Physics::PerformExtraSimulationStep(float timeDelta)
    {
        MAKE_SCOPE_PROFILER("Physics::SimulationStep()");
        WORLD->stepSimulation(timeDelta);
    }

    void Physics::SetSimulationStep(float timeDelta)
    {
        PhysicsModule::SetSimulationStep(timeDelta);
    }
}