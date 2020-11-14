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

#include "Core/Serialization/SceneSerializer.h"
#include "Core/Components/Physics/RigidBody.h"
#include "Core/Components/Physics/BoxCollider.h"
#include "Core/Components/Physics/SphereCollider.h"
#include "Core/Components/Physics/CapsuleCollider.h"
#include "Core/Components/Physics/CylinderCollider.h"
#include "Core/Components/Physics/CompoundCollider.h"
#include "Core/Components/Physics/CharacterController.h"

namespace MxEngine
{
    void Serialize(JsonFile& json, const RigidBody& body)
    {
        json["activation-state"] = body.GetActivationState();
        json["is-kinematic"] = body.IsKinematic();
        json["is-dynamic"] = body.IsDynamic();
        json["is-static"] = body.IsStatic();
        json["is-trigger"] = body.IsTrigger();
        json["is-raycastable"] = body.IsRayCastable();
        json["mass"] = body.GetMass();
        json["friction"] = body.GetFriction();
        json["spinning-friction"] = body.GetSpinningFriction();
        json["rolling-friction"] = body.GetRollingFriction();
        json["bounce-factor"] = body.GetBounceFactor();
        json["gravity"] = body.GetGravity();
        json["linear-velocity"] = body.GetLinearVelocity();
        json["angular-velocity"] = body.GetAngularVelocity();
        json["linear-force-factor"] = body.GetLinearForceFactor();
        json["angular-force-factor"] = body.GetAngularForceFactor();
        json["linear-air-resistance"] = body.GetLinearAirResistance();
        json["angular-air-resistance"] = body.GetAngularAirResistance();
        json["push-velocity"] = body.GetPushVelocity();
        json["turn-velocity"] = body.GetTurnVelocity();
        json["anisotropic-friction"] = body.GetAnisotropicFriction();
        json["has-anisotropic-friction"] = body.HasAnisotropicFriction();
    }

    void Serialize(JsonFile& json, const CharacterController& character)
    {
        json["jump-power"] = character.GetJumpPower();
        json["jump-speed"] = character.GetJumpSpeed();
    }

    void Serialize(JsonFile& json, const BoxShape& box)
    {
        auto b = box.GetBoundingBoxUnchanged();
        json["center"] = b.Center;
        json["half"] = b.Max;
    }

    void Serialize(JsonFile& json, const SphereShape& sphere)
    {
        auto s = sphere.GetBoundingSphereUnchanged();
        json["center"] = s.Center;
        json["radius"] = s.Radius;
    }

    void Serialize(JsonFile& json, const CapsuleShape& capsule)
    {
        auto c = capsule.GetBoundingCapsuleUnchanged();
        json["height"] = c.Height;
        json["radius"] = c.Radius;
        json["orientation"] = c.Orientation;
    }

    void Serialize(JsonFile& json, const CylinderShape& cylinder)
    {
        auto c = cylinder.GetBoundingCylinderUnchanged();
        json["height"] = c.Height;
        json["radiusX"] = c.RadiusX;
        json["radiusZ"] = c.RadiusZ;
        json["orientation"] = c.Orientation;
    }

    void Serialize(JsonFile& json, const BoxCollider& box)
    {
        Serialize(json, *box.GetNativeHandle());
    }

    void Serialize(JsonFile& json, const SphereCollider& sphere)
    {
        Serialize(json, *sphere.GetNativeHandle());
    }

    void Serialize(JsonFile& json, const CapsuleCollider& capsule)
    {
        Serialize(json, *capsule.GetNativeHandle());
    }

    void Serialize(JsonFile& json, const CylinderCollider& cylinder)
    {
        Serialize(json, *cylinder.GetNativeHandle());
    }

    void Serialize(JsonFile& json, const CompoundCollider& compound)
    {
        auto& jShapes = json["shapes"];
        size_t shapeCount = compound.GetShapeCount();
        for (size_t i = 0; i < shapeCount; i++)
        {
            auto& jShape = jShapes.emplace_back();
            Serialize(jShape["transform"], compound.GetShapeTransformByIndex(i));
            
            auto box = compound.GetShapeByIndex<BoxShape>(i);
            auto sphere = compound.GetShapeByIndex<SphereShape>(i);
            auto cylinder = compound.GetShapeByIndex<CylinderShape>(i);
            auto capsule = compound.GetShapeByIndex<CapsuleShape>(i);

            if (box.IsValid())
            {
                Serialize(jShape["box-shape"], *box);
            }
            else if (sphere.IsValid())
            {
                Serialize(jShape["sphere-shape"], *sphere);
            }
            else if (cylinder.IsValid())
            {
                Serialize(jShape["cylinder-shape"], *cylinder);
            }
            else if (capsule.IsValid())
            {
                Serialize(jShape["capsule-shape"], *capsule);
            }
        }
    }
}