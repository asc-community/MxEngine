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
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshLOD.h"
#include "Core/Components/Rendering/DebugDraw.h"

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
        this->SetFogDensity(0.01f);

        // helper objects
        environment.RectangularObject.Init(1.0f);
        environment.SkyboxCubeObject.Init();
        this->DebugDrawer.Init();
        environment.DebugBufferObject.VAO = this->DebugDrawer.GetVAO();

        // default textures
        environment.DefaultBlackMap = Colors::MakeTexture(Colors::BLACK);
        environment.DefaultHeightMap = Colors::MakeTexture(Colors::GREY);
        environment.DefaultNormalMap = Colors::MakeTexture(Colors::FLAT_NORMAL);
        environment.DefaultMaterialMap = Colors::MakeTexture(Colors::WHITE);
        environment.DefaultBlackCubeMap = Colors::MakeCubeMap(Colors::BLACK);

        // shaders
        environment.MainShader = GraphicFactory::Create<Shader>();
        LoadMainShader();

        environment.SkyboxShader = GraphicFactory::Create<Shader>();
        environment.SkyboxShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(skybox_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(skybox_fragment)
        );

        environment.ShadowMapShader = GraphicFactory::Create<Shader>();
        environment.ShadowMapShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(depthtexture_vertex)
            , 
            #include MAKE_PLATFORM_SHADER(depthtexture_fragment)
        );

        environment.ShadowCubeMapShader = GraphicFactory::Create<Shader>();
        environment.ShadowCubeMapShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(depthcubemap_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(depthcubemap_geometry)
            ,
            #include MAKE_PLATFORM_SHADER(depthcubemap_fragment)
        );

        environment.MSAAHDRSplitShader = GraphicFactory::Create<Shader>();
        environment.MSAAHDRSplitShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(rect_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(msaa_fragment)
        );

        environment.HDRBloomCombineHDRShader = GraphicFactory::Create<Shader>();
        environment.HDRBloomCombineHDRShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(rect_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(hdr_fragment)
        );

        environment.BloomShader = GraphicFactory::Create<Shader>();
        environment.BloomShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(rect_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(bloom_fragment)
        );

        environment.ImageForwardShader = GraphicFactory::Create<Shader>();
        environment.ImageForwardShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(rect_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(rect_fragment)
        );

        environment.DebugDrawShader = GraphicFactory::Create<Shader>();
        environment.DebugDrawShader->LoadFromString(
            #include MAKE_PLATFORM_SHADER(debug_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(debug_fragment)
        );

        // framebuffers
        environment.DepthFrameBuffer = GraphicFactory::Create<FrameBuffer>();
        environment.PostProcessFrameBuffer = GraphicFactory::Create<FrameBuffer>();

        for (auto& bloomBuffer : environment.BloomBuffers)
        {
            auto bloomTexture = GraphicFactory::Create<Texture>();
            constexpr size_t bloomBufferSize = 512;

            for (auto& bloomBuffer : environment.BloomBuffers)
            {
                bloomTexture->Load(nullptr, bloomBufferSize, bloomBufferSize, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
            }

            bloomBuffer = GraphicFactory::Create<FrameBuffer>();
            bloomBuffer->AttachTexture(bloomTexture, Attachment::COLOR_ATTACHMENT0);
            bloomBuffer->Validate();
        }
    }

    void RenderAdaptor::LoadMainShader(bool useLighting)
    {
        if (useLighting)
        {
            this->Renderer.GetEnvironment().MainShader->LoadFromString(
                #include MAKE_PLATFORM_SHADER(object_vertex)
                ,
                #include MAKE_PLATFORM_SHADER(object_fragment)
            );
        }
        else
        {
            this->Renderer.GetEnvironment().MainShader->LoadFromString(
                #include MAKE_PLATFORM_SHADER(nolight_object_vertex)
                ,
                #include MAKE_PLATFORM_SHADER(nolight_object_fragment)
            );
        }
    }

    void RenderAdaptor::PerformRenderIteration()
    {
        this->Renderer.ResetPipeline();

        auto& environment = this->Renderer.GetEnvironment();
        environment.MainCameraIndex = std::numeric_limits<decltype(environment.MainCameraIndex)>::max();
        auto viewportPosition = MakeVector3(0.0f);
        float viewportZoom = 0.0f;
        if (this->Viewport.IsValid())
        {
            viewportPosition = MxObject::GetByComponent(*this->Viewport).Transform->GetPosition();
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

        constexpr auto ComputeLODLevel = [](const AABB& box, const Vector3& viewportPosition, float viewportZoom)
        {
            float distance = Length(box.GetCenter() - viewportPosition);
            Vector3 length = box.Length();
            float maxLength = Max(length.x, length.y, length.z);
            float scaledDistance = maxLength / distance / viewportZoom;

            constexpr static std::array lodDistance = {
                    0.21f,
                    0.15f,
                    0.10f,
                    0.06f,
                    0.03f,
            };
            size_t lod = 0;
            while (lod < lodDistance.size() && scaledDistance < lodDistance[lod])
                lod++;

            return lod;
        };

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitCameras()");
            auto cameraView = ComponentFactory::GetView<CameraController>();
            for (const auto& camera : cameraView)
            {
                auto& object = MxObject::GetByComponent(camera);
                auto& transform = object.Transform;
                auto skyboxComponent = object.GetComponent<Skybox>();
                Skybox skybox = skyboxComponent.IsValid() ? *skyboxComponent.GetUnchecked() : Skybox();
                this->Renderer.SubmitCamera(camera, *transform, skybox);
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
                    auto materialId = submesh.GetMaterialId() < meshRenderer->Materials.size() ? submesh.GetMaterialId() : 0;
                    auto material = meshRenderer->Materials[materialId];

                    this->Renderer.SubmitPrimitive(submesh, *material, *transform, instanceCount);
                }
            }
        }

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitLightSources()");
            auto dirLightView = ComponentFactory::GetView<DirectionalLight>();
            for (const auto& dirLight : dirLightView)
            {
                auto& transform = MxObject::GetByComponent(dirLight).Transform;
                this->Renderer.SubmitLightSource(dirLight, *transform);
            }

            auto spotLightView = ComponentFactory::GetView<SpotLight>();
            for (const auto& spotLight : spotLightView)
            {
                auto& transform = MxObject::GetByComponent(spotLight).Transform;
                this->Renderer.SubmitLightSource(spotLight, *transform);
            }

            auto pointLightView = ComponentFactory::GetView<PointLight>();
            for (const auto& pointLight : pointLightView)
            {
                auto& transform = MxObject::GetByComponent(pointLight).Transform;
                this->Renderer.SubmitLightSource(pointLight, *transform);
            }
        }

        {
            MAKE_SCOPE_PROFILER("RenderAdaptor::SubmitDebugData()");
            this->DebugDrawer.ClearBuffer();

            constexpr auto submitDebugData = [](DebugBuffer& drawer, DebugDraw& debugDraw, const AABB& box)
            {
                if (debugDraw.RenderBoundingBox)
                    drawer.SubmitAABB(box, debugDraw.Color);
                if (debugDraw.RenderBoundingSphere)
                    drawer.SubmitSphere(ToSphere(box), debugDraw.Color);
            };

            auto debugDrawView = ComponentFactory::GetView<DebugDraw>();
            for (auto& debugDraw : debugDrawView)
            {
                auto& object = MxObject::GetByComponent(debugDraw);
                auto instances = object.GetComponent<InstanceFactory>();
                auto meshSource = object.GetComponent<MeshSource>();
                AABB box;
                if (meshSource.IsValid())
                {
                    if (instances.IsValid())
                    {
                        for (const auto& instance : instances->GetInstances())
                        {
                            box = meshSource->Mesh->GetAABB() * instance.Transform.GetMatrix();
                            submitDebugData(this->DebugDrawer, debugDraw, box);
                        }
                    }
                    else
                    {
                        box = meshSource->Mesh->GetAABB() * object.Transform->GetMatrix();
                        submitDebugData(this->DebugDrawer, debugDraw, box);
                    }
                }
            }
            this->DebugDrawer.SubmitBuffer();
            environment.DebugBufferObject.VertexCount = this->DebugDrawer.GetSize();
            environment.OverlayDebugDraws = this->DebugDrawer.DrawAsScreenOverlay;
        }

        this->Renderer.StartPipeline();
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
        constexpr size_t maxIterations = (size_t)std::numeric_limits<BlutIterType>::max();
        environment.ShadowBlurIterations = (BlutIterType)Min(iterations, maxIterations);
    }

    size_t RenderAdaptor::GetShadowBlurIterations() const
    {
        return (size_t)this->Renderer.GetEnvironment().ShadowBlurIterations;
    }
}