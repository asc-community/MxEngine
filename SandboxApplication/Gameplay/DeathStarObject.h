#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitDeathStar(MxObject& object)
{
	auto transform = object.GetComponent<Transform>();
	auto objectPath = "objects/death_star/death_star.obj"_id;

	object.AddComponent<MeshSource>(AssetManager::LoadMesh(objectPath));
	object.AddComponent<MeshRenderer>(AssetManager::LoadMaterials(objectPath));

	transform->Scale(0.00005f);
	transform->RotateX(-90.0f);
	transform->RotateZ(-90.0f);
	transform->Translate({ -10.0f, 10.0f, 10.0f });

	struct DeathStarBehaviour
	{
		void OnUpdate(MxObject& object, float dt)
		{
			object.GetTransform().RotateY(2.0f * dt);
		}
	};
	object.AddComponent<Behaviour>(DeathStarBehaviour{ });
}