#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class DeathStarObject : public MxObject
{
public:
	inline DeathStarObject()
	{
		auto context = Context::Instance();
		this->Load(context->LoadObjectBase("objects/death_star/death_star.obj"));
		this->Texture = context->CreateTexture("objects/death_star/texture.jpg");

		this->Scale(0.00005f);
		this->RotateX(-90.0f);
		this->RotateZ(-90.0f);
		this->Translate(-10.0f, 10.0f, 10.0f);
	}

	inline virtual void OnUpdate() override
	{
		float dt = Context::Instance()->GetTimeDelta();
		this->RotateZ(2.0f * dt);
	}
};