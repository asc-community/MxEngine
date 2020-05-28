#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Cube.h>
#include <Core/Components/Update.h>

using namespace MxEngine;

void InitCube(MxObject& cube)
{
	cube.AddComponent<Update>([](auto& self, float dt)
	{
		static float counter = 1.0f;
		static size_t offset = 0;

		auto& object = MxObject::GetByComponent(self);
		auto& instances = object.GetInstances();

		float maxHeight = 0.5f * (object.GetInstances().size() - 1);

		for (size_t idx = 0; idx < instances.size(); idx++)
		{
			int id = int(idx - offset);
			counter += 0.0005f * dt;

			Vector3 position;
			position.x = 5.0f * std::sin(0.2f * id + counter);
			position.y = 0.5f * (id + counter);
			position.z = 5.0f * std::cos(0.2f * id + counter);

			if (position.y > maxHeight) offset++;

			instances[idx].Model.SetTranslation(position);
		}
	});
}

class CubeObject : public Cube
{
	int cubeCount = 100;
public:
	inline CubeObject()
	{
		auto context = Application::Get();
		this->ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("objects/crate/crate.jpg"_id)));
		this->MakeInstanced(cubeCount);

		this->Translate(0.5f, 0.0f, 0.5f);
	}
};