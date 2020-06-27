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

#include "RenderManager.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    CameraController::Handle MxEngine::RenderManager::GetViewport()
    {
        return Application::Get()->GetRenderAdaptor().Viewport;
    }

    void RenderManager::SetViewport(const CameraController::Handle& viewport)
    {
        Application::Get()->GetRenderAdaptor().Viewport = viewport;
    }

    void RenderManager::ResizeViewport(size_t width, size_t height)
    {
        auto viewport = RenderManager::GetViewport();
        if (viewport.IsValid())
        {
            viewport->ResizeRenderTexture(width, height);
        }
    }

    RenderController& RenderManager::GetController()
    {
        return Application::Get()->GetRenderAdaptor().Renderer;
    }

    DebugBuffer& RenderManager::GetDebugDrawer()
    {
        return Application::Get()->GetRenderAdaptor().DebugDrawer;
    }

    #define FWD(func_name, ...) Application::Get()->GetRenderAdaptor().func_name(__VA_ARGS__)

    void RenderManager::LoadMainShader(bool useLighting)
    {
        FWD(LoadMainShader, useLighting);
    }

    void RenderManager::SetRenderToDefaultFrameBuffer(bool value)
    {
        FWD(SetRenderToDefaultFrameBuffer, value);
    }

    bool RenderManager::IsRenderedToDefaultFrameBuffer()
    {
        return FWD(IsRenderedToDefaultFrameBuffer);
    }

    void RenderManager::SetFogColor(const Vector3& color)
    {
        FWD(SetFogColor, color);
    }

    const Vector3& RenderManager::GetFogColor()
    {
        return FWD(GetFogColor);
    }

    void RenderManager::SetFogDensity(float density)
    {
        FWD(SetFogDensity, density);
    }

    float RenderManager::GetFogDensity()
    {
        return FWD(GetFogDensity);
    }

    void RenderManager::SetFogDistance(float distance)
    {
        FWD(SetFogDistance, distance);
    }

    float RenderManager::GetFogDistance()
    {
        return FWD(GetFogDistance);
    }

    void RenderManager::SetShadowBlurIterations(size_t iterations)
    {
        FWD(SetShadowBlurIterations, iterations);
    }

    size_t RenderManager::GetShadowBlurIterations()
    {
        return FWD(GetShadowBlurIterations);
    }
}