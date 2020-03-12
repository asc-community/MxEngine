#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class CubeObject : public Instanced<MxObject>
{
	int cubeCount = 100;
public:
	inline CubeObject()
	{
		auto context = Application::Get();
		this->SetMesh(context->LoadMesh("objects/crate/crate.obj"));
		this->ObjectTexture = context->CreateTexture("objects/crate/crate.jpg");
		this->MakeInstanced(cubeCount, UsageType::DYNAMIC_DRAW);

		this->Translate(0.5f, 0.0f, 0.5f);
	}

	inline virtual void OnUpdate() override
	{
		this->GenerateInstanceModels([this](int idx)
			{
				static float counter = 1.0f;
				static int offset = 0;
				static float maxHeight = 0.5f * (cubeCount - 1);

				int id = idx - offset;
				counter += 0.0005f * Application::Get()->GetTimeDelta();

				Vector3 position;
				position.x = 5.0f * std::sin(0.2f * id + counter);
				position.y = 0.5f * (id + counter);
				position.z = 5.0f * std::cos(0.2f * id + counter);

				if (position.y > maxHeight) offset++;

				return MxEngine::Translate(Matrix4x4(1.0f), position);
			});
		this->BufferInstances();
	}
};