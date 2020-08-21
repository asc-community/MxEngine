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
#include "Rectangle.h"
#include "SkyboxObject.h"
#include "RenderHelperObject.h"

namespace MxEngine
{
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
        TextureHandle HDRTexture;

        FrustrumCuller Culler;
        Matrix4x4 ViewMatrix;
        Matrix4x4 ProjectionMatrix;
        Matrix4x4 StaticViewMatrix;
        Vector3 ViewportPosition;
        TextureHandle OutputTexture;
        Matrix3x3 InversedSkyboxRotation;
        CubeMapHandle SkyboxMap;
        float BloomWeight;
        float Exposure;

        bool IsPerspective;
        bool RenderToTexture;
        uint8_t BloomIterations;
    };

    struct EnvironmentUnit
    {
        ShaderHandle GBufferShader;
        ShaderHandle GlobalIlluminationShader;
        ShaderHandle SpotLightShader;
        ShaderHandle HDRToLDRShader;
        ShaderHandle SkyboxShader;
        ShaderHandle ShadowMapShader;
        ShaderHandle ShadowCubeMapShader;
        ShaderHandle BloomShader;
        ShaderHandle ImageForwardShader;
        ShaderHandle DebugDrawShader;

        TextureHandle DefaultMaterialMap;
        TextureHandle DefaultNormalMap;
        TextureHandle DefaultBlackMap;
        CubeMapHandle DefaultBlackCubeMap;

        FrameBufferHandle DepthFrameBuffer;
        FrameBufferHandle PostProcessFrameBuffer;
        std::array<FrameBufferHandle, 2> BloomBuffers;

        SkyboxObject SkyboxCubeObject;
        DebugBufferUnit DebugBufferObject;
        Rectangle RectangularObject;
        RenderHelperObject PyramidObject;
        RenderHelperObject SphereObject;
        VectorInt2 Viewport;

        Vector3 FogColor;
        float FogDistance;
        float FogDensity;

        uint8_t ShadowBlurIterations;
        uint8_t MainCameraIndex;
        bool OverlayDebugDraws;
        bool RenderToDefaultFrameBuffer;
    };

    struct DirectionalLigthUnit
    {
        TextureHandle ShadowMap;
        Matrix4x4 ProjectionMatrix;
        Matrix4x4 BiasedProjectionMatrix;
        Vector3 Direction;
        Vector3 AmbientColor;
        Vector3 DiffuseColor;
        Vector3 SpecularColor;
    };

    struct PointLightUnit
    {
        CubeMapHandle ShadowMap;
        Matrix4x4 ProjectionMatrices[6];
        Vector3 Position;
        float FarDistance;
        Vector3 Factors;
        Vector3 AmbientColor;
        Vector3 DiffuseColor;
        Vector3 SpecularColor;
    };

    struct SpotLightUnit
    {
        TextureHandle ShadowMap;
        Matrix4x4 ProjectionMatrix;
        Matrix4x4 FrustrumTransformMatrix;
        Matrix4x4 BiasedProjectionMatrix;
        Vector3 Position;
        Vector3 AmbientColor;
        Vector3 DiffuseColor;
        Vector3 SpecularColor;
        Vector3 Direction;
        float InnerAngleCos;
        float OuterAngleCos;
    };

    struct LightingSystem
    {
        MxVector<DirectionalLigthUnit> DirectionalLights;
        MxVector<PointLightUnit> PointLights;
        MxVector<SpotLightUnit> SpotLights;
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