#pragma once

#include "Utilities/Memory/Memory.h"
#include "Core/Interfaces/IDrawable.h"
#include "Core/Camera/CameraController.h"
#include "Core/Interfaces/GraphicAPI/Renderer.h"

namespace MomoEngine
{
    class RenderController
    {
        Renderer& renderer;
    public:
        RenderController(Renderer& renderer);

        CameraController ViewPort;
        Ref<Shader> ObjectShader, MeshShader;
        Ref<Texture> DefaultTexture;

        Renderer& GetRenderEngine() const;
        void Render() const;
        void Clear() const;
        void DrawObject(const IDrawable& object) const;
        void DrawObjectMesh(const IDrawable& object) const;
    };
}