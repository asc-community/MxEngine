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

	object.Name = "Arc170";

	auto transform = object.GetComponent<Transform>();
	auto update = object.AddComponent<Behaviour>(ArcBehaviour{ });
	auto script = object.AddComponent<Script>("scripts/update.py"_id);

	object.AddComponent<MeshSource>(AssetManager::LoadMesh("objects/arc170/arc170.obj"_id));
	object.AddComponent<MeshRenderer>(AssetManager::LoadMaterials("objects/arc170/arc170.obj"_id));
	
	transform->Scale(0.005f);
	transform->Translate(MakeVector3(5.0f, 1.0f, -5.0f));
}