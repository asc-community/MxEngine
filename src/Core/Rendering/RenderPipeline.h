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
#include "RenderObjects/Rectangle.h"
#include "RenderObjects/SkyboxObject.h"
#include "RenderObjects/RenderHelperObject.h"
#include "RenderObjects/PointLightInstancedObject.h"
#include "RenderObjects/SpotLightInstancedObject.h"
#include "Core/Resources/ACESCurve.h"
#include "Core/Resources/Material.h"

#include "Utilities/STL/MxHashMap.h"
#include "Utilities/String/String.h"

namespace MxEngine
{
    class Skybox;
    class CameraEffects;
    class CameraToneMapping;
    class CameraSSR;
    
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
        TextureHandle SwapTexture;

        FrustrumCuller Culler;
        Matrix4x4 InverseViewProjMatrix;
        Matrix4x4 ViewProjectionMatrix;
        Matrix4x4 StaticViewProjectionMatrix;

        Vector3 ViewportPosition;
        TextureHandle OutputTexture;

        Matrix3x3 InverseSkyboxRotation;
        CubeMapHandle SkyboxTexture;

        float Gamma;

        bool IsPerspective;
        bool RenderToTexture;

        const CameraEffects* Effects;
        const CameraToneMapping* ToneMapping;
        const CameraSSR* SSR;
    };

    struct EnvironmentUnit
    {
        MxHashMap<StringId, ShaderHandle> Shaders;

        TextureHandle DefaultMaterialMap;
        TextureHandle DefaultNormalMap;
        TextureHandle DefaultBlackMap;
        TextureHandle DefaultGreyMap;
        TextureHandle AverageWhiteTexture;
        CubeMapHandle DefaultBlackCubeMap;

        FrameBufferHandle DepthFrameBuffer;
        FrameBufferHandle PostProcessFrameBuffer;
        std::array<FrameBufferHandle, 2> BloomBuffers;

        SkyboxObject SkyboxCubeObject;
        DebugBufferUnit DebugBufferObject;
        Rectangle RectangularObject;
        VectorInt2 Viewport;

        Vector3 FogColor;
        float FogDistance;
        float FogDensity;
        float TimeDelta;

        uint8_t ShadowBlurIterations;
        uint8_t MainCameraIndex;
        bool OverlayDebugDraws;
        bool RenderToDefaultFrameBuffer;
    };

    struct DirectionalLightUnit
    {
        TextureHandle ShadowMap;
        Matrix4x4 ProjectionMatrix;
        Matrix4x4 BiasedProjectionMatrix;
        Vector3 Direction;
        Vector3 AmbientColor;
        Vector3 DiffuseColor;
        Vector3 SpecularColor;
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
        PointLightInstancedObject PointLigthsInstanced;
        RenderHelperObject SphereLight;
        RenderHelperObject PyramidLight;
    };

    struct RenderUnit
    {
        VertexArrayHandle VAO;
        IndexBufferHandle IBO;

        size_t materialIndex;
        
        Matrix4x4 ModelMatrix;
        Matrix3x3 NormalMatrix;

        Vector3 MinAABB, MaxAABB;
        size_t InstanceCount;
    };

    struct RenderPipeline
    {
        EnvironmentUnit Environment;
        LightingSystem Lighting;
        MxVector<RenderUnit> ShadowCasterUnits;
        MxVector<RenderUnit> OpaqueRenderUnits;
        MxVector<RenderUnit> TransparentRenderUnits;
        MxVector<Material> MaterialUnits;
        MxVector<CameraUnit> Cameras;
    };
}