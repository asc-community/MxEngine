#pragma once

void InitSphere(MxObject& sphere)
{
    sphere.Transform->Translate(MakeVector3(-13.0f, 1.0f, 2.0f));

    sphere.Name = "Sphere";
    auto sphereTexture = AssetManager::LoadTexture("textures/planet_texture.png"_id);
    sphere.AddComponent<MeshSource>(Primitives::CreateSphere());
    
    auto material = sphere.GetOrAddComponent<MeshRenderer>()->GetMaterial();
    material->AmbientMap = sphereTexture;
    material->DiffuseMap = sphereTexture;
    material->HeightMap = AssetManager::LoadTexture("textures/planet_height.png"_id);
    material->NormalMap = AssetManager::LoadTexture("textures/planet_normal.png"_id);
}