#pragma once

void InitDeathStar(MxObject& object)
{
	auto objectPath = "objects/death_star/death_star.obj"_id;

	object.AddComponent<MeshSource>(AssetManager::LoadMesh(objectPath));
	object.AddComponent<MeshRenderer>(AssetManager::LoadMaterials(objectPath));

	object.Transform->Scale(0.00005f);
	object.Transform->RotateX(-90.0f);
	object.Transform->RotateZ(-90.0f);
	object.Transform->Translate({ -10.0f, 10.0f, 10.0f });

	struct DeathStarBehaviour
	{
		void OnUpdate(MxObject& object, float dt)
		{
			object.Transform->RotateY(2.0f * dt);
		}
	};
	object.AddComponent<Behaviour>(DeathStarBehaviour{ });
}