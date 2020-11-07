#pragma once

void InitGrid(MxObject& object)
{
	object.Transform.Scale(3.0f);

	object.Name = "Grid";
	object.AddComponent<MeshSource>(Primitives::CreatePlane(1000));

	auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
	material->AlbedoMap = AssetManager::LoadTexture("textures/brick.jpg"_id);
	material->NormalMap = AssetManager::LoadTexture("textures/brick_normal.jpg"_id);
	material->AmbientOcclusionMap = AssetManager::LoadTexture("textures/brick_ao.jpg"_id);
}