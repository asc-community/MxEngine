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
        this->ObjectTexture = scene.LoadTexture("textures/planet_texture.png");
        material.map_height = GraphicFactory::Create<Texture>(ToMxString(FileModule::GetFilePath("textures/planet_height.png"_id)));
        material.map_normal = GraphicFactory::Create<Texture>(ToMxString(FileModule::GetFilePath("textures/planet_normal.png"_id)));
    }
};