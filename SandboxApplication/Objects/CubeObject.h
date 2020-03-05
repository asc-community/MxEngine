#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class CubeObject : public MxObject
{
	int cubeCount = 100;
public:
	inline CubeObject()
	{
		auto context = Context::Instance();
		this->Load(context->LoadObjectBase("objects/crate/crate.obj"));
		this->Texture = context->CreateTexture("objects/crate/crate.jpg");

		this->Translate(0.5f, 0.0f, 0.5f);
		this->AddInstancedBufferGenerator([](int idx) { return MakeVector3(0.0f); }, cubeCount, UsageType::DYNAMIC_DRAW);
	}

	inline virtual void OnUpdate() override
	{
		int posIndex = 0;
		this->GenerateDataByIndex(posIndex, [count = cubeCount](int idx)
			{
				static float counter = 1.0f;
				static float diff = 0.0f;
				static float maxHeight = 0.5f * (count - 1);
				float id = idx - diff;
				counter += 0.0005f * Context::Instance()->GetTimeDelta();
				Vector3 position;
				position.x = 5.0f * std::sin(0.2f * id + counter);
				position.y = 0.5f * (id + counter);
				position.z = 5.0f * std::cos(0.2f * id + counter);
				if (position.y > maxHeight) diff += 1.0f;
				return position;
			}, cubeCount);
	}
};