#pragma once

void InitPBRObject(MxObject& object)
{
    object.Name = "PBR Test Object";
    object.Transform.TranslateY(2.0f);
    object.AddComponent<MeshSource>(Primitives::CreateSphere());

    auto material = object.AddComponent<MeshRenderer>()->GetMaterial();
    // material->AlbedoMap = AssetManager::LoadTexture("textures/PBR/pirate-gold_albedo.png"_id);
    // material->AmbientOcclusionMap = AssetManager::LoadTexture("textures/PBR/pirate-gold_ao.png"_id);
    // material->HeightMap = AssetManager::LoadTexture("textures/PBR/pirate-gold_height.png"_id);
    // material->MetallicMap = AssetManager::LoadTexture("textures/PBR/pirate-gold_metallic.png"_id);
    // material->RoughnessMap = AssetManager::LoadTexture("textures/PBR/pirate-gold_roughness.png"_id);
    // material->NormalMap = AssetManager::LoadTexture("textures/PBR/pirate-gold_normal-dx.png"_id);
}