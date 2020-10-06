#pragma once

void InitGrid(MxObject& object)
{
	object.Transform.Scale(3.0f);

	object.Name = "Grid";
	object.AddComponent<MeshSource>(Primitives::CreatePlane(1000));

	auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
	material->AlbedoMap = AssetManager::LoadTexture("textures/brick.jpg"_id);
	material->NormalMap = AssetManager::LoadTexture("textures/brick_normal.jpg"_id);
	material->Reflection = 0.75f;

	Rendering::SetFogDensity(0.0f);

	auto sponza = MxObject::Create();
	sponza->AddComponent<MeshSource>(AssetManager::LoadMesh("Sponza/glTF/Sponza.gltf"_id));
	sponza->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Sponza/glTF/Sponza.gltf"_id));
	sponza->Transform.SetScale(0.02f);
	sponza->Transform.TranslateY(13.0f);
}