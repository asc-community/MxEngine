#pragma once

void InitSphere(MxObject& sphere)
{
    sphere.Transform.Translate(MakeVector3(-13.0f, 1.0f, 2.0f));
    sphere.Transform.Scale(2.0f);

    sphere.Name = "Sphere";
    sphere.AddComponent<MeshSource>(Primitives::CreateSphere());
    
    auto material = sphere.GetOrAddComponent<MeshRenderer>()->GetMaterial();
    material->SpecularFactor = 0.01f;
    material->AlbedoMap = AssetManager::LoadTexture("textures/planet_texture.png"_id);
    material->HeightMap = AssetManager::LoadTexture("textures/planet_height.png"_id);
    material->NormalMap = AssetManager::LoadTexture("textures/planet_normal.png"_id);
}