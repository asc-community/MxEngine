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
#include "Library/Noise/NoiseGenerator.h"
#include "Core/Config/GlobalConfig.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Core/Components/Rendering/MeshLOD.h"
#include "Core/Components/Rendering/DebugDraw.h"
#include "Core/Components/Camera/CameraEffects.h"
#include "Core/Components/Camera/CameraSSR.h"
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
        this->SetShadowBlurIterations(1);
        this->SetLightSamples(4);

        // helper objects
        environment.RectangularObject.Init(1.0f);
        environment.SkyboxCubeObject.Init();
        this->DebugDrawer.Init();
        environment.DebugBufferObject.VAO = this->DebugDrawer.GetVAO();

        // light bounding objects
        auto pyramid = Primitives::CreatePyramid();
        auto& pyramidMesh = pyramid->Submeshes.front();
        this->Renderer.GetLightInformation().PyramidLight =
            RenderHelperObject(pyramidMesh.Data.GetVBO(), pyramidMesh.Data.GetVAO(), pyramidMesh.Data.GetIBO());

        auto sphere = Primitives::CreateSphere(8);
        auto& sphereMesh = sphere->Submeshes.front();
        this->Renderer.GetLightInformation().SphereLight =
            RenderHelperObject(sphereMesh.Data.GetVBO(), sphereMesh.Data.GetVAO(), sphereMesh.Data.GetIBO());

        auto pyramidInstanced = Primitives::CreatePyramid();
        auto& pyramidInstancedMesh = pyramidInstanced->Submeshes.front();
        this->Renderer.GetLightInformation().SpotLightsInstanced = 
            SpotLightInstancedObject(pyramidInstancedMesh.Data.GetVBO(), pyramidInstancedMesh.Data.GetVAO(), pyramidInstancedMesh.Data.GetIBO());

        auto sphereInstanced = Primitives::CreateSphere(8);
        auto& sphereInstancedMesh = sphereInstanced->Submeshes.front();
        this->Renderer.GetLightInformation().PointLigthsInstanced =
            PointLightInstancedObject(sphereInstancedMesh.Data.GetVBO(), sphereInstancedMesh.Data.GetVAO(), sphereInstancedMesh.Data.GetIBO());

        int internalTextureSize = (int)GlobalConfig::GetEngineTextureSize();
        // default textures
        environment.DefaultShadowMap = Colors::MakeTexture(Colors::BLACK);
        environment.DefaultBlackMap = Colors::MakeTexture(Colors::BLACK);
        environment.DefaultNormalMap = Colors::MakeTexture(Colors::FLAT_NORMAL);
        environment.DefaultMaterialMap = Colors::MakeTexture(Colors::WHITE);
        environment.DefaultGreyMap = Colors::MakeTexture(Colors::GREY);
        environment.DefaultShadowCubeMap = Colors::MakeCubeMap(Colors::BLACK);
        environment.DefaultSkybox = Colors::MakeCubeMap(Colors::GREY);

        environment.DefaultBlackMap->SetPath("[[black color]]");
        environment.DefaultNormalMap->SetPath("[[default normal]]");
        environment.DefaultMaterialMap->SetPath("[[white color]]");
        environment.DefaultGreyMap->SetPath("[[grey color]]");
        environment.DefaultShadowMap->SetPath("[[default shadow map]]");

        environment.AverageWhiteTexture = GraphicFactory::Create<Texture>();
        environment.AverageWhiteTexture->Load(nullptr, internalTextureSize, internalTextureSize, 1, false, TextureFormat::R16F);
        environment.AverageWhiteTexture->SetSamplingFromLOD(environment.AverageWhiteTexture->GetMaxTextureLOD());
        environment.AverageWhiteTexture->SetPath("[[average white]]");

        environment.AmbientOcclusionTexture = GraphicFactory::Create<Texture>();
        environment.AmbientOcclusionTexture->Load(nullptr, internalTextureSize, internalTextureSize, 1, false, TextureFormat::R);
        environment.AmbientOcclusionTexture->SetPath("[[ambient occlusion]]");
        
        // shaders
        auto shaderFolder = FileManager::GetEngineShaderFolder();
        if (!File::Exists(shaderFolder))
        {
            MXLOG_FATAL("MxEngine::Application", "there is not Engine/Shaders folder in root directory. Try rebuilding your application");
        }

        environment.Shaders["GBuffer"_id] = AssetManager::LoadShader(
            shaderFolder / "gbuffer_vertex.glsl", 
            shaderFolder / "gbuffer_fragment.glsl"
        );

        environment.Shaders["Transparent"_id] = AssetManager::LoadShader(
            shaderFolder / "gbuffer_vertex.glsl", 
            shaderFolder / "transparent_fragment.glsl"
        );

        environment.Shaders["DirLight"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "dirlight_fragment.glsl"
        );

        environment.Shaders["SpotLight"_id] = AssetManager::LoadShader(
            shaderFolder / "spotlight_vertex.glsl",
            shaderFolder / "spotlight_fragment.glsl"
        );

        environment.Shaders["PointLight"_id] = AssetManager::LoadShader(
            shaderFolder / "pointlight_vertex.glsl",
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

        environment.Shaders["DepthTexture"_id] = AssetManager::LoadShader(
            shaderFolder / "depthtexture_vertex.glsl", 
            shaderFolder / "depthtexture_fragment.glsl"
        );

        environment.Shaders["DepthCubeMap"_id] = AssetManager::LoadShader(
            shaderFolder / "depthcubemap_vertex.glsl",
            shaderFolder / "depthcubemap_geometry.glsl",
            shaderFolder / "depthcubemap_fragment.glsl"
        );

        environment.Shaders["BloomIteration"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "bloom_iter_fragment.glsl"
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
        
        environment.Shaders["ChromaticAbberation"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "chromatic_abberation_fragment.glsl"
        );

        environment.Shaders["AmbientOcclusion"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "ambient_occlusion_fragment.glsl"
        );

        environment.Shaders["ApplyAmbientOcclusion"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "apply_ambient_occlusion_fragment.glsl"
        );

        environment.Shaders["ColorGrading"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "color_grading_fragment.glsl"
        );

        environment.Shaders["IBL"_id] = AssetManager::LoadShader(
            shaderFolder / "rect_vertex.glsl",
            shaderFolder / "ibl_fragment.glsl"
        );

        // framebuffers
        environment.DepthFrameBuffer = GraphicFactory::Create<FrameBuffer>();
        environment.DepthFrameBuffer->UseOnlyDepth();
        environment.PostProcessFrameBuffer = GraphicFactory::Create<FrameBuffer>();

        auto bloomBufferSize = (int)GlobalConfig::GetEngineTextureSize();
        for (auto& bloomBuffer : environment.BloomBuffers)
        {
            auto bloomTexture = GraphicFactory::Create<Texture>();
            bloomTexture->Load(nullptr, bloomBufferSize, bloomBufferSize, 3, false, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
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
                auto toneMappingComponent = object.GetComponent<CameraToneMapping>();
                auto ssrComponent = object.GetComponent<CameraSSR>();
                Skybox skybox                  = skyboxComponent.IsValid()      ? *skyboxComponent.GetUnchecked()     : Skybox();
                CameraEffects* effects         = effectsComponent.IsValid()     ? effectsComponent.GetUnchecked()     : nullptr;
                CameraToneMapping* toneMapping = toneMappingComponent.IsValid() ? toneMappingComponent.GetUnchecked() : nullptr;
                CameraSSR* ssr                 = ssrComponent.IsValid()         ? ssrComponent.GetUnchecked()         : nullptr;

                this->Renderer.SubmitCamera(camera, transform, skybox, effects, toneMapping, ssr);
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
                bool castsShadow = meshSource.CastsShadow;

                if (!meshSource.IsDrawn || !meshRenderer.IsValid()) continue;

                // we do not try to use LODs for instanced objects, as its quite hard and time consuming. TODO: fix this
                if (meshLOD.IsValid() && instanceCount == 0)
                {
                    meshLOD->FixBestLOD(viewportPosition, viewportZoom);
                    mesh = meshLOD->GetMeshLOD();
                }

                auto& submeshes = mesh->Submeshes;
                for (const auto& submesh : submeshes)
                {
                    auto materialId = submesh.GetMaterialId();
                    if (materialId >= meshRenderer->Materials.size()) continue;
                    auto material = meshRenderer->Materials[materialId];

                    this->Renderer.SubmitPrimitive(submesh, *material, castsShadow, transform, instanceCount);
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

    void RenderAdaptor::SetLightSamples(size_t samples)
    {
        auto& environment = this->Renderer.GetEnvironment();
        using SampleCountType = decltype(environment.LightSamples);
        constexpr size_t maxSamples = 1024;
        environment.LightSamples = (SampleCountType)Clamp(samples, (size_t)1, maxSamples);
    }

    size_t RenderAdaptor::GetLightSamples() const
    {
        return (size_t)this->Renderer.GetEnvironment().LightSamples;
    }
}