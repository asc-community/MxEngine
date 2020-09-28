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

#include "RenderAdaptor.h"
#include "Library/Primitives/Colors.h"
#include "Library/Primitives/Primitives.h"
#include "Core/Config/GlobalConfig.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshLOD.h"
#include "Core/Components/Rendering/DebugDraw.h"
#include "Core/Components/Audio/AudioSource.h"
#include "Core/Components/Physics/BoxCollider.h"
#include "Core/Components/Physics/SphereCollider.h"
#include "Core/Components/Physics/CylinderCollider.h"
#include "Core/Components/Physics/CapsuleCollider.h"
#include "Core/Components/Physics/RigidBody.h"
#include "Core/Components/Instancing/InstanceFactory.h"
#include "Core/Components/Lighting/DirectionalLight.h"
#include "Core/Components/Lighting/SpotLight.h"
#include "Core/Components/Lighting/PointLight.h"
#include "Core/Components/Camera/CameraEffects.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Core/BoundingObjects/Cone.h"
#include "Core/BoundingObjects/Frustrum.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
    void RenderAdaptor::InitRendererEnvironment()
    {
        MAKE_SCOPE_PROFILER("RenderAdaptor::InitEnvironment()");
        auto& environment = this->Renderer.GetEnvironment();

        this->SetRenderToDefaultFrameBuffer();
        this->SetShadowBlurIterations(1);
        
        // fog
        this->SetFogColor(MakeVector3(0.5f, 0.6f, 0.7f));
        this->SetFogDistance(1.0f);
        this->SetFogDensity(0.001f);

        // helper objects
        environment.RectangularObject.Init(1.0f);
        environment.SkyboxCubeObject.Init();
        this->DebugDrawer.Init();
        environment.DebugBufferObject.VAO = this->DebugDrawer.GetVAO();

        // light bounding objects
        auto pyramid = Primitives::CreatePyramid();
        auto& pyramidMesh = pyramid->GetSubmeshes().front();
        this->Renderer.GetLightInformation().PyramidLight =
            RenderHelperObject(pyramidMesh.Data.GetVBO(), pyramidMesh.Data.GetVAO(), pyramidMesh.Data.GetIBO());

        auto sphere = Primitives::CreateSphere(8);
        auto& sphereMesh = sphere->GetSubmeshes().front();
        this->Renderer.GetLightInformation().SphereLight =
            RenderHelperObject(sphereMesh.Data.GetVBO(), sphereMesh.Data.GetVAO(), sphereMesh.Data.GetIBO());

        auto pyramidInstanced = Primitives::CreatePyramid();
        auto& pyramidInstancedMesh = pyramidInstanced->GetSubmeshes().front();
        this->Renderer.GetLightInformation().SpotLightsInstanced = 
            SpotLightInstancedObject(pyramidInstancedMesh.Data.GetVBO(), pyramidInstancedMesh.Data.GetVAO(), pyramidInstancedMesh.Data.GetIBO());

        auto sphereInstanced = Primitives::CreateSphere(8);
        auto& sphereInstancedMesh = sphereInstanced->GetSubmeshes().front();
        this->Renderer.GetLightInformation().PointLigthsInstanced =
            PointLightInstancedObject(sphereInstancedMesh.Data.GetVBO(), sphereInstancedMesh.Data.GetVAO(), sphereInstancedMesh.Data.GetIBO());

        // default textures
        environment.DefaultBlackMap = Colors::MakeTexture(Colors::BLACK);
        environment.DefaultNormalMap = Colors::MakeTexture(Colors::FLAT_NORMAL);
        environment.DefaultMaterialMap = Colors::MakeTexture(Colors::WHITE);
        environment.DefaultGreyMap = Colors::MakeTexture(Colors::GREY);
        environment.DefaultBlackCubeMap = Colors::MakeCubeMap(Colors::BLACK);

        environment.DefaultBlackMap->SetPath("[[black color]]");
        environment.DefaultNormalMap->SetPath("[[default normal]]");
        environment.DefaultMaterialMap->SetPath("[[white color]]");
        environment.DefaultGreyMap->SetPath("[[grey color]]");

        environment.AverageWhiteTexture = GraphicFactory::Create<Texture>();
        environment.AverageWhiteTexture->Load(nullptr, (int)GlobalConfig::GetBloomTextureSize(), (int)GlobalConfig::GetBloomTextureSize(), HDRTextureFormat);
        environment.AverageWhiteTexture->SetSamplingFromLOD(environment.AverageWhiteTexture->GetMaxTextureLOD());
        environment.AverageWhiteTexture->SetPath("[[average white]]");

        // shaders
        environment.Shaders["GBuffer"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["GBuffer"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/gbuffer_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/gbuffer_fragment.glsl"
        );

        environment.Shaders["Transparent"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["Transparent"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/gbuffer_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/transparent_fragment.glsl"
        );

        environment.Shaders["GlobalIllumination"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["GlobalIllumination"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/global_illum_fragment.glsl"
        );

        environment.Shaders["SpotLight"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["SpotLight"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/spotlight_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/spotlight_fragment.glsl"
        );

        environment.Shaders["PointLight"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["PointLight"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/pointlight_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/pointlight_fragment.glsl"
        );

        environment.Shaders["HDRToLDR"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["HDRToLDR"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/hdr_to_ldr_fragment.glsl"
        );

        environment.Shaders["FXAA"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["FXAA"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/fxaa_fragment.glsl"
        );

        environment.Shaders["Fog"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["Fog"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/fog_fragment.glsl"
        );

        environment.Shaders["Vignette"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["Vignette"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/vignette_fragment.glsl"
        );

        environment.Shaders["Skybox"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["Skybox"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/skybox_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/skybox_fragment.glsl"
        );

        environment.Shaders["DepthTexture"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["DepthTexture"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/depthtexture_vertex.glsl"
            , 
            #include "Platform/OpenGL/Shaders/depthtexture_fragment.glsl"
        );

        environment.Shaders["DepthCubeMap"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["DepthCubeMap"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/depthcubemap_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/depthcubemap_geometry.glsl"
            ,
            #include "Platform/OpenGL/Shaders/depthcubemap_fragment.glsl"
        );

        environment.Shaders["BloomIteration"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["BloomIteration"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/bloom_iter_fragment.glsl"
        );

        environment.Shaders["BloomSplit"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["BloomSplit"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/bloom_split_fragment.glsl"
        );

        environment.Shaders["ImageForward"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["ImageForward"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/rect_fragment.glsl"
        );

        environment.Shaders["DebugDraw"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["DebugDraw"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/debug_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/debug_fragment.glsl"
        );

        environment.Shaders["AverageWhite"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["AverageWhite"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/average_white_fragment.glsl"
        );

        environment.Shaders["SSR"_id] = GraphicFactory::Create<Shader>();
        environment.Shaders["SSR"_id]->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            #include "Platform/OpenGL/Shaders/ssr_fragment.glsl"
        );

        // framebuffers
        environment.DepthFrameBuffer = GraphicFactory::Create<FrameBuffer>();
        environment.DepthFrameBuffer->UseOnlyDepth();
        environment.PostProcessFrameBuffer = GraphicFactory::Create<FrameBuffer>();

        auto bloomBufferSize = (int)GlobalConfig::GetBloomTextureSize();
        for (auto& bloomBuffer : environment.BloomBuffers)
        {
            auto bloomTexture = GraphicFactory::Create<Texture>();
            bloomTexture->Load(nullptr, bloomBufferSize, bloomBufferSize, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
            bloomTexture->SetPath("[[bloom]]");

            bloomBuffer = GraphicFactory::Create<FrameBuffer>();
            bloomBuffer->AttachTexture(bloomTexture, Attachment::COLOR_ATTACHMENT0);
            bloomBuffer->Validate();
        }
    }

    void RenderAdaptor::RenderFrame()
    {
        this->Renderer.ResetPipeline();

        auto& environment = this->Renderer.GetEnvironment();
        environment.MainCameraIndex = std::numeric_limits<decltype(environment.MainCameraIndex)>::max();
        auto viewportPosition = MakeVector3(0.0f);
        float viewportZoom = 0.0f;
        if (this->Viewport.IsValid())
        {
            viewportPosition = MxObject::GetByComponent(*this->Viewport).Transform.GetPosition();
            viewportZoom = this->Viewport->Camera.GetZoom();
        }

        auto TrackMainCameraIndex = [this, mainCameraIndex = 0, &environment](const CameraController& camera) mutable
        {
            auto cameraComponent = MxObject::GetByComponent(camera).GetComponent<CameraController>();
            if (cameraComponent == this->Viewport)
            {
                environment.MainCameraIndex = mainCameraIndex;
            }
            else
            {
                mainCameraIndex++;
            }
        };

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitCameras()");
            auto cameraView = ComponentFactory::GetView<CameraController>();
            for (const auto& camera : cameraView)
            {
                auto& object = MxObject::GetByComponent(camera);
                auto& transform = object.Transform;
                auto skyboxComponent = object.GetComponent<Skybox>();
                auto effectsComponent = object.GetComponent<CameraEffects>();
                Skybox skybox = skyboxComponent.IsValid() ? *skyboxComponent.GetUnchecked() : Skybox();
                CameraEffects effects = effectsComponent.IsValid() ? *effectsComponent.GetUnchecked() : CameraEffects();
                this->Renderer.SubmitCamera(camera, transform, skybox, effects);
                TrackMainCameraIndex(camera);
            }
        }

        // submit render units
        auto meshSourceView = ComponentFactory::GetView<MeshSource>();
        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitMeshPrimitives()");
            for (const auto& meshSource : meshSourceView)
            {
                auto& object = MxObject::GetByComponent(meshSource);
                auto& transform = object.Transform;
                auto meshRenderer = object.GetComponent<MeshRenderer>();
                auto meshLOD = object.GetComponent<MeshLOD>();
                auto instances = object.GetComponent<InstanceFactory>();

                size_t instanceCount = 0;
                if (instances.IsValid()) instanceCount = instances->GetCount();
                auto mesh = meshSource.Mesh;

                if (!meshSource.IsDrawn || !meshRenderer.IsValid()) continue;

                // we do not try to use LODs for instanced objects, as its quite hard and time consuming. TODO: fix this
                if (meshLOD.IsValid() && instanceCount == 0)
                {
                    meshLOD->FixBestLOD(viewportPosition, viewportZoom);
                    mesh = meshLOD->GetMeshLOD();
                }

                auto& submeshes = mesh->GetSubmeshes();
                for (const auto& submesh : submeshes)
                {
                    auto materialId = submesh.GetMaterialId();
                    if (materialId >= meshRenderer->Materials.size()) continue;
                    auto material = meshRenderer->Materials[materialId];

                    this->Renderer.SubmitPrimitive(submesh, *material, transform, instanceCount);
                }
            }
        }

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitLightSources()");
            auto dirLightView = ComponentFactory::GetView<DirectionalLight>();
            for (const auto& dirLight : dirLightView)
            {
                auto& transform = MxObject::GetByComponent(dirLight).Transform;
                this->Renderer.SubmitLightSource(dirLight, transform);
            }

            auto spotLightView = ComponentFactory::GetView<SpotLight>();
            for (const auto& spotLight : spotLightView)
            {
                auto& transform = MxObject::GetByComponent(spotLight).Transform;
                this->Renderer.SubmitLightSource(spotLight, transform);
            }

            auto pointLightView = ComponentFactory::GetView<PointLight>();
            for (const auto& pointLight : pointLightView)
            {
                auto& transform = MxObject::GetByComponent(pointLight).Transform;
                this->Renderer.SubmitLightSource(pointLight, transform);
            }
        }

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitDebugData()");

            auto debugDrawView = ComponentFactory::GetView<DebugDraw>();
            for (auto& debugDraw : debugDrawView)
            {
                auto& object = MxObject::GetByComponent(debugDraw);
                auto instanceFactory = object.GetComponent<InstanceFactory>();
                auto instance = object.GetComponent<Instance>();
                auto meshSource = object.GetComponent<MeshSource>();
                auto spotLight = object.GetComponent<SpotLight>();
                auto pointLight = object.GetComponent<PointLight>();
                auto cameraController = object.GetComponent<CameraController>();
                auto audioSource = object.GetComponent<AudioSource>();
                auto boxCollider = object.GetComponent<BoxCollider>();
                auto sphereCollider = object.GetComponent<SphereCollider>();
                auto cylinderCollider = object.GetComponent<CylinderCollider>();
                auto capsuleCollider = object.GetComponent<CapsuleCollider>();
                auto rigidBody = object.GetComponent<RigidBody>();

                if(instance.IsValid()) meshSource = instance->GetParent()->GetComponent<MeshSource>();

                if (meshSource.IsValid() && !instanceFactory.IsValid())
                {
                    if (debugDraw.RenderBoundingBox)
                    {
                        auto box = meshSource->Mesh->GetBoundingBox() * object.Transform.GetMatrix();
                        this->DebugDrawer.Submit(box, debugDraw.BoundingBoxColor);
                    }
                    if (debugDraw.RenderBoundingSphere)
                    {
                        auto sphere = meshSource->Mesh->GetBoundingSphere();
                        sphere.Center += object.Transform.GetPosition();
                        sphere.Radius *= ComponentMax(object.Transform.GetScale());
                        this->DebugDrawer.Submit(sphere, debugDraw.BoundingSphereColor);
                    }
                }
                if (debugDraw.RenderLightingBounds && pointLight.IsValid())
                {
                    BoundingSphere sphere(object.Transform.GetPosition(), pointLight->GetRadius());
                    this->DebugDrawer.Submit(sphere, debugDraw.LightSourceColor);
                }
                if (debugDraw.RenderLightingBounds && spotLight.IsValid())
                {
                    Cone cone(object.Transform.GetPosition(), spotLight->Direction, 3.0f, spotLight->GetOuterAngle());
                    this->DebugDrawer.Submit(cone, debugDraw.LightSourceColor);
                }
                if (debugDraw.RenderSoundBounds && audioSource.IsValid())
                {
                    if (!audioSource->IsRelative())
                    {
                        if (audioSource->IsOmnidirectional())
                        {
                            BoundingSphere sphere(object.Transform.GetPosition(), 3.0f);
                            this->DebugDrawer.Submit(sphere, debugDraw.SoundSourceColor);
                        }
                        else
                        {
                            Cone cone(object.Transform.GetPosition(), audioSource->GetDirection(), 3.0f, audioSource->GetOuterAngle());
                            this->DebugDrawer.Submit(cone, debugDraw.SoundSourceColor);
                        }
                    }
                }
                if (debugDraw.RenderFrustrumBounds && cameraController.IsValid())
                {
                    auto direction = cameraController->GetDirection();
                    auto up = cameraController->GetDirectionUp();
                    auto aspect = cameraController->Camera.GetAspectRatio();
                    auto zoom = cameraController->Camera.GetZoom() * 65.0f;
                    Frustrum frustrum(object.Transform.GetPosition() + Normalize(direction), direction, up, zoom, aspect);
                    this->DebugDrawer.Submit(frustrum, debugDraw.FrustrumColor);
                }
                if (rigidBody.IsValid() && debugDraw.RenderPhysicsCollider)
                {
                    if (boxCollider.IsValid())
                        this->DebugDrawer.Submit(boxCollider->GetBoundingBox(), debugDraw.BoundingBoxColor);
                    if (sphereCollider.IsValid())
                        this->DebugDrawer.Submit(sphereCollider->GetBoundingSphere(), debugDraw.BoundingSphereColor);
                    if (cylinderCollider.IsValid())
                        this->DebugDrawer.Submit(cylinderCollider->GetBoundingCylinder(), debugDraw.BoundingBoxColor);
                    if (capsuleCollider.IsValid())
                        this->DebugDrawer.Submit(capsuleCollider->GetBoundingCapsule(), debugDraw.BoundingSphereColor);
                }
            }
            environment.DebugBufferObject.VertexCount = this->DebugDrawer.GetSize();
            environment.OverlayDebugDraws = this->DebugDrawer.DrawAsScreenOverlay;
            this->DebugDrawer.SubmitBuffer();
            this->DebugDrawer.ClearBuffer();

            environment.TimeDelta = Time::Delta();
        }

        this->Renderer.StartPipeline();
    }

    void RenderAdaptor::SubmitRenderedFrame()
    {
        this->Renderer.EndPipeline();
        this->Renderer.Render();
    }

    void RenderAdaptor::SetWindowSize(const VectorInt2& size)
    {
        this->Renderer.GetEnvironment().Viewport = size;
    }

    void RenderAdaptor::SetRenderToDefaultFrameBuffer(bool value)
    {
        this->Renderer.GetEnvironment().RenderToDefaultFrameBuffer = value;
    }

    bool RenderAdaptor::IsRenderedToDefaultFrameBuffer() const
    {
        return this->Renderer.GetEnvironment().RenderToDefaultFrameBuffer;
    }
        
    void RenderAdaptor::SetFogColor(const Vector3& color)
    {
        this->Renderer.GetEnvironment().FogColor = Clamp(color, MakeVector3(0.0f), MakeVector3(1.0f));
    }

    const Vector3& RenderAdaptor::GetFogColor() const
    {
        return this->Renderer.GetEnvironment().FogColor;
    }

    void RenderAdaptor::SetFogDensity(float density)
    {
        this->Renderer.GetEnvironment().FogDensity = Max(0.0f, density);
    }

    float RenderAdaptor::GetFogDensity() const
    {
        return this->Renderer.GetEnvironment().FogDensity;
    }

    void RenderAdaptor::SetFogDistance(float distance)
    {
        this->Renderer.GetEnvironment().FogDistance = Clamp(distance, 0.0f, 1.0f);
    }

    float RenderAdaptor::GetFogDistance() const
    {
        return this->Renderer.GetEnvironment().FogDistance;
    }

    void RenderAdaptor::SetShadowBlurIterations(size_t iterations)
    {
        auto& environment = this->Renderer.GetEnvironment();
        using BlutIterType = decltype(environment.ShadowBlurIterations);
        constexpr size_t maxIterations = 5;
        environment.ShadowBlurIterations = (BlutIterType)Min(iterations, maxIterations);
    }

    size_t RenderAdaptor::GetShadowBlurIterations() const
    {
        return (size_t)this->Renderer.GetEnvironment().ShadowBlurIterations;
    }
}