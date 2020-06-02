#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitDestroyer(MxObject& object)
{
	auto transform = object.GetComponent<Transform>();
	auto meshRenderer = object.GetComponent<MeshRenderer>();

	object.AddComponent<MeshSource>(ResourceFactory::Create<Mesh>(FileManager::GetFilePath("objects/destroyer/destroyer.obj"_id), meshRenderer.GetUnchecked()));
	object.ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("objects/destroyer/Aluminm5.jpg"_id)));

	transform->Translate(MakeVector3(3.0f, 1.0f, 2.0f));
	transform->Translate(MakeVector3(-5, 0.0f, 5.0f));
	transform->Scale(0.0005f);
}