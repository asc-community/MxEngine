#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class DestroyerObject : public MxObject
{
public:
	inline DestroyerObject()
	{
		auto context = Context::Instance();
		this->Load(context->LoadObjectBase("objects/destroyer/destroyer.obj"));
		this->Texture = context->CreateTexture("objects/destroyer/Aluminm5.jpg");

		this->Translate(3.0f, 1.0f, 2.0f);
		this->Translate(-5, 0.0f, 5.0f);
		this->Scale(0.0005f);

		this->AddInstancedBufferGenerator([](size_t idx)
			{
				Vector3 pos(0.0f);
				pos.x = 20000.0f * idx;
				return pos;
			}, 100);
	}
};