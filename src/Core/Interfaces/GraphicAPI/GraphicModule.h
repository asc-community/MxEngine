#pragma once

namespace MxEngine
{
    struct WindowHandler;

    struct GraphicModule
    {
        virtual void Init() = 0;
        virtual void OnWindowCreate(WindowHandler* window) = 0;
        virtual void OnWindowUpdate(WindowHandler* window) = 0;
        virtual void OnWindowDestroy(WindowHandler* window) = 0;
        virtual void OnRenderDraw() = 0;
        virtual void Destroy() = 0;
    };
}