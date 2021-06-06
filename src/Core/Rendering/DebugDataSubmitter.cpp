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

#include "DebugDataSubmitter.h"
#include "Core/Components/Components.h"
#include "Core/Rendering/RenderObjects/DebugBuffer.h"
#include "Core/BoundingObjects/Cone.h"
#include "Core/BoundingObjects/Frustrum.h"

namespace MxEngine
{
    void Process(DebugBuffer& buffer, const DebugDraw& debugDraw, MxObject& object, const MeshSource::Handle& meshSource)
    {
        if (meshSource.IsValid())
        {
            if (debugDraw.RenderBoundingBox)
            {
                for (const auto& submesh : meshSource->Mesh->GetSubMeshes())
                {
                    auto box = submesh.GetAABB() * (object.LocalTransform.GetMatrix() * submesh.GetTransform().GetMatrix());
                    buffer.Submit(box, debugDraw.BoundingBoxColor);
                }
            }
            if (debugDraw.RenderBoundingSphere)
            {
                for (const auto& submesh : meshSource->Mesh->GetSubMeshes())
                {
                    auto sphere = submesh.GetBoundingSphere();
                    sphere.Center += object.LocalTransform.GetPosition() + submesh.GetTransform().GetPosition();
                    sphere.Radius *= ComponentMax(object.LocalTransform.GetScale() * submesh.GetTransform().GetScale());
                    buffer.Submit(sphere, debugDraw.BoundingSphereColor);
                }
            }
        }
    }

    void Process(DebugBuffer& buffer, const DebugDraw& debugDraw, MxObject& object, const PointLight::Handle& pointLight)
    {
        if (debugDraw.RenderLightingBounds && pointLight.IsValid())
        {
            BoundingSphere sphere(object.LocalTransform.GetPosition(), pointLight->GetRadius());
            buffer.Submit(sphere, debugDraw.LightSourceColor);
        }
    }

    void Process(DebugBuffer& buffer, const DebugDraw& debugDraw, MxObject& object, const SpotLight::Handle& spotLight)
    {
        if (debugDraw.RenderLightingBounds && spotLight.IsValid())
        {
            Cone cone(object.LocalTransform.GetPosition(), spotLight->Direction, 3.0f, spotLight->GetOuterAngle());
            buffer.Submit(cone, debugDraw.LightSourceColor);
        }
    }

    void Process(DebugBuffer& buffer, const DebugDraw& debugDraw, MxObject& object, const AudioSource::Handle& audioSource)
    {
        if (debugDraw.RenderSoundBounds && audioSource.IsValid())
        {
            if (!audioSource->IsRelative())
            {
                if (audioSource->IsOmnidirectional())
                {
                    BoundingSphere sphere(object.LocalTransform.GetPosition(), 3.0f);
                    buffer.Submit(sphere, debugDraw.SoundSourceColor);
                }
                else
                {
                    Cone cone(object.LocalTransform.GetPosition(), audioSource->GetDirection(), 3.0f, audioSource->GetOuterAngle());
                    buffer.Submit(cone, debugDraw.SoundSourceColor);
                }
            }
        }
    }

    void Process(DebugBuffer& buffer, const DebugDraw& debugDraw, MxObject& object, const CameraController::Handle& cameraController)
    {
        if (debugDraw.RenderFrustrumBounds && cameraController.IsValid())
        {
            auto direction = cameraController->GetDirection();
            auto up = cameraController->GetDirectionUp();
            auto aspect = cameraController->Camera.GetAspectRatio();
            auto zoom = cameraController->Camera.GetZoom() * 65.0f;
            Frustrum frustrum(object.LocalTransform.GetPosition() + Normalize(direction), direction, up, zoom, aspect);
            buffer.Submit(frustrum, debugDraw.FrustrumColor);
        }
    }

    void Process(DebugBuffer& buffer, const DebugDraw& debugDraw, MxObject& object, const RigidBody::Handle& rigidBody)
    {
        if (rigidBody.IsValid() && debugDraw.RenderPhysicsCollider)
        {
            auto boxCollider = object.GetComponent<BoxCollider>();
            auto sphereCollider = object.GetComponent<SphereCollider>();
            auto cylinderCollider = object.GetComponent<CylinderCollider>();
            auto capsuleCollider = object.GetComponent<CapsuleCollider>();
            auto compoundCollider = object.GetComponent<CompoundCollider>();

            if (boxCollider.IsValid())
                buffer.Submit(boxCollider->GetBoundingBox(), debugDraw.BoundingBoxColor);
            if (sphereCollider.IsValid())
                buffer.Submit(sphereCollider->GetBoundingSphere(), debugDraw.BoundingSphereColor);
            if (cylinderCollider.IsValid())
                buffer.Submit(cylinderCollider->GetBoundingCylinder(), debugDraw.BoundingBoxColor);
            if (capsuleCollider.IsValid())
                buffer.Submit(capsuleCollider->GetBoundingCapsule(), debugDraw.BoundingSphereColor);

            if (compoundCollider.IsValid())
            {
                for (size_t i = 0; i < compoundCollider->GetShapeCount(); i++)
                {
                    auto& child = compoundCollider->GetShapeByIndex(i);
                    auto childTransform = compoundCollider->GetShapeTransformByIndex(i);
                    childTransform.SetPosition(childTransform.GetPosition() * object.LocalTransform.GetScale());

                    std::visit([&buffer, &debugDraw, transform = childTransform * object.LocalTransform](auto&& shape) mutable
                    {
                        buffer.Submit(shape->GetNativeBoundingTransformed(transform), debugDraw.BoundingBoxColor);
                    }, child);
                }
            }
        }
    }

    void DebugDataSubmitter::ProcessObject(MxObject& object)
    {
        auto debugDrawHandle = object.GetComponent<DebugDraw>();
        if (!debugDrawHandle.IsValid()) return;
        auto& debugDraw = *debugDrawHandle.GetUnchecked();

        auto instanceFactory = object.GetComponent<InstanceFactory>();
        auto instance = object.GetComponent<Instance>();
        auto meshSource = object.GetComponent<MeshSource>();
        auto spotLight = object.GetComponent<SpotLight>();
        auto pointLight = object.GetComponent<PointLight>();
        auto cameraController = object.GetComponent<CameraController>();
        auto audioSource = object.GetComponent<AudioSource>();
        auto rigidBody = object.GetComponent<RigidBody>();

        if (instance.IsValid()) meshSource = instance->GetParent()->GetComponent<MeshSource>();
        if (instanceFactory.IsValid()) meshSource = { }; // do not show meshSource for factory objects

        Process(this->debug, debugDraw, object, meshSource);
        Process(this->debug, debugDraw, object, pointLight);
        Process(this->debug, debugDraw, object, spotLight);
        Process(this->debug, debugDraw, object, audioSource);
        Process(this->debug, debugDraw, object, cameraController);
        Process(this->debug, debugDraw, object, rigidBody);
    }
}