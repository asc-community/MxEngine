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

#include "RenderController.h"
#include "Utilities/Format/Format.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/ParticleSystem.h"
#include "Core/Components/Camera/CameraController.h"
#include "Core/Components/Camera/CameraEffects.h"
#include "Core/Components/Camera/CameraToneMapping.h"
#include "Core/Components/Camera/CameraSSR.h"
#include "Core/Components/Camera/CameraSSGI.h"
#include "Core/Components/Camera/CameraSSAO.h"
#include "Core/Components/Camera/CameraGodRay.h"
#include "Core/Components/Lighting/DirectionalLight.h"
#include "Core/Components/Lighting/SpotLight.h"
#include "Core/Components/Lighting/PointLight.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Utilities/Profiler/Profiler.h"
#include "Platform/Compute/Compute.h"
#include "RenderUtilities/ShadowMapGenerator.h"

namespace MxEngine
{
    constexpr size_t MaxDirLightCount = 4;
    constexpr size_t ParticleComputeGroupSize = 64;

    void RenderController::PrepareShadowMaps()
    {
        MAKE_SCOPE_PROFILER("RenderController::PrepareShadowMaps()");

        ShadowMapGenerator generatorOpaque(this->Pipeline.ShadowCasters, this->Pipeline.RenderUnits, this->Pipeline.MaterialUnits);
        ShadowMapGenerator generatorMasked(this->Pipeline.MaskedShadowCasters, this->Pipeline.RenderUnits, this->Pipeline.MaterialUnits);

        this->Pipeline.Environment.RenderVAO->Bind();

        {
            MAKE_SCOPE_PROFILER("RenderController::PrepareDirectionalLightMaps()");
            generatorOpaque.GenerateFor(
                *this->Pipeline.Environment.Shaders["DirLightDepthMap"_id], 
                this->Pipeline.Lighting.DirectionalLights,
                ShadowMapGenerator::LoadStoreOptions::CLEAR
            );
            generatorMasked.GenerateFor(
                *this->Pipeline.Environment.Shaders["DirLightMaskDepthMap"_id],
                this->Pipeline.Lighting.DirectionalLights,
                ShadowMapGenerator::LoadStoreOptions::LOAD
            );
        }

        {
            MAKE_SCOPE_PROFILER("RenderController::PrepareSpotLightMaps()");
            generatorOpaque.GenerateFor(
                *this->Pipeline.Environment.Shaders["SpotLightDepthMap"_id], 
                this->Pipeline.Lighting.SpotLights,
                ShadowMapGenerator::LoadStoreOptions::CLEAR
            );
            generatorMasked.GenerateFor(
                *this->Pipeline.Environment.Shaders["SpotLightMaskDepthMap"_id],
                this->Pipeline.Lighting.SpotLights,
                ShadowMapGenerator::LoadStoreOptions::LOAD
            );
        }

        {
            MAKE_SCOPE_PROFILER("RenderController::PreparePointLightMaps()");
            generatorOpaque.GenerateFor(
                *this->Pipeline.Environment.Shaders["PointLightDepthMap"_id], 
                this->Pipeline.Lighting.PointLights,
                ShadowMapGenerator::LoadStoreOptions::CLEAR
            );
            generatorMasked.GenerateFor(
                *this->Pipeline.Environment.Shaders["PointLightDepthMap"_id],
                this->Pipeline.Lighting.PointLights,
                ShadowMapGenerator::LoadStoreOptions::LOAD
            );
        }
    }
    
    void RenderController::ComputeParticles(const MxVector<ParticleSystemUnit>& particleSystems)
    {
        if (particleSystems.empty()) return;
        MAKE_SCOPE_PROFILER("RenderController::ComputeParticles()");
        
        auto& computeShader = this->Pipeline.Environment.ComputeShaders["Particle"_id];
        computeShader->Bind();
        computeShader->SetUniform("dt", Min(Time::Delta(), 1.0f / 60.0f));

        this->Pipeline.Environment.RenderSSBO->BindBase(0);
        for (const auto& particleSystem : particleSystems)
        {
            computeShader->SetUniform("bufferOffset", (int)particleSystem.ParticleBufferOffset);
            computeShader->SetUniform("lifetime", particleSystem.ParticleLifetime);
            computeShader->SetUniform("spawnpoint", particleSystem.IsRelative ? Vector3(0.0f) : Vector3(particleSystem.Transform[3]));

            Compute::Dispatch(computeShader, particleSystem.InvocationCount, 1, 1);
        }
    }

    void RenderController::SortParticles(const CameraUnit& camera, MxVector<ParticleSystemUnit>& particleSystems)
    {
        std::sort(particleSystems.begin(), particleSystems.end(), 
            [&camera](const ParticleSystemUnit& p1, const ParticleSystemUnit& p2)
            {
                auto dist1 = camera.ViewportPosition - Vector3(p1.Transform[3]);
                auto dist2 = camera.ViewportPosition - Vector3(p2.Transform[3]);
                return Dot(dist1, dist1) > Dot(dist2, dist2);
            });
    }

    void RenderController::DrawParticles(const CameraUnit& camera, MxVector<ParticleSystemUnit>& particleSystems, const Shader& shader)
    {
        if (particleSystems.empty()) return;
        MAKE_SCOPE_PROFILER("RenderController::DrawParticles()");
        this->SortParticles(camera, particleSystems);

        shader.Bind();
        shader.IgnoreNonExistingUniform("viewportSize");
        shader.IgnoreNonExistingUniform("depthTex");
        shader.IgnoreNonExistingUniform("light");
        shader.IgnoreNonExistingUniform("light");
        shader.IgnoreNonExistingUniform("fading");
        shader.IgnoreNonExistingUniform("lifetime");
        shader.IgnoreNonExistingUniform("environment.skybox");
        shader.IgnoreNonExistingUniform("environment.irradiance");
        shader.IgnoreNonExistingUniform("environment.skyboxRotation");
        shader.IgnoreNonExistingUniform("environment.intensity");
        shader.IgnoreNonExistingUniform("environment.envBRDFLUT");

        auto viewportSize = MakeVector2((float)camera.OutputTexture->GetWidth(), (float)camera.OutputTexture->GetHeight());

        Texture::TextureBindId textureId = 0;
        this->BindSkyboxInformation(camera, shader, textureId);
        
        shader.SetUniform("viewportSize", viewportSize);
        shader.SetUniform("projMatrix", camera.ViewProjectionMatrix);
        shader.SetUniform("aspectRatio", camera.AspectRatio);
        shader.SetUniform("gamma", camera.Gamma);

        auto& particleMesh = this->Pipeline.Environment.RectangularObject;
        auto& VAO = particleMesh.GetVAO();
        VAO.Bind();

        camera.DepthTexture->Bind(textureId++);
        shader.SetUniform("depthTex", camera.DepthTexture->GetBoundId());
        shader.SetUniform("albedoTex", textureId);

        Compute::SetMemoryBarrier(BarrierType::SHADER_STORAGE_BUFFER);
        this->Pipeline.Environment.RenderSSBO->BindBase(0);

        for (const auto& particleSystem : particleSystems)
        {
            auto& material = this->Pipeline.MaterialUnits[particleSystem.MaterialIndex];

            material.AlbedoMap->Bind(textureId);

            Vector3 systemCenter = particleSystem.Transform[3];
            Vector3 normal = Normalize(camera.ViewportPosition - systemCenter);
            Vector3 totalLight{ 0.0f };
            for (const auto& dirLight : this->Pipeline.Lighting.DirectionalLights)
                totalLight += (0.5f * Dot(normal, dirLight.Direction) + 0.5f) * dirLight.Color * dirLight.Intensity * (1.0f + dirLight.AmbientIntensity);

            shader.SetUniform("normal", normal);
            shader.SetUniform("transform", particleSystem.IsRelative ? particleSystem.Transform : Matrix4x4(1.0f));
            shader.SetUniform("metallness", material.MetallicFactor);
            shader.SetUniform("roughness", material.RoughnessFactor);
            shader.SetUniform("color", material.BaseColor);
            shader.SetUniform("transparency", material.Transparency);
            shader.SetUniform("emmision", material.Emission);
            shader.SetUniform("light", totalLight);
            shader.SetUniform("bufferOffset", (int)particleSystem.ParticleBufferOffset);
            shader.SetUniform("lifetime", particleSystem.ParticleLifetime);
            shader.SetUniform("fading", particleSystem.Fading);

            this->DrawIndices(RenderPrimitive::TRIANGLES, particleMesh.IndexCount, 0, 0, particleSystem.InvocationCount * ParticleComputeGroupSize, 0);
        }
    }

