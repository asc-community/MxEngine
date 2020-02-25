#pragma once

#include <MxEngine.h>

#include <array>

using namespace MxEngine;

class CubeObject : public MxObject
{
public:
	inline CubeObject()
	{
		auto context = Context::Instance();
		this->Load(context->LoadObjectBase("objects/crate/crate.obj"));

		int cubeCount = 100;
		this->Translate(0.5f, 0.5f, 0.5f);
		this->AddInstanceBufferGenerator([](size_t idx)
			{
				Vector3 position;
				position.x = 5.0f * std::sin(0.2f * idx);
				position.y = 0.5f * idx;
				position.z = 5.0f * std::cos(0.2f * idx);
				return position;
			}, cubeCount);
	}
};