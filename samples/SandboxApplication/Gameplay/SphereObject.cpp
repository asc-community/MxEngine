#include <MxEngine.h>
using namespace MxEngine;

void InitSphere(MxObject& sphere)
{
    sphere.Transform.Translate(MakeVector3(-13.0f, 1.0f, 2.0f));
    sphere.Transform.Scale(2.0f);

    sphere.Name = "Sphere";
    sphere.AddComponent<MeshSource>(Primitives::CreateSphere());
    
    auto material = sphere.GetOrAddComponent<MeshRenderer>()->GetMaterial();
    material->AlbedoMap = AssetManager::LoadTexture("Resources/textures/planet_texture.png"_id);
    material->HeightMap = AssetManager::LoadTexture("Resources/textures/planet_height.png"_id);
    material->NormalMap = AssetManager::LoadTexture("Resources/textures/planet_normal.png"_id);
}