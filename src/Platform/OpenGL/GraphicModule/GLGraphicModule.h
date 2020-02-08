#pragma once

#include "Core/Interfaces/GraphicAPI/GraphicModule.h"

namespace MxEngine
{
    class GLGraphicModule final : public GraphicModule
    {
    public:
        // Inherited via GraphicModule
        virtual void Init() override;
        virtual void OnWindowCreate(WindowHandler* window) override;
        virtual void OnWindowUpdate(WindowHandler* window) override;
        virtual void OnWindowDestroy(WindowHandler* window) override;
        virtual void OnRenderDraw() override;
        virtual void Destroy() override;
    };
}