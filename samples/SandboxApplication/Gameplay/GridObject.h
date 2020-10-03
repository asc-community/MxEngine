#pragma once

void InitGrid(MxObject& object)
{
	object.Transform.Scale(3.0f);

	object.Name = "Grid";
	object.AddComponent<MeshSource>(Primitives::CreatePlane(1000));

	auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
	// material->AlbedoMap = AssetManager::LoadTexture("textures/brick.jpg"_id);
	// material->NormalMap = AssetManager::LoadTexture("textures/brick_normal.jpg"_id);
	material->Reflection = 1.0f;

	Rendering::SetFogDensity(0.0f);
	MxObject::GetByComponent(*Rendering::GetViewport()).RemoveComponent<CameraEffects>();

	auto sponza = MxObject::Create();
	sponza->AddComponent<MeshSource>(AssetManager::LoadMesh(R"(D:\repos\glTF-Sample-Models\2.0\Sponza\glTF\Sponza.gltf)"));
	sponza->AddComponent<MeshRenderer>(MeshRenderer::LoadMaterials(R"(D:\repos\glTF-Sample-Models\2.0\Sponza\glTF\Sponza.gltf)"));
	sponza->Transform.SetScale(0.02f);
	sponza->Transform.TranslateY(13.0f);
}