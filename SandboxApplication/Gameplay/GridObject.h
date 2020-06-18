#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitGrid(MxObject& object)
{
	auto transform = object.GetComponent<Transform>();

	transform->Scale(3.0f);

	object.Name = "Grid";
	object.AddComponent<MeshSource>(Primitives::CreatePlane(1000));

	auto gridTexture = AssetManager::LoadTexture("textures/brick.jpg"_id);
	auto meshRenderer = object.GetComponent<MeshRenderer>();
	auto material = meshRenderer->GetMaterial();
	material->AmbientMap = gridTexture;
	material->DiffuseMap = gridTexture;
	material->NormalMap = AssetManager::LoadTexture("textures/brick_normal.jpg"_id);
}