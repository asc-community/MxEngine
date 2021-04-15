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

#include "Bullet3/BoxShape.h"
#include "Bullet3/SphereShape.h"
#include "Bullet3/CylinderShape.h"
#include "Bullet3/CapsuleShape.h"
#include "Bullet3/CompoundShape.h"
#include "Bullet3/NativeRigidBody.h"
#include "Utilities/Factory/Factory.h"

namespace MxEngine
{
    MXENGINE_MAKE_FACTORY(BoxShape);
    MXENGINE_MAKE_FACTORY(SphereShape);
    MXENGINE_MAKE_FACTORY(CylinderShape);
    MXENGINE_MAKE_FACTORY(CapsuleShape);
    MXENGINE_MAKE_FACTORY(CompoundShape);
    MXENGINE_MAKE_FACTORY(NativeRigidBody);

    // Physics: how to add new collider
    //
    //
    // Create new bounding object with all desired data for collider
    // 
    // Create class <ColliderType>Shape (copy-past from any other existing shape)
    // 
    // add MXENGINE_MAKE_FACTORY macro above
    //
    // create component <ColliderType>Collider (copy-past from any other existing shape)
    //
    // add tests for new collider in RigidBody.cpp in Init() and UpdateCollider() methods
    //
    // add Submit method in DebugDrawer to draw collider in game
    //
    // add component to Components.h
    //
    // add test for collider in RenderAdaptor.cpp DebugDraw component case
    //
    // add editor for collider component, editor for collider and register callbacks in runtime editor
    //
    // add check to compound collider if collider can be part of other collider
}