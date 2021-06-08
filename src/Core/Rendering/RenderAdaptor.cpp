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
// AND ANY EXPRESS OR IMPLIED WARRANTIES, fINCLUDING, BUT NOT LIMITED TO, THE
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
#include "Library/Noise/NoiseGenerator.h"
#include "Core/Config/GlobalConfig.h"
#include "Core/Resources/BufferAllocator.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Core/Components/Rendering/MeshLOD.h"
#include "Core/Components/Rendering/DebugDraw.h"
#include "Core/Components/Rendering/ParticleSystem.h"
#include "Core/Components/Camera/CameraEffects.h"
#include "Core/Components/Camera/CameraSSR.h"
#include "Core/Components/Camera/CameraSSGI.h"
#include "Core/Components/Camera/CameraSSAO.h"
#include "Core/Components/Camera/CameraToneMapping.h"
#include "Core/Components/Lighting/DirectionalLight.h"
#include "Core/Components/Lighting/PointLight.h"
#include "Core/Components/Lighting/SpotLight.h"
#include "Core/Components/Instancing/InstanceFactory.h"
#include "Core/Rendering/DebugDataSubmitter.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/FileSystem/FileManager.h"

namespace MxEngine
{
    void RenderAdaptor::InitRendererEnvironment()
    {
        MAKE_SCOPE_PROFILER("RenderAdaptor::InitEnvironment()");
        auto& environment = this->Renderer.GetEnvironment();

        this->SetRenderToDefaultFrameBuffer();

        environment.RenderVAO = BufferAllocator::GetVAO();
        environment.RenderSSBO = BufferAllocator::GetSSBO();

        // helper objects
        environment.RectangularObject.Init(1.0f);
        environment.SkyboxCubeObject.Init();
        this->DebugDrawer.Init();
        environment.DebugBufferObject.VAO = this->DebugDrawer.GetVAO();

        // light bounding objects
        auto pyramidInstanced = Primitives::CreatePyramid();
        pyramidInstanced.MakeStatic();
        this->Renderer.GetLightInformation().SpotLightsInstanced = SpotLightInstancedObject(
            pyramidInstanced->GetBaseVerteciesOffset(), pyramidInstanced->GetTotalVerteciesCount(),
            pyramidInstanced->GetBaseIndiciesOffset(),  pyramidInstanced->GetTotalIndiciesCount());

        auto sphereInstanced = Primitives::CreateSphere(8);
        sphereInstanced.MakeStatic();
        this->Renderer.GetLightInformation().PointLightsInstanced = PointLightInstancedObject(
            sphereInstanced->GetBaseVerteciesOffset(), sphereInstanced->GetTotalVerteciesCount(),
            sphereInstanced->GetBaseIndiciesOffset(),  sphereInstanced->GetTotalIndiciesCount());

        auto pyramid = Primitives::CreatePyramid();
        pyramid.MakeStatic();
        this->Renderer.GetLightInformation().SpotLight = RenderHelperObject(
            pyramid->GetBaseVerteciesOffset(), pyramid->GetTotalVerteciesCount(),
            pyramid->GetBaseIndiciesOffset(), pyramid->GetTotalIndiciesCount(),
            environment.RenderVAO);

        auto sphere = Primitives::CreateSphere(8);
        sphere.MakeStatic();
        this->Renderer.GetLightInformation().PointLight = RenderHelperObject(
            sphere->GetBaseVerteciesOffset(), sphere->GetTotalVerteciesCount(),
            sphere->GetBaseIndiciesOffset(), sphere->GetTotalIndiciesCount(),
            environment.RenderVAO);

        auto textureFolder = FileManager::GetEngineTextureDirectory();
        int internalTextureSize = (int)GlobalConfig::GetEngineTextureSize();
        // default textures
        environment.DefaultShadowMap = Colors::MakeTexture(Colors::BLACK);
        environment.DefaultBlackMap = Colors::MakeTexture(Colors::BLACK);
        environment.DefaultNormalMap = Colors::MakeTexture(Colors::FLAT_NORMAL);
        environment.DefaultMaterialMap = Colors::MakeTexture(Colors::WHITE);
        environment.DefaultGreyMap = Colors::MakeTexture(Colors::GREY);
        environment.DefaultShadowCubeMap = Colors::MakeCubeMap(Colors::BLACK);
        environment.DefaultSkybox = Colors::MakeCubeMap(0.8f, 0.9f, 1.0f);

        environment.DefaultBlackMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("black color"));
        environment.DefaultNormalMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("default normal"));
        environment.DefaultMaterialMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("white color"));
        environment.DefaultGreyMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("grey color"));
        environment.DefaultShadowMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("default shadow map"));
        environment.DefaultShadowCubeMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("default shadow cubemap"));
        environment.DefaultSkybox->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("default skybox cubemap"));

        environment.AverageWhiteTexture = Factory<Texture>::Create();
        environment.AverageWhiteTexture->Load(nullptr, internalTextureSize, internalTextureSize, 1, false, TextureFormat::R16F);
        environment.AverageWhiteTexture->SetMinLOD(environment.AverageWhiteTexture->GetMaxTextureLOD());
        environment.AverageWhiteTexture->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("average white"));

        // TODO: use RG16
        environment.EnvironmentBRDFLUT = AssetManager::LoadTexture(textureFolder / "env_brdf_lut.png", TextureFormat::RG);
        environment.EnvironmentBRDFLUT->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("BRDF LUT"));
        
        // shaders
        auto shaderFolder = FileManager::GetEngineShaderDirectory();
        if (!File::Exists(shaderFolder))
        {
            MXLOG_FATAL("MxEngine::Application", "there is not Engine/Shaders folder in root directory. Try rebuilding your application");
        }

        environment.Shaders["GBuffer"_id] = AssetManager::LoadShader(
            shaderFolder / "gbuffer_vertex.glsl", 
            shaderFolder / "gbuffer_fragment.glsl"
        );

        environment.Shaders["GBufferMask"_id] = AssetManager::LoadShader(
            shaderFolder / "gbuffer_vertex.glsl",
            shaderFolder / "gbuffer_mask_fragment.glsl"
        );

        environment.Shaders["Transparent"_id] = AssetManager::LoadShader(
            shaderFolder / "gbuffer_vertex.glsl", 
            shaderFolder / "transparent_fragment.glsl"
        );

        environment.Shaders["DirLight"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "dirlight_fragment.glsl"
        );

        environment.Shaders["SpotLightShadow"_id] = AssetManager::LoadShader(
            shaderFolder / "spotlight_shadow_vertex.glsl",
            shaderFolder / "spotlight_fragment.glsl"
        );

        environment.Shaders["SpotLightNonShadow"_id] = AssetManager::LoadShader(
            shaderFolder / "spotlight_nonshadow_vertex.glsl",
            shaderFolder / "spotlight_fragment.glsl"
        );

        environment.Shaders["PointLightShadow"_id] = AssetManager::LoadShader(
            shaderFolder / "pointlight_shadow_vertex.glsl",
            shaderFolder / "pointlight_fragment.glsl"
        );

        environment.Shaders["PointLightNonShadow"_id] = AssetManager::LoadShader(
            shaderFolder / "pointlight_nonshadow_vertex.glsl",
            shaderFolder / "pointlight_fragment.glsl"
        );

        environment.Shaders["HDRToLDR"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "hdr_to_ldr_fragment.glsl"
        );

        environment.Shaders["FXAA"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "fxaa_fragment.glsl"
        );

        environment.Shaders["Fog"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "fog_fragment.glsl"
        );

        environment.Shaders["Vignette"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "vignette_fragment.glsl"
        );

        environment.Shaders["Skybox"_id] = AssetManager::LoadShader(
            shaderFolder / "skybox_vertex.glsl",
            shaderFolder / "skybox_fragment.glsl"
        );

        environment.Shaders["DirLightDepthMap"_id] = AssetManager::LoadShader(
            shaderFolder / "depthtexture_vertex.glsl",
            shaderFolder / "depthtexture_fragment.glsl"
        );

        environment.Shaders["DirLightMaskDepthMap"_id] = AssetManager::LoadShader(
            shaderFolder / "depthtexture_vertex.glsl",
            shaderFolder / "depthtexture_mask_fragment.glsl"
        );

        environment.Shaders["SpotLightDepthMap"_id] = AssetManager::LoadShader(
            shaderFolder / "depthtexture_vertex.glsl", 
            shaderFolder / "depthtexture_fragment.glsl"
        );

        environment.Shaders["SpotLightMaskDepthMap"_id] = AssetManager::LoadShader(
            shaderFolder / "depthtexture_vertex.glsl",
            shaderFolder / "depthtexture_mask_fragment.glsl"
        );

        environment.Shaders["PointLightDepthMap"_id] = AssetManager::LoadShader(
            shaderFolder / "depthcubemap_vertex.glsl",
            shaderFolder / "depthcubemap_geometry.glsl",
            shaderFolder / "depthcubemap_fragment.glsl"
        );

        environment.Shaders["GaussianBlur"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "gaussian_blur_fragment.glsl"
        );

        environment.Shaders["BloomSplit"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "bloom_split_fragment.glsl"
        );

        environment.Shaders["ImageForward"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "rect_fragment.glsl"
        );

        environment.Shaders["DebugDraw"_id] = AssetManager::LoadShader(
            shaderFolder / "debug_vertex.glsl",
            shaderFolder / "debug_geometry.glsl",
            shaderFolder / "debug_fragment.glsl"
        );

        environment.Shaders["VRCamera"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "vr_fragment.glsl"
        );

        environment.Shaders["AverageWhite"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "average_white_fragment.glsl"
        );

        environment.Shaders["SSR"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "ssr_fragment.glsl"
        );

        environment.Shaders["ApplySSR"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "apply_ssr_fragment.glsl"
        );

        environment.Shaders["SSGI"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "ssgi_fragment.glsl"
        );

        environment.Shaders["ApplySSGI"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "apply_ssgi_fragment.glsl"
        );
        
        environment.Shaders["ChromaticAbberation"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "chromatic_abberation_fragment.glsl"
        );

        environment.Shaders["AmbientOcclusion"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "ssao_fragment.glsl"
        );

        environment.Shaders["ApplyAmbientOcclusion"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "apply_ssao_fragment.glsl"
        );

        environment.Shaders["ColorGrading"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "color_grading_fragment.glsl"
        );

        environment.Shaders["IBL"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "ibl_fragment.glsl"
        );

        environment.Shaders["ParticleOpaque"_id] = AssetManager::LoadShader(
            shaderFolder / "particle_opaque_vertex.glsl",
            shaderFolder / "particle_opaque_fragment.glsl"
        );

        environment.Shaders["ParticleTransparent"_id] = AssetManager::LoadShader(
            shaderFolder / "particle_transparent_vertex.glsl",
            shaderFolder / "particle_transparent_fragment.glsl"
        );

        // compute shaders
        environment.ComputeShaders["Particle"_id] = AssetManager::LoadComputeShader(
            shaderFolder / "particle_compute.glsl"
        );

        // framebuffers
        environment.DepthFrameBuffer = Factory<FrameBuffer>::Create();
        environment.DepthFrameBuffer->UseOnlyDepth();
        environment.PostProcessFrameBuffer = Factory<FrameBuffer>::Create();
        environment.BloomFrameBuffer = Factory<FrameBuffer>::Create();

        auto bloomBufferSize = (int)GlobalConfig::GetEngineTextureSize();
        for (auto& bloomTexture : environment.BloomTextures)
        {
            bloomTexture = Factory<Texture>::Create();
            bloomTexture->Load(nullptr, bloomBufferSize, bloomBufferSize, 3, false, HDRTextureFormat);
            bloomTexture->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("bloom"));
            bloomTexture->SetWrapType(TextureWrap::CLAMP_TO_EDGE);
        }
    }

    void RenderAdaptor::RenderFrame()
    {
        auto& environment = this->Renderer.GetEnvironment();
        environment.MainCameraIndex = std::numeric_limits<decltype(environment.MainCameraIndex)>::max();
        auto viewportPosition = MakeVector3(0.0f);
        float viewportZoom = 0.0f;
        if (this->Viewport.IsValid())
        {
            viewportPosition = MxObject::GetByComponent(*this->Viewport).LocalTransform.GetPosition();
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
                auto& transform = object.LocalTransform;

                auto skyboxComponent = object.GetComponent<Skybox>();
                auto effectsComponent = object.GetComponent<CameraEffects>();
                auto toneMappingComponent = object.GetComponent<CameraToneMapping>();
                auto ssrComponent = object.GetComponent<CameraSSR>();
                auto ssgiComponent = object.GetComponent<CameraSSGI>();
                auto ssaoComponent = object.GetComponent<CameraSSAO>();
                Skybox* skybox                 = skyboxComponent.IsValid()      ? skyboxComponent.GetUnchecked()      : nullptr;
                CameraEffects* effects         = effectsComponent.IsValid()     ? effectsComponent.GetUnchecked()     : nullptr;
                CameraToneMapping* toneMapping = toneMappingComponent.IsValid() ? toneMappingComponent.GetUnchecked() : nullptr;
                CameraSSR* ssr                 = ssrComponent.IsValid()         ? ssrComponent.GetUnchecked()         : nullptr;
                CameraSSGI* ssgi               = ssgiComponent.IsValid()        ? ssgiComponent.GetUnchecked()        : nullptr;
                CameraSSAO* ssao               = ssaoComponent.IsValid()        ? ssaoComponent.GetUnchecked()        : nullptr;

                this->Renderer.SubmitCamera(camera, transform, skybox, effects, toneMapping, ssr, ssgi, ssao);
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
                auto& transform = object.LocalTransform;
                auto meshRenderer = object.GetComponent<MeshRenderer>();
                auto meshLOD = object.GetComponent<MeshLOD>();
                auto instances = object.GetComponent<InstanceFactory>();

                size_t instanceCount = 0, instanceOffset = 0;
                if (instances.IsValid())
                {
                    instanceCount = instances->GetInstanceCount();
                    instanceOffset = instances->GetInstanceBufferOffset();
                    if (instanceCount == 0) continue; // skip objects without instances
                }

                auto mesh = meshSource.Mesh;
                bool castsShadow = meshSource.CastsShadow;

                if (!meshSource.IsDrawn || !meshRenderer.IsValid() || !mesh.IsValid()) continue;

                // we do not try to use LODs for instanced objects, as its quite hard and time consuming. TODO: fix this
                if (meshLOD.IsValid() && instanceCount == 0)
                {
                    meshLOD->FixBestLOD(viewportPosition, viewportZoom);
                    mesh = meshLOD->GetMeshLOD();
                }

                size_t renderGroupIndex = this->Renderer.SubmitRenderGroup(*mesh, instanceOffset, instanceCount);
                for (const auto& submesh : mesh->GetSubMeshes())
                {
                    auto materialId = submesh.GetMaterialId();
                    if (materialId >= meshRenderer->Materials.size()) continue;
                    auto material = meshRenderer->Materials[materialId];

                    this->Renderer.SubmitRenderUnit(renderGroupIndex, submesh, *material, transform, castsShadow, object.Name.c_str());
                }
            }
        }

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitParticleSystems()");
            auto particleSystemView = ComponentFactory::GetView<ParticleSystem>();
            for (const auto& particleSystem : particleSystemView)
            {
                auto& object = MxObject::GetByComponent(particleSystem);
                auto meshRenderer = (IsInstance(object) ? *GetInstanceParent(object) : object).GetComponent<MeshRenderer>();
                if (!meshRenderer.IsValid() || meshRenderer->Materials.empty())
                    continue;

                auto& transform = MxObject::GetByComponent(particleSystem).LocalTransform;
                this->Renderer.SubmitParticleSystem(particleSystem, *meshRenderer->GetMaterial(), transform);
            }
        }

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitLightSources()");
            auto dirLightView = ComponentFactory::GetView<DirectionalLight>();
            for (const auto& dirLight : dirLightView)
            {
                auto& transform = MxObject::GetByComponent(dirLight).LocalTransform;
                this->Renderer.SubmitLightSource(dirLight, transform);
            }

            auto spotLightView = ComponentFactory::GetView<SpotLight>();
            for (const auto& spotLight : spotLightView)
            {
                auto& transform = MxObject::GetByComponent(spotLight).LocalTransform;
                this->Renderer.SubmitLightSource(spotLight, transform);
            }

            auto pointLightView = ComponentFactory::GetView<PointLight>();
            for (const auto& pointLight : pointLightView)
            {
                auto& transform = MxObject::GetByComponent(pointLight).LocalTransform;
                this->Renderer.SubmitLightSource(pointLight, transform);
            }
        }

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitDebugData()");

            DebugDataSubmitter debugProcessor(this->DebugDrawer);

            auto debugDrawView = ComponentFactory::GetView<DebugDraw>();
            for (auto& debugDraw : debugDrawView)
            {
                auto& object = MxObject::GetByComponent(debugDraw);
                debugProcessor.ProcessObject(object);
            }
                
            environment.DebugBufferObject.VertexCount = this->DebugDrawer.GetSize();
            environment.OverlayDebugDraws = this->DebugDrawer.DrawAsScreenOverlay;
            this->DebugDrawer.SubmitBuffer();
            this->DebugDrawer.ClearBuffer();

            environment.TimeDelta = Time::Delta();
        }

        this->Renderer.GetRenderStatistics().ResetAll();
        this->Renderer.StartPipeline();
    }

    void RenderAdaptor::SubmitRenderedFrame()
    {
        this->Renderer.EndPipeline();
        this->Renderer.Render();
        this->Renderer.ResetPipeline();
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
}