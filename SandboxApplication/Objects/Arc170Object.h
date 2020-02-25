#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class Arc170Object : public MxObject
{
public:
	inline Arc170Object()
	{
		auto context = Context::Instance();
		this->Load(context->LoadObjectBase("objects/arc170/arc170.obj"));

		this->Scale(0.005f);
		this->Translate(10.0f, 0.5f, -10.0f);
	}

	inline virtual void OnUpdate() override
	{
		float dt = Context::Instance()->GetTimeDelta();
		this->RotateY(-10.0f * dt);
		this->TranslateForward(2.0f * dt);
	}
};