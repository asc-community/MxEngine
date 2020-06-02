#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Sphere.h>
#include <Library/Primitives/Colors.h>

using namespace MxEngine;

void InitSphere(MxObject& sphere)
{
    sphere.GetTransform().Translate(MakeVector3(-13.0f, 1.0f, 2.0f));
    auto meshRenderer = sphere.GetComponent<MeshRenderer>();
    
    auto material = meshRenderer->GetMaterial();
    material->map_height = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("textures/planet_height.png"_id)));
    material->map_normal = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("textures/planet_normal.png"_id)));
}

class SphereObject : public Sphere
{
public:
    inline SphereObject()
    {
        auto& scene = Application::Get()->GetCurrentScene();
        this->ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("textures/planet_texture.png"_id)));
    }
};