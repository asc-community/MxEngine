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

#include "Platform/GraphicAPI.h"
#include "Rectangle.h"
#include "SkyboxObject.h"

namespace MxEngine
{
    struct DebugBufferUnit
    {
        GResource<VertexArray> VAO;
        size_t VertexCount;
    };

    struct CameraUnit
    {
        Matrix4x4 ViewProjMatrix;
        Matrix4x4 StaticViewProjMatrix;
        Vector3 ViewportPosition;
        GResource<FrameBuffer> FrameBufferMSAA;
        GResource<FrameBuffer> FrameBufferHDR;
        GResource<Texture> BloomTextureHDR;
        GResource<Texture> OutputTexture;
        Matrix3x3 InversedSkyboxRotation;
        GResource<CubeMap> SkyboxMap;
        float BloomWeight;
        float Exposure;

        bool IsPerspective;
        bool RenderToTexture;
        uint8_t BloomIterations;
    };

    struct EnvironmentUnit
    {
        GResource<Shader> MainShader;
        GResource<Shader> SkyboxShader;
        GResource<Shader> ShadowMapShader;
        GResource<Shader> ShadowCubeMapShader;
        GResource<Shader> MSAAHDRSplitShader;
        GResource<Shader> BloomShader;
        GResource<Shader> HDRBloomCombineHDRShader;
        GResource<Shader> ImageForwardShader;
        GResource<Shader> DebugDrawShader;

        GResource<Texture> DefaultMaterialMap;
        GResource<Texture> DefaultNormalMap;
        GResource<Texture> DefaultHeightMap;
        GResource<Texture> DefaultBlackMap;
        GResource<CubeMap> DefaultBlackCubeMap;

        GResource<FrameBuffer> DepthFrameBuffer;
        GResource<FrameBuffer> PostProcessFrameBuffer;
        std::array<GResource<FrameBuffer>, 2> BloomBuffers;

        SkyboxObject SkyboxCubeObject;
        DebugBufferUnit DebugBufferObject;
        Rectangle RectangularObject;
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
        GResource<Texture> ShadowMap;
        Matrix4x4 ProjectionMatrix;
        Matrix4x4 BiasedProjectionMatrix;
        Vector3 Direction;
        Vector3 AmbientColor;
        Vector3 DiffuseColor;
        Vector3 SpecularColor;
    };

    struct PointLightUnit
    {
        GResource<CubeMap> ShadowMap;
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
        GResource<Texture> ShadowMap;
        Matrix4x4 ProjectionMatrix;
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
        GResource<VertexArray> VAO;
        GResource<IndexBuffer> IBO;

        Material RenderMaterial;
        
        Matrix4x4 ModelMatrix;
        Matrix3x3 NormalMatrix;

        size_t InstanceCount;
    };

    struct RenderPipeline
    {
        EnvironmentUnit Environment;
        LightingSystem Lighting;
        MxVector<RenderUnit> OpaqueRenderUnits;
        MxVector<RenderUnit> TransparentRenderUnits;
        MxVector<CameraUnit> Cameras;
    };
}