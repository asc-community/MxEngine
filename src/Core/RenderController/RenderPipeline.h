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
        GResource<FrameBuffer> AttachedFrameBuffer;
        GResource<Texture> OutputTexture;
        Matrix3x3 InversedSkyboxRotation;
        GResource<CubeMap> SkyboxMap;
        float BloomWeight;
        float Exposure;

        bool IsPerspective;
        bool HasSkybox;
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
        GResource<Texture> BloomHDRMap;

        GResource<FrameBuffer> DepthFrameBuffer;
        GResource<FrameBuffer> HDRFrameBuffer;
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
        MxVector<RenderUnit> RenderUnits;
        MxVector<CameraUnit> Cameras;
    };
}