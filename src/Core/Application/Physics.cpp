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
#include "Utilities/Logging/Logger.h"
#include "Platform/Modules/PhysicsModule.h"
#include "Platform/Bullet3/Bullet3Utils.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
    #define WORLD PhysicsModule::GetImpl()->World

    void Physics::AddRigidBody(void* body) //-V813
    {
        WORLD->addRigidBody((btRigidBody*)body);
    }

    void Physics::RemoveRigidBody(void* body) //-V813
    {
        WORLD->removeRigidBody((btRigidBody*)body);
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