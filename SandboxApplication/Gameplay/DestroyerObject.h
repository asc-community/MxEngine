#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitDestroyer(MxObject& object)
{
	auto transform = object.GetComponent<Transform>();
	auto objectPath = "objects/destroyer/destroyer.obj"_id;

	object.AddComponent<MeshSource>(AssetManager::LoadMesh(objectPath));
	object.AddComponent<MeshRenderer>(AssetManager::LoadMaterials(objectPath));

	transform->Translate(MakeVector3(3.0f, 1.0f, 2.0f));
	transform->Translate(MakeVector3(-5, 0.0f, 5.0f));
	transform->Scale(0.0005f);
}