#pragma once

#include <MxEngine.h>
#include <iostream>

using namespace MxEngine;

class GridObject : public MxObject
{
public:
	inline GridObject()
	{
		auto context = Context::Instance();
		this->Load(context->LoadObjectBase("objects/grid/grid.obj"));
		this->Texture = context->CreateTexture("objects/grid/grid.png");
	}
};