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

#include "Core/BoundingObjects/FrustrumCuller.h"
#include "RenderObjects/RectangleObject.h"
#include "RenderObjects/SkyboxObject.h"
#include "RenderObjects/RenderHelperObject.h"
#include "RenderObjects/PointLightInstancedObject.h"
#include "RenderObjects/SpotLightInstancedObject.h"
#include "RenderUtilities/RenderStatistics.h"
#include "Core/Resources/ACESCurve.h"
#include "Core/Resources/Material.h"
#include "Utilities/String/String.h"

namespace MxEngine
{
    class Skybox;
    class CameraEffects;
    class CameraToneMapping;
    class CameraSSR;
    class CameraSSGI;
    class CameraSSAO;
    class CameraGodRay;
    class CameraLensFlare;
    
    struct DebugBufferUnit
    {
        VertexArrayHandle VAO;
        size_t VertexCount;
    };

    struct CameraUnit
    {
        FrameBufferHandle GBuffer;
        TextureHandle AlbedoTexture;
        TextureHandle NormalTexture;
        TextureHandle MaterialTexture;
        TextureHandle DepthTexture;
        TextureHandle AverageWhiteTexture;
        TextureHandle HDRTexture;
        TextureHandle SwapTexture1;
        TextureHandle SwapTexture2;
        TextureHandle SwapQuaterTexture1;
        TextureHandle SwapQuaterTexture2;
        TextureHandle SwapQuaterTexture3;

        FrustrumCuller Culler;
        Matrix4x4 InverseViewProjMatrix;
        Matrix4x4 ViewProjectionMatrix;
        Matrix4x4 StaticViewProjectionMatrix;

        TextureHandle OutputTexture;
        Vector3 ViewportPosition;

        float SkyboxIntensity;
        Matrix3x3 InversedSkyboxRotation;
        CubeMapHandle SkyboxTexture;
        CubeMapHandle IrradianceTexture;

        float Gamma;
        float AspectRatio;

        bool IsPerspective;
        bool RenderToTexture;

        const CameraEffects* Effects;
        const CameraToneMapping* ToneMapping;
        const CameraSSR* SSR;
        const CameraSSGI* SSGI;
        const CameraSSAO* SSAO;
        const CameraGodRay* GodRay;
        const CameraLensFlare* LensFlare;
    };

    struct EnvironmentUnit
    {
        MxHashMap<StringId, ShaderHandle> Shaders;
        MxHashMap<StringId, ComputeShaderHandle> ComputeShaders;

        TextureHandle DefaultMaterialMap;
        TextureHandle DefaultNormalMap;
        TextureHandle DefaultBlackMap;
        TextureHandle DefaultGreyMap;
        TextureHandle DefaultShadowMap;
        TextureHandle AverageWhiteTexture;
        TextureHandle DownSampleTexture;
        TextureHandle EnvironmentBRDFLUT;
        CubeMapHandle DefaultShadowCubeMap;
        CubeMapHandle DefaultSkybox;

        VertexArrayHandle RenderVAO;
        ShaderStorageBufferHandle RenderSSBO;

        FrameBufferHandle DepthFrameBuffer;
        FrameBufferHandle PostProcessFrameBuffer;
        FrameBufferHandle BloomFrameBuffer;
        std::array<TextureHandle, 2> BloomTextures;

        SkyboxObject SkyboxCubeObject;
        DebugBufferUnit DebugBufferObject;
        RectangleObject RectangularObject;
        size_t DefaultBaseInstance;

        VectorInt2 Viewport;
        float TimeDelta;

        uint8_t MainCameraIndex;
        bool OverlayDebugDraws;
        bool RenderToDefaultFrameBuffer;
    };

    struct DirectionalLightUnit
    {
        TextureHandle ShadowMap;
        std::array<Matrix4x4, 3> ProjectionMatrices;
        std::array<Matrix4x4, 3> BiasedProjectionMatrices;
        Vector3 Direction;
        float AmbientIntensity;
        Vector3 Color;
        float Intensity;
    };

    struct PointLightUnit : PointLightBaseData
    {
        CubeMapHandle ShadowMap;
        Matrix4x4 ProjectionMatrices[6];
    };

    struct SpotLightUnit : SpotLightBaseData
    {
        TextureHandle ShadowMap;
        Matrix4x4 ProjectionMatrix;
        Matrix4x4 BiasedProjectionMatrix;
    };

    struct LightingSystem
    {
        MxVector<DirectionalLightUnit> DirectionalLights;
        MxVector<PointLightUnit> PointLights;
        MxVector<SpotLightUnit> SpotLights;
        SpotLightInstancedObject SpotLightsInstanced;
        PointLightInstancedObject PointLightsInstanced;
        RenderHelperObject PointLight;
        RenderHelperObject SpotLight;
    };

    struct RenderGroup
    {
        size_t BaseInstance;
        size_t InstanceCount;
        size_t UnitCount;
    };

    struct RenderUnit
    {
        size_t MaterialIndex;
        size_t VertexOffset;
        size_t VertexCount;
        size_t IndexOffset;
        size_t IndexCount;
        
        Matrix4x4 ModelMatrix;
        Matrix3x3 NormalMatrix;

        Vector3 MinAABB, MaxAABB;
        #if defined(MXENGINE_DEBUG)
        const char* DebugName;
        #endif
    };

    struct RenderList
    {
        MxVector<RenderGroup> Groups;
        MxVector<size_t> UnitsIndex;
    };

    struct ParticleSystemUnit
    {
        size_t ParticleBufferOffset;
        Matrix4x4 Transform;
        float ParticleLifetime;
        float Fading;
        size_t InvocationCount;
        size_t MaterialIndex;
        bool IsRelative;
    };

    struct RenderPipeline
    {
        EnvironmentUnit Environment;
        LightingSystem Lighting;

        RenderList ShadowCasters;
        RenderList MaskedShadowCasters;
        RenderList TransparentObjects;
        RenderList MaskedObjects;
        RenderList OpaqueObjects;
        MxVector<RenderUnit> RenderUnits;

        MxVector<ParticleSystemUnit> OpaqueParticleSystems;
        MxVector<ParticleSystemUnit> TransparentParticleSystems;
        MxVector<Material> MaterialUnits;
        MxVector<CameraUnit> Cameras;
        RenderStatistics Statistics;
    };
}