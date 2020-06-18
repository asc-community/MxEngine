#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitSphere(MxObject& sphere)
{
    auto transform = sphere.GetComponent<Transform>();
    auto meshRenderer = sphere.GetComponent<MeshRenderer>();

    transform->Translate(MakeVector3(-13.0f, 1.0f, 2.0f));

    sphere.Name = "Sphere";
    auto sphereTexture = AssetManager::LoadTexture("textures/planet_texture.png"_id);
    sphere.AddComponent<MeshSource>(Primitives::CreateSphere());
    
    auto material = meshRenderer->GetMaterial();
    material->AmbientMap = sphereTexture;
    material->DiffuseMap = sphereTexture;
    material->HeightMap = AssetManager::LoadTexture("textures/planet_height.png"_id);
    material->NormalMap = AssetManager::LoadTexture("textures/planet_normal.png"_id);
}