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
		this->Texture = context->CreateTexture("objects/arc170/arc170.jpg");

		this->Scale(0.005f);
		this->Translate(10.0f, 1.0f, -10.0f);
		/*this->AddInstancedBufferGenerator([this] (int idx)
			{
				auto v = MakeVector3(5.0f * idx - 10.0f, 0.0f, 5.0f * idx - 10.0f);
				return Inverse(this->GetModelMatrix()) * Vector4(v, 0.0f);
			}, 4);
		*/
	}

	inline virtual void OnUpdate() override
	{
		float dt = Context::Instance()->GetTimeDelta();
		this->RotateY(-10.0f * dt);
		this->TranslateForward(2.0f * dt);
	}
};