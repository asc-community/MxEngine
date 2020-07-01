#pragma once

void InitGrid(MxObject& object)
{
	object.Transform->Scale(3.0f);

	object.Name = "Grid";
	object.AddComponent<MeshSource>(Primitives::CreatePlane(1000));

	auto gridTexture = AssetManager::LoadTexture("textures/brick.jpg"_id);
	auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
	material->AmbientMap = gridTexture;
	material->DiffuseMap = gridTexture;
	material->NormalMap = AssetManager::LoadTexture("textures/brick_normal.jpg"_id);
}