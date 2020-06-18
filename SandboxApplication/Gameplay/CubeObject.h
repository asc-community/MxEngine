#pragma once

#include <MxEngine.h>
#include <Core/Components/Behaviour.h>

using namespace MxEngine;

struct CubeBehaviour
{
	std::vector<InstanceFactory::MxInstance> instances;

	void OnUpdate(MxObject& object, float dt)
	{
		static float counter = 1.0f;
		static size_t offset = 0;
		size_t idx = 0;

		float maxHeight = 0.5f * (object.GetInstanceCount() - 1);

		auto instances = object.GetInstances();
		for (auto it = instances.begin(); it != instances.end(); it++, idx++)
		{
			int id = int(idx - offset);
			counter += 0.0005f * dt;

			Vector3 position;
			position.x = 5.0f * std::sin(0.2f * id + counter);
			position.y = 0.5f * (id + counter);
			position.z = 5.0f * std::cos(0.2f * id + counter);

			if (position.y > maxHeight) offset++;

			it->Model.SetTranslation(position);
		}
	}
};

void InitCube(MxObject& cube)
{
	auto transform = cube.GetComponent<Transform>();
	auto meshRenderer = cube.GetComponent<MeshRenderer>();

	transform->Translate(MakeVector3(0.5f, 0.0f, 0.5f));

	cube.Name = "Crate";
	cube.AddComponent<MeshSource>(Primitives::CreateCube());

	size_t cubeCount = 100;
	CubeBehaviour behaviour;
	for (size_t i = 0; i < cubeCount; i++)
	{
		behaviour.instances.push_back(cube.Instanciate());
	}

	cube.AddComponent<Behaviour>(std::move(behaviour));

	auto cubeTexture = AssetManager::LoadTexture("objects/crate/crate.jpg"_id);
	meshRenderer->GetMaterial()->AmbientMap = cubeTexture;
	meshRenderer->GetMaterial()->DiffuseMap = cubeTexture;
}