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

namespace MxEngine
{
    void RenderAdaptor::InitRendererEnvironment()
    {
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
        this->DebugDraw.Init();
        environment.DebugBufferObject.VAO = this->DebugDraw.GetVAO();

        // default textures
        environment.DefaultBlackMap = Colors::MakeTexture(Colors::BLACK);
        environment.DefaultHeightMap = Colors::MakeTexture(Colors::GREY);
        environment.DefaultNormalMap = Colors::MakeTexture(Colors::FLAT_NORMAL);
        environment.DefaultMaterialMap = Colors::MakeTexture(Colors::WHITE);

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

        environment.BloomHDRMap = GraphicFactory::Create<Texture>();
        environment.BloomHDRMap->Load(nullptr, 1, 1, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);

        auto hdrTexture = GraphicFactory::Create<Texture>();
        hdrTexture->Load(nullptr, 1, 1, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);

        environment.HDRFrameBuffer = GraphicFactory::Create<FrameBuffer>();
        environment.HDRFrameBuffer->AttachTexture(hdrTexture, Attachment::COLOR_ATTACHMENT0);
        environment.HDRFrameBuffer->AttachTextureExtra(environment.BloomHDRMap, Attachment::COLOR_ATTACHMENT1);
        environment.HDRFrameBuffer->UseDrawBuffers(2);
        environment.HDRFrameBuffer->Validate();

        environment.PostProcessFrameBuffer = GraphicFactory::Create<FrameBuffer>();

        for (auto& bloomBuffer : environment.BloomBuffers)
        {
            auto bloomTexture = GraphicFactory::Create<Texture>();
            bloomTexture->Load(nullptr, 1, 1, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);

            bloomBuffer = GraphicFactory::Create<FrameBuffer>();
            bloomBuffer->AttachTexture(bloomTexture, Attachment::COLOR_ATTACHMENT0);
            bloomBuffer->Validate();
        }
    }

