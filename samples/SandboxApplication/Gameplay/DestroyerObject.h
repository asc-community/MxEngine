#pragma once

void InitDestroyer(MxObject& object)
{
	auto objectPath = "objects/destroyer/destroyer.obj"_id;

	object.AddComponent<MeshSource>(AssetManager::LoadMesh(objectPath));
	object.AddComponent<MeshRenderer>(AssetManager::LoadMaterials(objectPath));

	object.Transform->Translate(MakeVector3(3.0f, 1.0f, 2.0f));
	object.Transform->Translate(MakeVector3(-5, 0.0f, 5.0f));
	object.Transform->Scale(0.0005f);
}