    void RenderController::DrawObjects(const CameraUnit& camera, const Shader& shader, const RenderList& objects)
    {
        MAKE_SCOPE_PROFILER("RenderController::DrawObjects()");

        if (objects.UnitsIndex.empty()) return;
        shader.Bind();
        shader.IgnoreNonExistingUniform("camera.position");
        shader.IgnoreNonExistingUniform("camera.invViewProjMatrix");
        shader.IgnoreNonExistingUniform("material.transparency");

        this->BindCameraInformation(camera, shader);
        shader.SetUniform("gamma", camera.Gamma);

        size_t currentUnit = 0;
        this->Pipeline.Environment.RenderVAO->Bind();
        for (const auto& group : objects.Groups)
        {
            if (group.UnitCount == 0) continue;
            bool isInstanced = group.InstanceCount > 0;

            for (size_t i = 0; i < group.UnitCount; i++, currentUnit++)
            {
                const auto& unit = this->Pipeline.RenderUnits[objects.UnitsIndex[currentUnit]];
                bool isUnitVisible = isInstanced || camera.Culler.IsAABBVisible(unit.MinAABB, unit.MaxAABB);
                this->Pipeline.Statistics.AddEntry(isUnitVisible ? "drawn objects" : "culled objects", 1);

                if (isUnitVisible) this->DrawObject(unit, group.InstanceCount, group.BaseInstance, shader);
            }
        }
    }

    void RenderController::DrawObject(const RenderUnit& unit, size_t instanceCount, size_t baseInstance, const Shader& shader)
    {
        Texture::TextureBindId textureBindIndex = 0;
        const auto& material = this->Pipeline.MaterialUnits[unit.MaterialIndex];

        material.AlbedoMap->Bind(textureBindIndex++);
        material.MetallicMap->Bind(textureBindIndex++);
        material.RoughnessMap->Bind(textureBindIndex++);
        material.EmissiveMap->Bind(textureBindIndex++);
        material.NormalMap->Bind(textureBindIndex++);
        material.HeightMap->Bind(textureBindIndex++);
        material.AmbientOcclusionMap->Bind(textureBindIndex++);

        shader.SetUniform("map_albedo", material.AlbedoMap->GetBoundId());
        shader.SetUniform("map_metallic", material.MetallicMap->GetBoundId());
        shader.SetUniform("map_roughness", material.RoughnessMap->GetBoundId());
        shader.SetUniform("map_emmisive", material.EmissiveMap->GetBoundId());
        shader.SetUniform("map_normal", material.NormalMap->GetBoundId());
        shader.SetUniform("map_height", material.HeightMap->GetBoundId());
        shader.SetUniform("map_occlusion", material.AmbientOcclusionMap->GetBoundId());

        shader.SetUniform("material.roughness", material.RoughnessFactor);
        shader.SetUniform("material.metallic", material.MetallicFactor);
        shader.SetUniform("material.emmisive", material.Emission);
        shader.SetUniform("material.transparency", material.Transparency);

        shader.SetUniform("displacement", material.Displacement);
        shader.SetUniform("uvMultipliers", material.UVMultipliers);

        shader.SetUniform("parentModel", unit.ModelMatrix); //-V807
        shader.SetUniform("parentNormal", unit.NormalMatrix);
        shader.SetUniform("parentColor", material.BaseColor);
        
        this->DrawIndices(RenderPrimitive::TRIANGLES, unit.IndexCount, unit.IndexOffset, unit.VertexOffset, instanceCount, baseInstance);
    }

