#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitGrid(MxObject& object)
{
	object.Scale(3.0f, 3.0f, 3.0f);
	object.ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("textures/brick.jpg"_id)));
	auto meshRenderer = object.GetComponent<MeshRenderer>();
	auto material = meshRenderer->GetMaterial();
	material->map_normal = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("textures/brick_normal.jpg"_id)));
}