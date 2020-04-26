#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class DeathStarObject : public MxObject
{
public:
	inline DeathStarObject()
	{
		auto context = Application::Get();
		this->SetMesh(context->GetCurrentScene().LoadMesh("DeathStarMesh", "objects/death_star/death_star.obj"));
		this->ObjectTexture = context->GetCurrentScene().LoadTexture("DeathStartTexture", "objects/death_star/texture.jpg");

		this->ObjectTransform.Scale(0.00005f);
		this->ObjectTransform.RotateX(-90.0f);
		this->ObjectTransform.RotateZ(-90.0f);
		this->ObjectTransform.Translate({ -10.0f, 10.0f, 10.0f });
	}

	inline virtual void OnUpdate() override
	{
		float dt = Application::Get()->GetTimeDelta();
		this->ObjectTransform.RotateZ(2.0f * dt);
	}
};