    void RenderController::ComputeBloomEffect(CameraUnit& camera, const TextureHandle& output)
    {
        if (camera.Effects == nullptr) return;
        if (camera.Effects->GetBloomIterations() == 0) return;
        MAKE_SCOPE_PROFILER("RenderController::PerformBloomIterarations()");

        auto& bloomTextures = this->Pipeline.Environment.BloomTextures;
        auto& splitShader = this->Pipeline.Environment.Shaders["BloomSplit"_id];

        float fogReduceFactor = camera.Effects->GetFogDistance() * std::exp(-25.0f * camera.Effects->GetFogDensity());
        float bloomWeight = camera.Effects->GetBloomWeight() * fogReduceFactor;

        camera.AlbedoTexture->Bind(0);
        splitShader->Bind();
        splitShader->SetUniform("albedoTex", camera.AlbedoTexture->GetBoundId());
        splitShader->SetUniform("weight", bloomWeight);

        auto& blurTarget = bloomTextures.front();
        auto& blurTemp = bloomTextures.back();
        this->RenderToTexture(blurTarget, splitShader);

        this->ApplyGaussianBlur(blurTarget, blurTemp, camera.Effects->GetBloomIterations());
        blurTarget->GenerateMipmaps();
        
        // use additive blending to apply bloom to camera HDR image
        this->GetRenderEngine().UseBlendFactors(BlendFactor::ONE, BlendFactor::ONE);
        this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(output);
        this->AttachFrameBufferNoClear(this->Pipeline.Environment.PostProcessFrameBuffer);
        this->SubmitImage(blurTarget);
        this->GetRenderEngine().UseBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);
    }

    void RenderController::ApplySSAO(CameraUnit& camera, TextureHandle& input, TextureHandle& temporary, TextureHandle& output)
    {
        if (camera.SSAO == nullptr || camera.SSAO->GetSampleCount() == 0) return;
        MAKE_SCOPE_PROFILER("RenderController::ComputeAmbientOcclusion()");

        auto& ssaoShader = this->Pipeline.Environment.Shaders["AmbientOcclusion"_id];
        ssaoShader->Bind();
        ssaoShader->IgnoreNonExistingUniform("materialTex");
        ssaoShader->IgnoreNonExistingUniform("albedoTex");
        ssaoShader->IgnoreNonExistingUniform("camera.position");

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *ssaoShader, textureId);
        this->BindCameraInformation(camera, *ssaoShader);

        ssaoShader->SetUniform("sampleCount", (int)camera.SSAO->GetSampleCount());
        ssaoShader->SetUniform("radius", camera.SSAO->GetRadius());

        auto& blurInputOutput = temporary;
        auto& blurTemporary = output;

        this->RenderToTexture(temporary, ssaoShader);

        this->ApplyGaussianBlur(blurInputOutput, blurTemporary, camera.SSAO->GetBlurIterations(), camera.SSAO->GetBlurLOD());

        auto& applyShader = this->Pipeline.Environment.Shaders["ApplyAmbientOcclusion"_id];
        applyShader->Bind();
        input->Bind(0);
        blurInputOutput->Bind(1);
        applyShader->SetUniform("inputTex", input->GetBoundId());
        applyShader->SetUniform("aoTex", blurInputOutput->GetBoundId());
        applyShader->SetUniform("intensity", camera.SSAO->GetIntensity());

        this->RenderToTexture(output, applyShader);
        std::swap(input, output);
    }

    TextureHandle RenderController::ComputeAverageWhite(CameraUnit& camera)
    {
        MAKE_SCOPE_PROFILER("RenderController::ComputeAverageWhite()");
        MX_ASSERT(camera.ToneMapping != nullptr);
        camera.HDRTexture->GenerateMipmaps();

        float dt = this->Pipeline.Environment.TimeDelta;
        float fadingAdaptationSpeed = 1.0f - std::exp(-camera.ToneMapping->GetEyeAdaptationSpeed() * dt);
        float adaptationThreshold = camera.ToneMapping->GetEyeAdaptationThreshold();

        auto& shader = this->Pipeline.Environment.Shaders["AverageWhite"_id];
        auto& output = this->Pipeline.Environment.AverageWhiteTexture;
        shader->Bind();
        camera.HDRTexture->Bind(0);
        camera.AverageWhiteTexture->Bind(1);
        shader->SetUniform("curFrameHDR", 0);
        shader->SetUniform("prevFrameWhite", 1);
        shader->SetUniform("adaptSpeed", fadingAdaptationSpeed);
        shader->SetUniform("adaptThreshold", adaptationThreshold);
        this->RenderToTexture(output, shader);
        output->GenerateMipmaps();
        this->CopyTexture(output, camera.AverageWhiteTexture);
        return output;
    }

    void RenderController::PerformPostProcessing(CameraUnit& camera)
    {
        MAKE_SCOPE_PROFILER("RenderController::PerformPostProcessing()");

        camera.AlbedoTexture->GenerateMipmaps();
        camera.MaterialTexture->GenerateMipmaps();
        camera.NormalTexture->GenerateMipmaps();
        camera.DepthTexture->GenerateMipmaps();
        
        this->ApplySSAO(camera, camera.HDRTexture, camera.SwapTexture1, camera.SwapTexture2);
        this->ApplySSR(camera, camera.HDRTexture, camera.SwapTexture1, camera.SwapTexture2);
         
        // render skybox & debug buffer (HDR texture is already attached)
        this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(camera.HDRTexture, Attachment::COLOR_ATTACHMENT0);
        this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(camera.DepthTexture, Attachment::DEPTH_ATTACHMENT);
        this->AttachFrameBufferNoClear(this->Pipeline.Environment.PostProcessFrameBuffer);
        this->GetRenderEngine().UseDepthBufferMask(false);
        this->DrawSkybox(camera);
        
        this->GetRenderEngine().UseBlendFactors(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA);
        this->ToggleFaceCulling(false);

        this->DrawTransparentObjects(camera);
        this->DrawParticles(camera, this->Pipeline.TransparentParticleSystems, *this->Pipeline.Environment.Shaders["ParticleTransparent"_id]);
        this->DrawDebugBuffer(camera);

        this->ToggleFaceCulling(true);
        this->GetRenderEngine().UseBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);

        this->GetRenderEngine().UseDepthBufferMask(true);
        this->Pipeline.Environment.PostProcessFrameBuffer->DetachExtraTarget(Attachment::DEPTH_ATTACHMENT);

        this->ComputeBloomEffect(camera, camera.HDRTexture);
        this->ApplySSGI(camera, camera.HDRTexture, camera.SwapTexture1, camera.SwapTexture2);
        
        this->ApplyGodRayEffect(camera, camera.HDRTexture, camera.SwapTexture1);
        this->ApplyChromaticAbberation(camera, camera.HDRTexture, camera.SwapTexture1);
        this->ApplyFogEffect(camera, camera.HDRTexture, camera.SwapTexture1);

        this->ApplyHDRToLDRConversion(camera, camera.HDRTexture, camera.SwapTexture1);

        this->ApplyFXAA(camera, camera.HDRTexture, camera.SwapTexture1);
        this->ApplyColorGrading(camera, camera.HDRTexture, camera.SwapTexture1);
        this->ApplyVignette(camera, camera.HDRTexture, camera.SwapTexture1);
    }

    void RenderController::DrawDirectionalLights(CameraUnit& camera, TextureHandle& output)
    {
        MAKE_SCOPE_PROFILER("RenderController::DrawDirectionalLights()");
        auto& shader = this->Pipeline.Environment.Shaders["DirLight"_id];
        shader->Bind();

        shader->IgnoreNonExistingUniform("camera.viewProjMatrix");
        shader->IgnoreNonExistingUniform("camera.position");
        shader->IgnoreNonExistingUniform("albedoTex");
        shader->IgnoreNonExistingUniform("materialTex");

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *shader, textureId);
        this->BindCameraInformation(camera, *shader);

        SubmitDirectionalLightInformation(shader, textureId);

        this->RenderToTextureNoClear(output, shader);
    }

    void RenderController::PerformLightPass(CameraUnit& camera)
    {
        this->DrawIBL(camera, camera.HDRTexture);

        this->GetRenderEngine().UseBlendFactors(BlendFactor::ONE, BlendFactor::ONE);

        this->DrawDirectionalLights(camera, camera.HDRTexture);

        // swap culling for light bounds
        this->ToggleFaceCulling(true, true, false);
        
        this->DrawShadowedSpotLights(camera, camera.HDRTexture);
        this->DrawNonShadowedSpotLights(camera, camera.HDRTexture);
        this->DrawShadowedPointLights(camera, camera.HDRTexture);
        this->DrawNonShadowedPointLights(camera, camera.HDRTexture);
        
        this->ToggleFaceCulling(true, true, true);

        this->GetRenderEngine().UseBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);
    }

    void RenderController::DrawTransparentObjects(CameraUnit& camera)
    {
        if (this->Pipeline.TransparentObjects.UnitsIndex.empty()) return;
        MAKE_SCOPE_PROFILER("RenderController::DrawTransparentObjects()");

        auto& shader = this->Pipeline.Environment.Shaders["Transparent"_id];
        shader->Bind();

        shader->SetUniform("viewportPosition", camera.ViewportPosition);
        shader->SetUniform("gamma", camera.Gamma);

        Texture::TextureBindId textureId = Material::TextureCount;
        this->BindSkyboxInformation(camera, *shader, textureId);

        // submit directional light information
        const auto& dirLights = this->Pipeline.Lighting.DirectionalLights;
        size_t lightCount = Min(MaxDirLightCount, dirLights.size());

        shader->SetUniform("lightCount", (int)lightCount);

        for (size_t i = 0; i < lightCount; i++)
        {
            auto& dirLight = this->Pipeline.Lighting.DirectionalLights[i];

            Vector4 colorPacked = Vector4(dirLight.Color * dirLight.Intensity, dirLight.AmbientIntensity);
            dirLight.ShadowMap->Bind(textureId++);
            shader->SetUniform(MxFormat("lights[{}].color", i), colorPacked);
            shader->SetUniform(MxFormat("lights[{}].direction", i), dirLight.Direction);
            shader->SetUniform(MxFormat("lightDepthMaps[{}]", i), dirLight.ShadowMap->GetBoundId());

            for (size_t j = 0; j < dirLight.BiasedProjectionMatrices.size(); j++)
            {
                shader->SetUniform(MxFormat("lights[{}].transform[{}]", i, j), dirLight.BiasedProjectionMatrices[j]);
            }
        }

        this->Pipeline.Environment.DefaultShadowMap->Bind(textureId);
        for (size_t i = lightCount; i < MaxDirLightCount; i++)
        {
            shader->SetUniform(MxFormat("lightDepthMaps[{}]", i), this->Pipeline.Environment.DefaultShadowMap->GetBoundId());
        }

        this->DrawObjects(camera, *shader, this->Pipeline.TransparentObjects);
    }

    void RenderController::DrawIBL(CameraUnit& camera, TextureHandle& output)
    {
        MAKE_SCOPE_PROFILER("RenderController::ApplyIBL()");

        auto shader = this->Pipeline.Environment.Shaders["IBL"_id];
        shader->Bind();
        shader->IgnoreNonExistingUniform("camera.viewProjMatrix");
        Texture::TextureBindId textureId = 0;

        this->BindGBuffer(camera, *shader, textureId);
        this->BindCameraInformation(camera, *shader);
        this->BindSkyboxInformation(camera, *shader, textureId);
        
        shader->SetUniform("gamma", camera.Gamma);

        this->RenderToTexture(output, shader);
    }

    void RenderController::SubmitDirectionalLightInformation(ShaderHandle& shader, Texture::TextureBindId textureId)
    {
        const auto& dirLights = this->Pipeline.Lighting.DirectionalLights;
        size_t lightCount = Min(MaxDirLightCount, dirLights.size());

        shader->SetUniform("lightCount", (int)lightCount);

        for (size_t i = 0; i < lightCount; i++)
        {
            auto& dirLight = this->Pipeline.Lighting.DirectionalLights[i];

            Vector4 colorPacked = Vector4(dirLight.Color * dirLight.Intensity, dirLight.AmbientIntensity);
            dirLight.ShadowMap->Bind(textureId++);
            shader->SetUniform(MxFormat("lights[{}].color", i), colorPacked);
            shader->SetUniform(MxFormat("lights[{}].direction", i), dirLight.Direction);
            shader->SetUniform(MxFormat("lightDepthMaps[{}]", i), dirLight.ShadowMap->GetBoundId());

            for (size_t j = 0; j < dirLight.BiasedProjectionMatrices.size(); j++)
            {
                shader->SetUniform(MxFormat("lights[{}].transform[{}]", i, j), dirLight.BiasedProjectionMatrices[j]);
            }
        }

        this->Pipeline.Environment.DefaultShadowMap->Bind(textureId);
        for (size_t i = lightCount; i < MaxDirLightCount; i++)
        {
            shader->SetUniform(MxFormat("lightDepthMaps[{}]", i), this->Pipeline.Environment.DefaultShadowMap->GetBoundId());
        }
    }

    void RenderController::ApplyGodRayEffect(CameraUnit& camera, TextureHandle& input, TextureHandle& output)
    {
        MAKE_SCOPE_PROFILER("RenderController::ApplyGodRayEffect()");
        if (!camera.GodRay)
            return;
        auto dirLightCount = this->Pipeline.Lighting.DirectionalLights.size();
        if (dirLightCount == 0)
            return;

        auto godRayShader = this->Pipeline.Environment.Shaders["GodRay"_id];
        godRayShader->Bind();
        godRayShader->IgnoreNonExistingUniform("camera.viewProjMatrix");
        godRayShader->IgnoreNonExistingUniform("normalTex");
        godRayShader->IgnoreNonExistingUniform("albedoTex");
        godRayShader->IgnoreNonExistingUniform("materialTex");

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *godRayShader, textureId);
        this->BindCameraInformation(camera, *godRayShader);

        input->Bind(textureId++);
        godRayShader->SetUniform("cameraOutput", input->GetBoundId());

        auto maxSteps = camera.GodRay->GetGodRayMaxSteps();
        auto beginStride = camera.GodRay->GetGodRaySampleStep();
        auto inflationRate = camera.GodRay->GetGodRayStepIncrement();
        auto maxDistance = beginStride * (1 - pow(inflationRate, maxSteps)) / (1 - inflationRate);
        godRayShader->SetUniform("maxSteps", maxSteps);
        godRayShader->SetUniform("sampleStep", beginStride);
        godRayShader->SetUniform("stepIncrement", inflationRate);
        godRayShader->SetUniform("maxDistance", maxDistance);

        SubmitDirectionalLightInformation(godRayShader, textureId);

        this->RenderToTexture(output, godRayShader); 
        std::swap(input, output);
    }
    void RenderController::ApplyFogEffect(CameraUnit& camera, TextureHandle& input, TextureHandle& output)
    {
        if (camera.Effects == nullptr || (camera.Effects->GetFogDistance() == 1.0 && camera.Effects->GetFogDensity() == 0.0f))
            return; // such parameters produce no fog. Do not do extra work calling this shader

        MAKE_SCOPE_PROFILER("RenderController::ApplyFogEffect()");

        auto fogShader = this->Pipeline.Environment.Shaders["Fog"_id];
        fogShader->Bind();
        fogShader->IgnoreNonExistingUniform("camera.viewProjMatrix");
        fogShader->IgnoreNonExistingUniform("normalTex");
        fogShader->IgnoreNonExistingUniform("albedoTex");
        fogShader->IgnoreNonExistingUniform("materialTex");

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *fogShader, textureId);
        this->BindFogInformation(camera, *fogShader);
        this->BindCameraInformation(camera, *fogShader);

        input->Bind(textureId++);
        fogShader->SetUniform("cameraOutput", input->GetBoundId());

        this->RenderToTexture(output, fogShader);
        std::swap(input, output);
    }

    void RenderController::ApplyChromaticAbberation(CameraUnit& camera, TextureHandle& input, TextureHandle& output)
    {
        if (camera.Effects == nullptr || camera.Effects->GetChromaticAberrationIntensity() <= 0.0f) return;
        MAKE_SCOPE_PROFILER("RenderController::ApplyChromaticAbberation()");

        auto& shader = this->Pipeline.Environment.Shaders["ChromaticAbberation"_id];
        shader->Bind();
        input->Bind(0);

        shader->SetUniform("tex", input->GetBoundId());
        shader->SetUniform("chromaticAbberationParams", Vector3{
            camera.Effects->GetChromaticAberrationMinDistance(),
            camera.Effects->GetChromaticAberrationIntensity(),
            camera.Effects->GetChromaticAberrationDistortion()
        });

        this->RenderToTexture(output, shader);
        std::swap(input, output);
    }

    void RenderController::ApplySSR(CameraUnit& camera, TextureHandle& input, TextureHandle& temporary, TextureHandle& output)
    {
        if (camera.SSR == nullptr || camera.SSR->GetSteps() == 0) return;
        MAKE_SCOPE_PROFILER("RenderController::ApplySSR()");

        auto& SSRShader = this->Pipeline.Environment.Shaders["SSR"_id];
        SSRShader->Bind();
        SSRShader->IgnoreNonExistingUniform("albedoTex");
        SSRShader->IgnoreNonExistingUniform("materialTex");
        
        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *SSRShader, textureId);
        this->BindCameraInformation(camera, *SSRShader);

        SSRShader->SetUniform("thickness", camera.SSR->GetThickness());
        SSRShader->SetUniform("startDistance", camera.SSR->GetStartDistance());
        SSRShader->SetUniform("steps", (int)camera.SSR->GetSteps());

        this->RenderToTexture(temporary, SSRShader);
        temporary->GenerateMipmaps();

        auto& applySSRShader = this->Pipeline.Environment.Shaders["ApplySSR"_id];
        applySSRShader->Bind();
        
        textureId = 0;
        camera.MaterialTexture->Bind(textureId++);
        camera.AlbedoTexture->Bind(textureId++);
        temporary->Bind(textureId++);
        input->Bind(textureId++);
        applySSRShader->SetUniform("albedoTex", camera.AlbedoTexture->GetBoundId());
        applySSRShader->SetUniform("materialTex", camera.MaterialTexture->GetBoundId());
        applySSRShader->SetUniform("SSRTex", temporary->GetBoundId());
        applySSRShader->SetUniform("HDRTex", input->GetBoundId());

        this->RenderToTexture(output, applySSRShader);
        std::swap(input, output);
    }

    void RenderController::ApplySSGI(CameraUnit& camera, TextureHandle& input, TextureHandle& temporary, TextureHandle& output)
    {
        if (camera.SSGI == nullptr || camera.SSGI->GetIntensity() == 0.0f) return;

        MAKE_SCOPE_PROFILER("RenderController::ApplySSR()");
        input->GenerateMipmaps();

        auto& SSGIShader = this->Pipeline.Environment.Shaders["SSGI"_id];
        SSGIShader->Bind();
        SSGIShader->IgnoreNonExistingUniform("albedoTex");
        SSGIShader->IgnoreNonExistingUniform("normalTex");
        SSGIShader->IgnoreNonExistingUniform("materialTex");
        SSGIShader->IgnoreNonExistingUniform("camera.position");

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *SSGIShader, textureId);
        this->BindCameraInformation(camera, *SSGIShader);

        input->Bind(textureId++);
        SSGIShader->SetUniform("inputTex", input->GetBoundId());
        SSGIShader->SetUniform("raySteps", (int)camera.SSGI->GetRaySteps());
        SSGIShader->SetUniform("intensity", camera.SSGI->GetIntensity());
        SSGIShader->SetUniform("distance", camera.SSGI->GetDistance());

        auto& blurInputOutput = this->Pipeline.Environment.BloomTextures.front();
        auto& blurTemporary = this->Pipeline.Environment.BloomTextures.back();

        this->RenderToTexture(blurInputOutput, SSGIShader);

        this->ApplyGaussianBlur(blurInputOutput, blurTemporary, camera.SSGI->GetBlurIterations(), camera.SSGI->GetBlurLOD());

        auto& applyShader = this->Pipeline.Environment.Shaders["ApplySSGI"_id];
        applyShader->Bind();
        applyShader->IgnoreNonExistingUniform("depthTex");
        applyShader->IgnoreNonExistingUniform("normalTex");

        textureId = 0;
        this->BindGBuffer(camera, *applyShader, textureId);

        input->Bind(textureId++);
        blurInputOutput->Bind(textureId++);
        applyShader->SetUniform("inputTex", input->GetBoundId());
        applyShader->SetUniform("SSGITex", blurInputOutput->GetBoundId());

        this->RenderToTexture(output, applyShader);

        std::swap(input, output);
    }

    void RenderController::ApplyHDRToLDRConversion(CameraUnit& camera, TextureHandle& input, TextureHandle& output)
    {
        if (camera.ToneMapping == nullptr) return;
        MAKE_SCOPE_PROFILER("RenderController::ApplyHDRToLDRConversion()");

        auto& HDRToLDRShader = this->Pipeline.Environment.Shaders["HDRToLDR"_id];
        auto averageWhite = this->ComputeAverageWhite(camera);
        auto aces = camera.ToneMapping->GetACESCoefficients();

        HDRToLDRShader->Bind();
        input->Bind(0);
        averageWhite->Bind(1);
        HDRToLDRShader->SetUniform("HDRTex", input->GetBoundId());
        HDRToLDRShader->SetUniform("averageWhiteTex", averageWhite->GetBoundId());

        HDRToLDRShader->SetUniform("exposure", camera.ToneMapping->GetExposure());
        HDRToLDRShader->SetUniform("colorMultiplier", camera.ToneMapping->GetColorScale());
        HDRToLDRShader->SetUniform("whitePoint", camera.ToneMapping->GetWhitePoint());
        HDRToLDRShader->SetUniform("minLuminance", camera.ToneMapping->GetMinLuminance());
        HDRToLDRShader->SetUniform("maxLuminance", camera.ToneMapping->GetMaxLuminance());
        HDRToLDRShader->SetUniform("ABCcoefsACES", Vector3{ aces.A, aces.B, aces.C });
        HDRToLDRShader->SetUniform("DEFcoefsACES", Vector3{ aces.D, aces.E, aces.F });

        HDRToLDRShader->SetUniform("gamma", camera.Gamma);

        this->RenderToTexture(output, HDRToLDRShader);
        std::swap(input, output);
    }

    void RenderController::ApplyFXAA(CameraUnit& camera, TextureHandle& input, TextureHandle& output)
    {
        if (camera.Effects == nullptr || !camera.Effects->IsFXAAEnabled()) return;
        MAKE_SCOPE_PROFILER("RenderController::ApplyFXAA");

        input->GenerateMipmaps();

        auto& fxaaShader = this->Pipeline.Environment.Shaders["FXAA"_id];
        fxaaShader->Bind();
        input->Bind(0);
        fxaaShader->SetUniform("tex", input->GetBoundId());
        
        this->RenderToTexture(output, fxaaShader);
        std::swap(input, output);
    }

    void RenderController::ApplyVignette(CameraUnit& camera, TextureHandle& input, TextureHandle& output)
    {
        if (camera.Effects == nullptr || camera.Effects->GetVignetteRadius() <= 0.0f) return;
        MAKE_SCOPE_PROFILER("RenderController::ApplyVignette");

        auto& vignetteShader = this->Pipeline.Environment.Shaders["Vignette"_id];
        vignetteShader->Bind();
        input->Bind(0);
        vignetteShader->SetUniform("tex", input->GetBoundId());

        vignetteShader->SetUniform("radius", camera.Effects->GetVignetteRadius());
        vignetteShader->SetUniform("intensity", camera.Effects->GetVignetteIntensity());

        this->RenderToTexture(output, vignetteShader);
        std::swap(input, output);
    }

    void RenderController::ApplyColorGrading(CameraUnit& camera, TextureHandle& input, TextureHandle& output)
    {
        if (camera.ToneMapping == nullptr) return;
        MAKE_SCOPE_PROFILER("RenderController::ApplyColorGrading");

        auto& colorGradingShader = this->Pipeline.Environment.Shaders["ColorGrading"_id];
        colorGradingShader->Bind();
        input->Bind(0);
        colorGradingShader->SetUniform("tex", input->GetBoundId());

        auto& colorGrading = camera.ToneMapping->GetColorGrading();
        colorGradingShader->SetUniform("channelR", colorGrading.R);
        colorGradingShader->SetUniform("channelG", colorGrading.G);
        colorGradingShader->SetUniform("channelB", colorGrading.B);

        this->RenderToTexture(output, colorGradingShader);
        std::swap(input, output);
    }

    void RenderController::DrawShadowedSpotLights(CameraUnit& camera, TextureHandle& output)
    {
        const auto& spotLights = this->Pipeline.Lighting.SpotLights;
        if (spotLights.empty()) return;
        MAKE_SCOPE_PROFILER("RenderController::DrawShadowedSpotLights()");

        auto shader = this->Pipeline.Environment.Shaders["SpotLightShadow"_id];
        shader->Bind();
        shader->IgnoreNonExistingUniform("camera.position");
        shader->IgnoreNonExistingUniform("albedoTex");
        shader->IgnoreNonExistingUniform("materialTex");

        auto& pyramid = this->Pipeline.Lighting.SpotLight;
        auto viewportSize = MakeVector2((float)camera.OutputTexture->GetWidth(), (float)camera.OutputTexture->GetHeight());

        shader->SetUniform("viewportSize", viewportSize);
        shader->SetUniform("castsShadows", true);

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *shader, textureId);
        this->BindCameraInformation(camera, *shader);
        
        shader->SetUniform("lightDepthMap", textureId);

        pyramid.GetVAO()->Bind();

        for (size_t i = 0; i < spotLights.size(); i++)
        {
            const auto& spotLight = spotLights[i];

            spotLight.ShadowMap->Bind(textureId);

            shader->SetUniform("worldToLightTransform", spotLight.BiasedProjectionMatrix);

            shader->SetUniform("transform", spotLight.Transform);
            shader->SetUniform("lightPosition", Vector4(spotLight.Position, spotLight.InnerAngle));
            shader->SetUniform("lightDirection", Vector4(spotLight.Direction, spotLight.OuterAngle));
            shader->SetUniform("colorParameters", Vector4(spotLight.Color, spotLight.AmbientIntensity));

            this->DrawIndices(RenderPrimitive::TRIANGLES, pyramid.GetIndexCount(), pyramid.GetIndexOffset(), pyramid.GetVertexOffset(), 0, 0);
        }
    }

    void RenderController::DrawShadowedPointLights(CameraUnit& camera, TextureHandle& output)
    {
        const auto& pointLights = this->Pipeline.Lighting.PointLights;
        if (pointLights.empty()) return;
        MAKE_SCOPE_PROFILER("RenderController::DrawShadowedPointLights()");

        auto shader = this->Pipeline.Environment.Shaders["PointLightShadow"_id];
        shader->Bind();
        shader->IgnoreNonExistingUniform("camera.position");
        shader->IgnoreNonExistingUniform("albedoTex");
        shader->IgnoreNonExistingUniform("materialTex");

        auto& sphere = this->Pipeline.Lighting.PointLight;
        auto viewportSize = MakeVector2((float)camera.OutputTexture->GetWidth(), (float)camera.OutputTexture->GetHeight());

        shader->SetUniform("viewportSize", viewportSize);
        shader->SetUniform("castsShadows", true);

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *shader, textureId);
        this->BindCameraInformation(camera, *shader);

        shader->SetUniform("lightDepthMap", textureId);

        sphere.GetVAO()->Bind();

        for (size_t i = 0; i < pointLights.size(); i++)
        {
            const auto& pointLight = pointLights[i];

            pointLight.ShadowMap->Bind(textureId);

            shader->SetUniform("transform", pointLight.Transform);
            shader->SetUniform("sphereParameters", Vector4(pointLight.Position, pointLight.Radius));
            shader->SetUniform("colorParameters", Vector4(pointLight.Color, pointLight.AmbientIntensity));

            this->DrawIndices(RenderPrimitive::TRIANGLES, sphere.GetIndexCount(), sphere.GetIndexOffset(), sphere.GetVertexOffset(), 0, 0);
        }
    }

    void RenderController::DrawNonShadowedPointLights(CameraUnit& camera, TextureHandle& output)
    {
        auto& instancedPointLights = this->Pipeline.Lighting.PointLightsInstanced;
        if (instancedPointLights.Instances.empty()) return;
        MAKE_SCOPE_PROFILER("RenderController::DrawNonShadowedPointLights()");

        auto shader = this->Pipeline.Environment.Shaders["PointLightNonShadow"_id];
        shader->Bind();
        shader->IgnoreNonExistingUniform("camera.position");
        shader->IgnoreNonExistingUniform("albedoTex");
        shader->IgnoreNonExistingUniform("materialTex");
        auto viewportSize = MakeVector2((float)camera.OutputTexture->GetWidth(), (float)camera.OutputTexture->GetHeight());

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *shader, textureId);
        this->BindCameraInformation(camera, *shader);

        this->Pipeline.Environment.DefaultShadowCubeMap->Bind(textureId++);

        shader->SetUniform("lightDepthMap", this->Pipeline.Environment.DefaultShadowCubeMap->GetBoundId());
        shader->SetUniform("viewportSize", viewportSize);
        shader->SetUniform("castsShadows", false);

        instancedPointLights.GetVAO()->Bind();

        this->DrawIndices(RenderPrimitive::TRIANGLES, 
            instancedPointLights.GetIndexCount(), instancedPointLights.GetIndexOffset(), 
            instancedPointLights.GetVertexOffset(), instancedPointLights.Instances.size(), 0
        );
    }

    void RenderController::DrawNonShadowedSpotLights(CameraUnit& camera, TextureHandle& output)
    {
        auto& instancedSpotLights = this->Pipeline.Lighting.SpotLightsInstanced;
        if (instancedSpotLights.Instances.empty()) return;
        MAKE_SCOPE_PROFILER("RenderController::DrawNonShadowedSpotLights()");

        auto shader = this->Pipeline.Environment.Shaders["SpotLightNonShadow"_id];
        shader->Bind();
        shader->IgnoreNonExistingUniform("camera.position");
        shader->IgnoreNonExistingUniform("albedoTex");
        shader->IgnoreNonExistingUniform("materialTex");
        auto viewportSize = MakeVector2((float)camera.OutputTexture->GetWidth(), (float)camera.OutputTexture->GetHeight());

        Texture::TextureBindId textureId = 0;
        this->BindGBuffer(camera, *shader, textureId);
        this->BindCameraInformation(camera, *shader);

        this->Pipeline.Environment.DefaultShadowCubeMap->Bind(textureId++);

        shader->SetUniform("lightDepthMap", this->Pipeline.Environment.DefaultShadowCubeMap->GetBoundId());
        shader->SetUniform("viewportSize", viewportSize);
        shader->SetUniform("castsShadows", false);

        instancedSpotLights.GetVAO()->Bind();

        this->DrawIndices(RenderPrimitive::TRIANGLES, 
            instancedSpotLights.GetIndexCount(), instancedSpotLights.GetIndexOffset(), 
            instancedSpotLights.GetVertexOffset(), instancedSpotLights.Instances.size(), 0
        );
    }

    void RenderController::SubmitInstancedLights()
    {
        this->Pipeline.Lighting.PointLightsInstanced.SubmitToVBO();
        this->Pipeline.Lighting.SpotLightsInstanced.SubmitToVBO();
    }

    void RenderController::BindFogInformation(const CameraUnit& camera, const Shader& shader)
    {
        MX_ASSERT(camera.Effects != nullptr);
        shader.SetUniform("fog.distance", camera.Effects->GetFogDistance());
        shader.SetUniform("fog.density", camera.Effects->GetFogDensity());
        shader.SetUniform("fog.color", camera.Effects->GetFogColor());
    }

    void RenderController::AttachDefaultVAO()
    {
        // simular to default framebuffer, we simply unbind any VAO to set it
        // to default (0) state
        this->Pipeline.Environment.RectangularObject.GetVAO().Unbind();
    }

    void RenderController::BindSkyboxInformation(const CameraUnit& camera, const Shader& shader, Texture::TextureBindId& startId)
    {
        camera.SkyboxTexture->Bind(startId++);
        camera.IrradianceTexture->Bind(startId++);
        this->Pipeline.Environment.EnvironmentBRDFLUT->Bind(startId++);
        shader.SetUniform("environment.skybox", camera.SkyboxTexture->GetBoundId());
        shader.SetUniform("environment.irradiance", camera.IrradianceTexture->GetBoundId());
        shader.SetUniform("environment.envBRDFLUT", this->Pipeline.Environment.EnvironmentBRDFLUT->GetBoundId());
        shader.SetUniform("environment.skyboxRotation", camera.InversedSkyboxRotation);
        shader.SetUniform("environment.intensity", camera.SkyboxIntensity);
    }

    void RenderController::BindCameraInformation(const CameraUnit& camera, const Shader& shader)
    {
        shader.SetUniform("camera.position", camera.ViewportPosition);
        shader.SetUniform("camera.viewProjMatrix", camera.ViewProjectionMatrix);
        shader.SetUniform("camera.invViewProjMatrix", camera.InverseViewProjMatrix);
    }

    void RenderController::BindGBuffer(const CameraUnit& camera, const Shader& shader, Texture::TextureBindId& startId)
    {
        camera.AlbedoTexture->Bind(startId++);
        camera.NormalTexture->Bind(startId++);
        camera.MaterialTexture->Bind(startId++);
        camera.DepthTexture->Bind(startId++);

        shader.SetUniform("albedoTex", camera.AlbedoTexture->GetBoundId());
        shader.SetUniform("normalTex", camera.NormalTexture->GetBoundId());
        shader.SetUniform("materialTex", camera.MaterialTexture->GetBoundId());
        shader.SetUniform("depthTex", camera.DepthTexture->GetBoundId());
    }

    const Renderer& RenderController::GetRenderEngine() const
    {
        return this->renderer;
    }

    Renderer& RenderController::GetRenderEngine()
    {
        return this->renderer;
    }

    void RenderController::Render() const
    {
        this->GetRenderEngine().Flush();
    }

    void RenderController::Clear() const
    {
        this->GetRenderEngine().Clear();
    }

    void RenderController::AttachDepthMap(const TextureHandle& texture)
    {
        auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
        framebuffer->AttachTexture(texture, Attachment::DEPTH_ATTACHMENT);
        this->AttachFrameBuffer(framebuffer);
    }

    void RenderController::AttachDepthMap(const CubeMapHandle& cubemap)
    {
        auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
        framebuffer->AttachCubeMap(cubemap, Attachment::DEPTH_ATTACHMENT);
        this->AttachFrameBuffer(framebuffer);
    }

    void RenderController::AttachDepthMapNoClear(const TextureHandle& texture)
    {
        auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
        framebuffer->AttachTexture(texture, Attachment::DEPTH_ATTACHMENT);
        this->AttachFrameBufferNoClear(framebuffer);
    }

    void RenderController::AttachDepthMapNoClear(const CubeMapHandle& cubemap)
    {
        auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
        framebuffer->AttachCubeMap(cubemap, Attachment::DEPTH_ATTACHMENT);
        this->AttachFrameBufferNoClear(framebuffer);
    }

    void RenderController::AttachFrameBuffer(const FrameBufferHandle& framebuffer)
    {
        this->AttachFrameBufferNoClear(framebuffer);
        this->Clear();
    }

    void RenderController::AttachFrameBufferNoClear(const FrameBufferHandle& framebuffer)
    {
        framebuffer->Bind();
        this->SetViewport(0, 0, (int)framebuffer->GetWidth(), (int)framebuffer->GetHeight());
    }

    void RenderController::AttachDefaultFrameBuffer()
    {
        // we can unbind any buffer to set target framebuffer to default one (id = 0)
        // doing this allows us to render to application window
        this->Pipeline.Environment.DepthFrameBuffer->Unbind();
        this->SetViewport(0, 0, this->Pipeline.Environment.Viewport.x, this->Pipeline.Environment.Viewport.y);
        this->Clear();
    }

    void RenderController::RenderToAttachedFrameBuffer(const Shader& shader)
    {
        this->Pipeline.Statistics.AddEntry("renders to framebuffer", 1);
        auto& rectangle = this->Pipeline.Environment.RectangularObject;
        shader.Bind();

        rectangle.GetVAO().Bind();
        this->DrawIndices(RenderPrimitive::TRIANGLES, rectangle.IndexCount, 0, 0, 0, 0);
    }

    void RenderController::RenderToFrameBuffer(const FrameBufferHandle& framebuffer, const ShaderHandle& shader)
    {
        this->AttachFrameBuffer(framebuffer);
        this->RenderToAttachedFrameBuffer(*shader);
    }

    void RenderController::RenderToFrameBufferNoClear(const FrameBufferHandle& framebuffer, const ShaderHandle& shader)
    {
        this->AttachFrameBufferNoClear(framebuffer);
        this->RenderToAttachedFrameBuffer(*shader);
    }

    void RenderController::RenderToTexture(const TextureHandle& texture, const ShaderHandle& shader, Attachment attachment)
    {
        this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(texture, attachment);
        this->RenderToFrameBuffer(this->Pipeline.Environment.PostProcessFrameBuffer, shader);
    }

    void RenderController::RenderToTextureNoClear(const TextureHandle& texture, const ShaderHandle& shader, Attachment attachment)
    {
        this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(texture, attachment);
        this->RenderToFrameBufferNoClear(this->Pipeline.Environment.PostProcessFrameBuffer, shader);
    }

    void RenderController::CopyTexture(const TextureHandle& input, const TextureHandle& output)
    {
        MAKE_SCOPE_PROFILER("RenderController::CopyTexture");
        this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(output);
        this->AttachFrameBufferNoClear(this->Pipeline.Environment.PostProcessFrameBuffer);
        this->SubmitImage(input);
    }

    void RenderController::ApplyGaussianBlur(const TextureHandle& inputOutput, const TextureHandle& temporary, size_t iterations, size_t lod)
    {
        if (iterations == 0) return;
        auto& shader = this->Pipeline.Environment.Shaders["GaussianBlur"_id];
        shader->Bind();
        shader->SetUniform("inputTex", 0);
        shader->SetUniform("lod", (int)lod);

        auto& framebuffer = this->Pipeline.Environment.BloomFrameBuffer;

        for (uint8_t i = 0; i < 2 * iterations; i++)
        {
            bool horizontalBlur = (i % 2 == 0);
            auto& source = horizontalBlur ? inputOutput : temporary;
            auto& target = horizontalBlur ? temporary : inputOutput;
            shader->SetUniform("horizontalBlur", horizontalBlur);

            if (lod != 0) source->GenerateMipmaps();
            source->Bind(0);

            framebuffer->AttachTexture(target);
            this->RenderToFrameBuffer(framebuffer, shader);
        }
    }

    void RenderController::DrawVertices(RenderPrimitive primitive, size_t vertexCount, size_t vertexOffset, size_t instanceCount, size_t baseInstance)
    {
        this->Pipeline.Statistics.AddEntry("draw calls", 1);
        this->Pipeline.Statistics.AddEntry("drawn vertecies", vertexCount * Max(baseInstance, 1));
        if (instanceCount == 0)
        {
            this->GetRenderEngine().DrawVertices(primitive, vertexCount, vertexOffset);
        }
        else
        {
            this->GetRenderEngine().DrawVerticesInstanced(primitive, vertexCount, vertexOffset, instanceCount, baseInstance);
        }
    }

    void RenderController::DrawIndices(RenderPrimitive primitive, size_t indexCount, size_t indexOffset, size_t baseVertex, size_t instanceCount, size_t baseInstance)
    {
        this->Pipeline.Statistics.AddEntry("draw calls", 1);
        this->Pipeline.Statistics.AddEntry("drawn vertecies", indexCount * Max(instanceCount, 1));
        if (instanceCount == 0)
        {
            this->GetRenderEngine().DrawIndicesBaseVertex(primitive, indexCount, indexOffset, baseVertex);
        }
        else
        {
            this->GetRenderEngine().DrawIndicesBaseVertexInstanced(primitive, indexCount, indexOffset, baseVertex, instanceCount, baseInstance);
        }
    }

    void RenderController::ToggleDepthOnlyMode(bool value)
    {
        bool useColor = !value;
        this->GetRenderEngine().UseColorMask(useColor, useColor, useColor, useColor);
    }

    void RenderController::ToggleReversedDepth(bool value)
    {
        this->GetRenderEngine().UseReversedDepth(value);
    }

    void RenderController::ToggleFaceCulling(bool value, bool counterClockWise, bool cullBack)
    {
        this->GetRenderEngine().UseCulling(value, counterClockWise, cullBack);
    }

    void RenderController::SetAnisotropicFiltering(float value)
    {
        this->GetRenderEngine().UseAnisotropicFiltering(value);
    }

    void RenderController::SetViewport(int x, int y, int width, int height)
    {
        this->GetRenderEngine().SetViewport(x, y, width, height);
    }

    void RenderController::DrawSkybox(const CameraUnit& camera)
    {
        MAKE_SCOPE_PROFILER("RenderController::DrawSkybox()");

        auto& shader = *this->Pipeline.Environment.Shaders["Skybox"_id];
        auto& skybox = this->Pipeline.Environment.SkyboxCubeObject;

        float skyLuminance = camera.SkyboxIntensity;
        for (size_t i = 0; i < Min(this->Pipeline.Lighting.DirectionalLights.size(), MaxDirLightCount); i++)
        {
            const auto& dirLight = this->Pipeline.Lighting.DirectionalLights[i];
            skyLuminance += dirLight.Intensity;
        }

        shader.Bind();
        shader.SetUniform("StaticViewProjection", camera.StaticViewProjectionMatrix);
        shader.SetUniform("Rotation", Transpose(camera.InversedSkyboxRotation));
        shader.SetUniform("gamma", camera.Gamma);
        shader.SetUniform("luminance", skyLuminance);
        camera.SkyboxTexture->Bind(0);
        shader.SetUniform("skybox", camera.SkyboxTexture->GetBoundId());

        skybox.GetVAO().Bind();

        this->DrawIndices(RenderPrimitive::TRIANGLES, skybox.IndexCount, 0, 0, 0, 0);
    }

    void RenderController::DrawDebugBuffer(const CameraUnit& camera)
    {
        if (this->Pipeline.Environment.DebugBufferObject.VertexCount == 0) return;
        MAKE_SCOPE_PROFILER("RenderController::DrawDebugBuffer()");

        auto& shader = *this->Pipeline.Environment.Shaders["DebugDraw"_id];
        shader.Bind();
        shader.SetUniform("ViewProjMatrix", camera.ViewProjectionMatrix);

        // TODO: refactor
        auto& VAO = *this->Pipeline.Environment.DebugBufferObject.VAO;
        VAO.Bind();

        this->DrawVertices(RenderPrimitive::LINES, this->Pipeline.Environment.DebugBufferObject.VertexCount, 0, 0, 0);
    }

    EnvironmentUnit& RenderController::GetEnvironment()
    {
        return this->Pipeline.Environment;
    }

    const EnvironmentUnit& RenderController::GetEnvironment() const
    {
        return this->Pipeline.Environment;
    }

    LightingSystem& RenderController::GetLightInformation()
    {
        return this->Pipeline.Lighting;
    }

    const LightingSystem& RenderController::GetLightInformation() const
    {
        return this->Pipeline.Lighting;
    }

    const RenderStatistics& RenderController::GetRenderStatistics() const
    {
        return this->Pipeline.Statistics;
        return this->Pipeline.Statistics;
    }

    RenderStatistics& RenderController::GetRenderStatistics()
    {
        return this->Pipeline.Statistics;
    }

    void RenderController::ResetPipeline()
    {
        this->Pipeline.Lighting.DirectionalLights.clear();
        this->Pipeline.Lighting.PointLightsInstanced.Instances.clear();
        this->Pipeline.Lighting.SpotLightsInstanced.Instances.clear();
        this->Pipeline.Lighting.PointLights.clear();
        this->Pipeline.Lighting.SpotLights.clear();
        this->Pipeline.ShadowCasters.Groups.clear();
        this->Pipeline.ShadowCasters.UnitsIndex.clear();
        this->Pipeline.MaskedShadowCasters.Groups.clear();
        this->Pipeline.MaskedShadowCasters.UnitsIndex.clear();
        this->Pipeline.TransparentObjects.Groups.clear();
        this->Pipeline.TransparentObjects.UnitsIndex.clear();
        this->Pipeline.MaskedObjects.Groups.clear();
        this->Pipeline.MaskedObjects.UnitsIndex.clear();
        this->Pipeline.OpaqueObjects.Groups.clear();
        this->Pipeline.OpaqueObjects.UnitsIndex.clear();
        this->Pipeline.RenderUnits.clear();
        this->Pipeline.OpaqueParticleSystems.clear();
        this->Pipeline.TransparentParticleSystems.clear();
        this->Pipeline.MaterialUnits.clear();
        this->Pipeline.Cameras.clear();
    }

    void RenderController::SubmitParticleSystem(const ParticleSystem& system, const Material& material, const Transform& parentTransform)
    {
        if (material.Transparency == 0.0f) return;
        bool isTransparent = (material.AlphaMode == AlphaModeGroup::TRANSPARENT);

        auto& particleSystem = (isTransparent ? this->Pipeline.TransparentParticleSystems : this->Pipeline.OpaqueParticleSystems).emplace_back();
        particleSystem.ParticleBufferOffset = system.GetParticleAllocationOffset();
        particleSystem.ParticleLifetime = system.GetParticleLifetime();
        particleSystem.Fading = system.GetFading();
        particleSystem.IsRelative = system.IsRelative();
        particleSystem.InvocationCount = system.GetMaxParticleCount() / ParticleComputeGroupSize;
        particleSystem.MaterialIndex = this->Pipeline.MaterialUnits.size();

        parentTransform.GetMatrix(particleSystem.Transform);
        
        auto& materialCopy = this->Pipeline.MaterialUnits.emplace_back(material);
        if (!materialCopy.AlbedoMap.IsValid()) materialCopy.AlbedoMap = this->Pipeline.Environment.DefaultMaterialMap;
    }

    void RenderController::SubmitLightSource(const DirectionalLight& light, const Transform& parentTransform)
    {
        auto& dirLight = this->Pipeline.Lighting.DirectionalLights.emplace_back();
        MX_ASSERT(dirLight.ProjectionMatrices.size() == DirectionalLight::TextureCount);
        MX_ASSERT(dirLight.BiasedProjectionMatrices.size() == DirectionalLight::TextureCount);

        dirLight.AmbientIntensity = light.GetAmbientIntensity();
        dirLight.Intensity = light.GetIntensity();
        dirLight.Color = light.GetColor();
        dirLight.Direction = Normalize(light.Direction);

        constexpr auto ProjectionBiasMatrix = [](size_t projectionIndex)
        {
            float scale = 1.0f / DirectionalLight::TextureCount;
            float offset = projectionIndex * scale;

            Matrix4x4 Result(
                0.5f * scale,          0.0f, 0.0f, 0.0f,
                0.0f,                  0.5f, 0.0f, 0.0f,
                0.0f,                  0.0f, 0.5f, 0.0f,
                0.5f * scale + offset, 0.5f, 0.5f, 1.0f
            );
            return Result;
        };

        dirLight.ShadowMap = light.DepthMap;
        for (size_t i = 0; i < dirLight.ProjectionMatrices.size(); i++)
        {
            dirLight.ProjectionMatrices[i] = light.GetMatrix(parentTransform.GetPosition(), i);
            dirLight.BiasedProjectionMatrices[i] = ProjectionBiasMatrix(i) * dirLight.ProjectionMatrices[i];
        }
    }

    void RenderController::SubmitLightSource(const PointLight& light, const Transform& parentTransform)
    {
        PointLightBaseData* baseLightData = nullptr;
        if (light.IsCastingShadows())
        {
            auto& pointLight = this->Pipeline.Lighting.PointLights.emplace_back();
            baseLightData = &pointLight;

            pointLight.ShadowMap = light.DepthMap;
            for (size_t i = 0; i < std::size(pointLight.ProjectionMatrices); i++)
                pointLight.ProjectionMatrices[i] = light.GetMatrix(i, parentTransform.GetPosition());
        }
        else
        {
            auto& pointLight = this->Pipeline.Lighting.PointLightsInstanced.Instances.emplace_back();
            baseLightData = &pointLight;
        }

        baseLightData->AmbientIntensity = light.GetAmbientIntensity();
        baseLightData->Color = light.GetIntensity() * light.GetColor();
        baseLightData->Position = parentTransform.GetPosition();
        baseLightData->Radius = light.GetRadius();
        baseLightData->Transform = light.GetSphereTransform(parentTransform.GetPosition());
    }

    void RenderController::SubmitLightSource(const SpotLight& light, const Transform& parentTransform)
    {
        SpotLightBaseData* baseLightData = nullptr;
        if (light.IsCastingShadows())
        {
            auto& spotLight = this->Pipeline.Lighting.SpotLights.emplace_back();
            baseLightData = &spotLight;

            spotLight.ProjectionMatrix = light.GetMatrix(parentTransform.GetPosition());
            spotLight.BiasedProjectionMatrix = MakeBiasMatrix() * light.GetMatrix(parentTransform.GetPosition());
            spotLight.ShadowMap = light.DepthMap;
        }
        else
        {
            auto& spotLight = this->Pipeline.Lighting.SpotLightsInstanced.Instances.emplace_back();
            baseLightData = &spotLight;
        }

        baseLightData->AmbientIntensity = light.GetAmbientIntensity();
        baseLightData->Color = light.GetIntensity() * light.GetColor();
        baseLightData->Position = parentTransform.GetPosition();
        baseLightData->Transform = light.GetPyramidTransform(parentTransform.GetPosition());
        baseLightData->InnerAngle = light.GetInnerCos();
        baseLightData->OuterAngle = light.GetOuterCos();
        // pack max distance to normalized direction vector (see spotlight vertex shader)
        baseLightData->Direction = light.GetMaxDistance() * Normalize(light.Direction);
    }

    void RenderController::SubmitCamera( 
        const CameraController& controller,
        const Transform& parentTransform, 
        const Skybox* skybox,
        const CameraEffects* effects,
        const CameraToneMapping* toneMapping, 
        const CameraSSR* ssr,
        const CameraSSGI* ssgi, 
        const CameraSSAO* ssao,
        const GodRayEffect* godRay)
    {
        auto& camera = this->Pipeline.Cameras.emplace_back();

        camera.ViewportPosition           = parentTransform.GetPosition();
        camera.AspectRatio                = controller.Camera.GetAspectRatio();
        camera.StaticViewProjectionMatrix = controller.GetMatrix(MakeVector3(0.0f));
        camera.ViewProjectionMatrix       = controller.GetMatrix(parentTransform.GetPosition());
        camera.InverseViewProjMatrix      = Inverse(camera.ViewProjectionMatrix);
        camera.Culler                     = controller.GetFrustrumCuller();
        camera.IsPerspective              = controller.GetCameraType() == CameraType::PERSPECTIVE;
        camera.GBuffer                    = controller.GetGBuffer();
        camera.AlbedoTexture              = controller.GetAlbedoTexture();
        camera.NormalTexture              = controller.GetNormalTexture();
        camera.MaterialTexture            = controller.GetMaterialTexture();
        camera.DepthTexture               = controller.GetDepthTexture();
        camera.AverageWhiteTexture        = controller.GetAverageWhiteTexture();
        camera.HDRTexture                 = controller.GetHDRTexture();
        camera.SwapTexture1               = controller.GetSwapHDRTexture1();
        camera.SwapTexture2               = controller.GetSwapHDRTexture2();
        camera.OutputTexture              = controller.GetRenderTexture();
        camera.RenderToTexture            = controller.IsRendering();
        camera.SkyboxTexture              = (skybox != nullptr && skybox->CubeMap.IsValid()) ? skybox->CubeMap : this->Pipeline.Environment.DefaultSkybox;
        camera.IrradianceTexture          = (skybox != nullptr && skybox->Irradiance.IsValid()) ? skybox->Irradiance : camera.SkyboxTexture;
        camera.SkyboxIntensity            = (skybox != nullptr) ? skybox->GetIntensity() : Skybox::DefaultIntensity;
        camera.InversedSkyboxRotation     = (skybox != nullptr) ? Transpose(MakeRotationMatrix(RadiansVec(skybox->GetRotation()))) : Matrix3x3(1.0f);
        camera.Gamma                      = (toneMapping != nullptr) ? toneMapping->GetGamma() : CameraToneMapping::DefaultGamma;
        camera.Effects                    = effects;
        camera.ToneMapping                = toneMapping;
        camera.SSR                        = ssr;
        camera.SSGI                       = ssgi;
        camera.SSAO                       = ssao;
        camera.GodRay                     = godRay;
    }

    size_t RenderController::SubmitRenderGroup(const Mesh& mesh, size_t instanceOffset, size_t instanceCount)
    {
        size_t renderGroupIndex = this->Pipeline.OpaqueObjects.Groups.size();

        std::array groupSubTypes = {
            std::ref(this->Pipeline.OpaqueObjects.Groups.emplace_back()),
            std::ref(this->Pipeline.MaskedObjects.Groups.emplace_back()),
            std::ref(this->Pipeline.TransparentObjects.Groups.emplace_back()),
            std::ref(this->Pipeline.ShadowCasters.Groups.emplace_back()),
            std::ref(this->Pipeline.MaskedShadowCasters.Groups.emplace_back()),
        };

        for (auto subType : groupSubTypes)
        {
            subType.get().BaseInstance = instanceOffset;
            subType.get().InstanceCount = instanceCount;
            subType.get().UnitCount = 0;
        }

        return renderGroupIndex;
    }

    void RenderController::SubmitRenderUnit(size_t renderGroupIndex, const SubMesh& submesh, const Material& material, const Transform& parentTransform, bool castsShadow, const char* debugName)
    {
        bool isInvisible = material.Transparency == 0.0f;
        bool isTransparent = material.AlphaMode == AlphaModeGroup::TRANSPARENT;
        bool isMasked = material.AlphaMode == AlphaModeGroup::MASKED && material.Transparency < 1.0f;
        if (isInvisible) return;

        size_t unitIndex = this->Pipeline.RenderUnits.size();
        auto& renderUnit = this->Pipeline.RenderUnits.emplace_back();

        renderUnit.MaterialIndex = this->Pipeline.MaterialUnits.size();
        renderUnit.IndexCount = submesh.Data.GetIndiciesCount();
        renderUnit.IndexOffset = submesh.Data.GetIndiciesOffset();
        renderUnit.VertexCount = submesh.Data.GetVerteciesCount();
        renderUnit.VertexOffset = submesh.Data.GetVerteciesOffset();
        renderUnit.ModelMatrix = parentTransform.GetMatrix() * submesh.GetTransform().GetMatrix(); //-V807
        renderUnit.NormalMatrix = parentTransform.GetNormalMatrix() * submesh.GetTransform().GetNormalMatrix();

        #if defined(MXENGINE_DEBUG)
        renderUnit.DebugName = debugName;
        #endif

        // compute aabb of primitive object for later frustrum culling
        auto aabb = submesh.Data.GetAABB() * renderUnit.ModelMatrix;
        renderUnit.MinAABB = aabb.Min;
        renderUnit.MaxAABB = aabb.Max;

        if (castsShadow)
        {
            auto& groupList = isMasked ? this->Pipeline.MaskedShadowCasters : this->Pipeline.ShadowCasters;
            groupList.Groups[renderGroupIndex].UnitCount++;
            groupList.UnitsIndex.push_back(unitIndex);
        }

        if (isTransparent)
        {
            auto& transparentObjects = this->Pipeline.TransparentObjects;
            transparentObjects.Groups[renderGroupIndex].UnitCount++;
            transparentObjects.UnitsIndex.push_back(unitIndex);
        }
        else if (isMasked)
        {
            auto& maskedObjects = this->Pipeline.MaskedObjects;
            maskedObjects.Groups[renderGroupIndex].UnitCount++;
            maskedObjects.UnitsIndex.push_back(unitIndex);
        }
        else
        {
            auto& opaqueObjects = this->Pipeline.OpaqueObjects;
            opaqueObjects.Groups[renderGroupIndex].UnitCount++;
            opaqueObjects.UnitsIndex.push_back(unitIndex);
        }

        // submit render material
        auto& renderMaterial = this->Pipeline.MaterialUnits.emplace_back(material); // create a copy of material for future work

        // we need to change displacement to account object scale, so we take average of object scale components as multiplier
        renderMaterial.Displacement *= Dot(parentTransform.GetScale() * submesh.GetTransform().GetScale(), MakeVector3(1.0f / 3.0f));

        if (renderMaterial.RoughnessMap.IsValid())         renderMaterial.RoughnessFactor = 1.0f;
        if (renderMaterial.MetallicMap.IsValid())          renderMaterial.MetallicFactor = 1.0f;

        // set default textures if they are not exist
        if (!renderMaterial.AlbedoMap.IsValid())           renderMaterial.AlbedoMap = this->Pipeline.Environment.DefaultMaterialMap;
        if (!renderMaterial.RoughnessMap.IsValid())        renderMaterial.RoughnessMap = this->Pipeline.Environment.DefaultMaterialMap;
        if (!renderMaterial.MetallicMap.IsValid())         renderMaterial.MetallicMap = this->Pipeline.Environment.DefaultMaterialMap;
        if (!renderMaterial.EmissiveMap.IsValid())         renderMaterial.EmissiveMap = this->Pipeline.Environment.DefaultMaterialMap;
        if (!renderMaterial.AmbientOcclusionMap.IsValid()) renderMaterial.AmbientOcclusionMap = this->Pipeline.Environment.DefaultMaterialMap;
        if (!renderMaterial.NormalMap.IsValid())           renderMaterial.NormalMap = this->Pipeline.Environment.DefaultNormalMap;
        if (!renderMaterial.HeightMap.IsValid())           renderMaterial.HeightMap = this->Pipeline.Environment.DefaultBlackMap;
    }

    void RenderController::SubmitImage(const TextureHandle& texture)
    {
        auto& finalShader = *this->Pipeline.Environment.Shaders["ImageForward"_id];
        auto& rectangle = this->Pipeline.Environment.RectangularObject;

        finalShader.Bind();
        finalShader.SetUniform("tex", 0);
        texture->Bind(0);

        rectangle.GetVAO().Bind();

        this->DrawIndices(RenderPrimitive::TRIANGLES, rectangle.IndexCount, 0, 0, 0, 0);
    }

    void RenderController::StartPipeline()
    {
        MAKE_SCOPE_PROFILER("RenderController::StartPipeline()");
        if (this->Pipeline.Cameras.empty())
        {
            if(this->Pipeline.Environment.RenderToDefaultFrameBuffer)
                this->AttachDefaultFrameBuffer();
            return;
        }

        this->SubmitInstancedLights();
        this->ComputeParticles(this->Pipeline.OpaqueParticleSystems);
        this->ComputeParticles(this->Pipeline.TransparentParticleSystems);

        this->PrepareShadowMaps();

        for (auto& camera : this->Pipeline.Cameras)
        {
            if (!camera.RenderToTexture) continue;

            this->GetRenderEngine().UseBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);
            this->ToggleReversedDepth(camera.IsPerspective);
            this->AttachFrameBuffer(camera.GBuffer);

            this->DrawObjects(camera, *this->Pipeline.Environment.Shaders["GBuffer"_id], this->Pipeline.OpaqueObjects);
            this->DrawObjects(camera, *this->Pipeline.Environment.Shaders["GBufferMask"_id], this->Pipeline.MaskedObjects);
            this->DrawParticles(camera, this->Pipeline.OpaqueParticleSystems, *this->Pipeline.Environment.Shaders["ParticleOpaque"_id]);

            this->PerformLightPass(camera);
            this->PerformPostProcessing(camera);

            this->CopyTexture(camera.HDRTexture, camera.OutputTexture);
            camera.OutputTexture->GenerateMipmaps();
        }
    }

    void RenderController::EndPipeline()
    {
        MAKE_SCOPE_PROFILER("RenderController::SubmitFinalImage");
        this->AttachDefaultFrameBuffer();
        if (this->Pipeline.Environment.RenderToDefaultFrameBuffer && this->Pipeline.Environment.MainCameraIndex < this->Pipeline.Cameras.size())
        {
            const auto& mainCamera = this->Pipeline.Cameras[this->Pipeline.Environment.MainCameraIndex];
            MX_ASSERT(mainCamera.OutputTexture.IsValid());

            this->SubmitImage(mainCamera.OutputTexture);
        }
        this->AttachDefaultVAO();
    }
}