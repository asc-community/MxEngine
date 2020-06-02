#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitArc(MxObject& object)
{
	struct ArcBehaviour
	{
		void OnUpdate(MxObject& object, float dt)
		{
			auto script = object.GetComponent<Script>();
			Application::Get()->ExecuteScript(*script);

			object.GetTransform().RotateY(-10.0f * dt);
			object.TranslateForward(2.0f * dt);
		}
	};

	auto meshRenderer = object.GetComponent<MeshRenderer>();
	auto transform = object.GetComponent<Transform>();
	auto update = object.AddComponent<Behaviour>(ArcBehaviour{ });
	auto script = object.AddComponent<Script>(FileManager::GetFilePath("scripts/update.py"_id));
	auto meshSource = object.AddComponent<MeshSource>(
		ResourceFactory::Create<Mesh>(FileManager::GetFilePath("objects/arc170/arc170.obj"_id), meshRenderer.GetUnchecked())
		);

	object.ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("objects/arc170/arc170.jpg"_id)));
	transform->Scale(0.005f);
	transform->Translate(MakeVector3(10.0f, 1.0f, -10.0f));
}