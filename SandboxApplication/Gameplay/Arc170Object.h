#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class Arc170Object : public MxObject
{
public:
	inline Arc170Object()
	{
		auto context = Application::Get();
		this->SetMesh(context->GetCurrentScene().LoadMesh("objects/arc170/arc170.obj"));
		this->ObjectTexture = context->GetCurrentScene().LoadTexture("objects/arc170/arc170.jpg");

		this->ObjectTransform.Scale(0.005f);
		this->Translate(10.0f, 1.0f, -10.0f);
	}

	inline virtual void OnUpdate() override
	{
		float dt = Application::Get()->GetTimeDelta();
		this->ObjectTransform.RotateY(-10.0f * dt);
		this->TranslateForward(2.0f * dt);
	}
};