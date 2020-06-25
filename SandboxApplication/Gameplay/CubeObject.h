#pragma once

struct CubeBehaviour
{
	std::vector<InstanceFactory::MxInstance> instances;

	void OnUpdate(MxObject& object, float dt)
	{
		static float counter = 1.0f;
		static size_t offset = 0;
		size_t idx = 0;

		auto instances = object.GetComponent<InstanceFactory>();
		if (!instances.IsValid()) return;
		float maxHeight = 0.5f * (instances->GetCount() - 1);

		auto view = instances->GetInstances();
		for (auto& instance : view)
		{
			int id = int(idx - offset);
			idx++;
			counter += 0.0005f * dt;

			Vector3 position;
			position.x = 5.0f * std::sin(0.2f * id + counter);
			position.y = 0.5f * (id + counter);
			position.z = 5.0f * std::cos(0.2f * id + counter);

			if (position.y > maxHeight) offset++;

			instance.Transform.SetPosition(position);
		}
	}
};

void InitCube(MxObject& cube)
{
	cube.Transform->Translate(MakeVector3(0.5f, 0.0f, 0.5f));
	cube.Name = "Crate";

	auto meshSource = cube.AddComponent<MeshSource>(Primitives::CreateCube());
	auto meshRenderer = cube.AddComponent<MeshRenderer>();
	auto instances = cube.AddComponent<InstanceFactory>();
	auto behaviour = cube.AddComponent<Behaviour>(CubeBehaviour{ });

	for (size_t i = 0; i < 100; i++)
	{
		instances->MakeInstance().MakeStatic();
	}

	auto cubeTexture = AssetManager::LoadTexture("objects/crate/crate.jpg"_id);
	meshRenderer->GetMaterial()->AmbientMap = cubeTexture;
	meshRenderer->GetMaterial()->DiffuseMap = cubeTexture;
}