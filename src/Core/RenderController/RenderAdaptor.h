#pragma once

#include "Core/RenderController/RenderController.h"

namespace MxEngine
{
    struct RenderAdaptor
    {
        RenderController Renderer;
        DebugBuffer DebugDraw;
        Resource<CameraController, ComponentFactory> Viewport;

        constexpr static TextureFormat HDRTextureFormat = TextureFormat::RGBA16F;

        void InitRendererEnvironment();
        void OnWindowResize(const VectorInt2& newViewport);
        void LoadMainShader(bool useLighting = true);
        void PerformRenderIteration();

        void SetFogColor(const Vector3& color);
        const Vector3& GetFogColor() const;
        void SetFogDensity(float density);
        float GetFogDensity() const;
        void SetFogDistance(float distance);
        float GetFogDistance() const;
        void SetShadowBlurIterations(size_t iterations);
        size_t getShadowBlurIterations() const;
    };
}