    void RenderAdaptor::OnWindowResize(const VectorInt2& newViewport)
    {
        auto& environment = this->Renderer.GetEnvironment();
        environment.Viewport = newViewport;

        GetAttachedTexture(environment.HDRFrameBuffer)->Load(nullptr, newViewport.x, newViewport.y, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
    	if((environment.BloomHDRMap->GetWidth() != newViewport.x) || (environment.BloomHDRMap->GetHeight() != newViewport.y))
            environment.BloomHDRMap->Load(nullptr, newViewport.x, newViewport.y, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
    
        VectorInt2 bloomTextureSize = newViewport;
        bloomTextureSize = (bloomTextureSize + 3) / 2 + ((bloomTextureSize + 3) / 2) % 2;
        for (auto& bloomBuffer : environment.BloomBuffers)
        {
            auto texture = GetAttachedTexture(bloomBuffer);
            GetAttachedTexture(bloomBuffer)->Load(nullptr, bloomTextureSize.x, bloomTextureSize.y, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
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
        if(this->Viewport.IsValid())
            viewportPosition = MxObject::GetByComponent(*this->Viewport).GetComponent<Transform>()->GetPosition();

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

        auto ComputeLODLevel = [this](const AABB& box, const Vector3& viewportPosition)
        {
            float distance = Length(box.GetCenter() - viewportPosition);
            Vector3 length = box.Length();
            float maxLength = Max(length.x, length.y, length.z);
            float scaledDistance = maxLength / distance / this->Viewport->GetZoom();

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

        auto cameraView = ComponentFactory::GetView<CameraController>();
        for (const auto& camera : cameraView)
        {
            if (!camera.HasCamera()) continue;

            auto& object = MxObject::GetByComponent(camera);
            auto transform = object.GetComponent<Transform>();
            auto skyboxComponent = object.GetComponent<Skybox>();
            Skybox skybox = skyboxComponent.IsValid() ? *skyboxComponent.GetUnchecked() : Skybox();
            this->Renderer.SubmitCamera(camera, *transform, skybox);
            TrackMainCameraIndex(camera);
        }

        // submit render units
        auto meshSourceView = ComponentFactory::GetView<MeshSource>();
        for (const auto& meshSource : meshSourceView)
        {
            auto& object = MxObject::GetByComponent(meshSource);

            auto meshRenderer = object.GetComponent<MeshRenderer>();
            auto meshLOD = object.GetComponent<MeshLOD>();
            auto transform = object.GetComponent<Transform>();
            auto instances = object.GetComponent<InstanceFactory>();

            if (!meshRenderer.IsValid() || !transform.IsValid()) continue;

            size_t instanceCount = 0;
            if (instances.IsValid()) instanceCount = instances->GetCount();

            auto mesh = meshSource.GetMesh();

            // we do not try to use LODs for instanced objects, as its quite hard and time consuming. TODO: fix this
            if (meshLOD.IsValid() && !meshLOD->LODs.empty() && instanceCount > 0)
            {
                size_t lod = ComputeLODLevel(mesh->GetAABB(), viewportPosition);
                lod = Min(lod, meshLOD->LODs.size());
                mesh = meshLOD->LODs[lod]; // use LOD of original mesh as render target
            }

            auto& submeshes = mesh->GetSubmeshes();
            for (const auto& submesh : submeshes)
            {
                auto materialId = submesh.GetMaterialId() < meshRenderer->Materials.size() ? submesh.GetMaterialId() : 0;
                auto material = meshRenderer->Materials[materialId];

                this->Renderer.SubmitPrimitive(submesh, *material, *transform, instanceCount);
            }
        }

        auto dirLightView = ComponentFactory::GetView<DirectionalLight>();
        for (auto& dirLight : dirLightView)
        {
            auto transform = MxObject::GetByComponent(dirLight).GetComponent<Transform>();
            this->Renderer.SubmitLightSource(dirLight, *transform);
        }

        auto spotLightView = ComponentFactory::GetView<SpotLight>();
        for (const auto& spotLight : spotLightView)
        {
            auto transform = MxObject::GetByComponent(spotLight).GetComponent<Transform>();
            this->Renderer.SubmitLightSource(spotLight, *transform);
        }

        auto pointLightView = ComponentFactory::GetView<PointLight>();
        for (const auto& pointLight : pointLightView)
        {
            auto transform = MxObject::GetByComponent(pointLight).GetComponent<Transform>();
            this->Renderer.SubmitLightSource(pointLight, *transform);
        }

        this->DebugDraw.ClearBuffer();
        auto debugColor = MakeVector4(1.0f, 0.0f, 0.0f, 1.0f);
        if (this->DebugDraw.DrawAxisBoundingBoxes | this->DebugDraw.DrawBoundingSpheres)
        {
            for (auto& meshSource : meshSourceView)
            {
                auto& object = MxObject::GetByComponent(meshSource);
                AABB box;
                if (object.GetInstanceCount() > 0 && object.GetComponent<MeshSource>().IsValid())
                {
                    for (const auto& instance : object.GetInstances())
                    {
                        box = object.GetComponent<MeshSource>()->GetMesh()->GetAABB() * instance.Model.GetMatrix();
                        if (this->DebugDraw.DrawAxisBoundingBoxes)
                            this->DebugDraw.SubmitAABB(box, debugColor);
                        if (this->DebugDraw.DrawBoundingSpheres)
                            this->DebugDraw.SubmitSphere(ToSphere(box), debugColor);
                    }
                }
                else
                {
                    box = object.GetAABB();
                    if (this->DebugDraw.DrawAxisBoundingBoxes)
                        this->DebugDraw.SubmitAABB(box, debugColor);
                    if (this->DebugDraw.DrawBoundingSpheres)
                        this->DebugDraw.SubmitSphere(ToSphere(box), debugColor);
                }
            }
            this->DebugDraw.SubmitBuffer();
            environment.DebugBufferObject.VertexCount = this->DebugDraw.GetSize();
            environment.OverlayDebugDraws = this->DebugDraw.DrawAsScreenOverlay;
        }

        this->Renderer.StartPipeline();
    }

    void RenderAdaptor::SetRenderToDefaultFrameBuffer(bool value)
    {
        this->Renderer.GetEnvironment().RenderToDefaultFrameBuffer = value;
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

    size_t RenderAdaptor::getShadowBlurIterations() const
    {
        return (size_t)this->Renderer.GetEnvironment().ShadowBlurIterations;
    }
}