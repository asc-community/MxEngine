#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Sphere.h>
#include <Library/Primitives/Colors.h>

using namespace MxEngine;

class SphereObject : public Sphere
{
public:
    inline SphereObject()
    {
        this->Translate(-13.0f, 1.0f, 2.0f);
        auto& scene = Application::Get()->GetCurrentScene();
        auto& material = this->GetMesh()->GetRenderObjects()[0].GetMaterial();
        this->ObjectTexture = scene.LoadTexture("PlanetTexture", "textures/planet_texture.png");
        material.map_height = Graphics::Instance()->CreateTexture("Resources/textures/planet_height.png");
        material.map_normal = Graphics::Instance()->CreateTexture("Resources/textures/planet_normal.png");
    }
};