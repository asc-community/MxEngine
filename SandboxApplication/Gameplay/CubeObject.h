#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Cube.h>

using namespace MxEngine;

class CubeObject : public Cube
{
	int cubeCount = 100;
public:
	inline CubeObject()
	{
		auto context = Application::Get();
		this->ObjectTexture = context->GetCurrentScene().LoadTexture("CrateTexture", "objects/crate/crate.jpg");
		this->MakeInstanced(cubeCount);

		this->Translate(0.5f, 0.0f, 0.5f);
	}

	inline virtual void OnUpdate() override
	{
		static float counter = 1.0f;
		static size_t offset = 0;
		static float maxHeight = 0.5f * (cubeCount - 1);

		auto& instances = this->GetInstances();

		for(size_t idx = 0; idx < instances.size(); idx++)
		{
			int id = int(idx - offset);
			counter += 0.0005f * Application::Get()->GetTimeDelta();

			Vector3 position;
			position.x = 5.0f * std::sin(0.2f * id + counter);
			position.y = 0.5f * (id + counter);
			position.z = 5.0f * std::cos(0.2f * id + counter);

			if (position.y > maxHeight) offset++;

			instances[idx].Model.SetTranslation(position);
		}
	}
};