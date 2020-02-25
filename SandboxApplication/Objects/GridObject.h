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
		this->Shader = context->CreateShader("shaders/grid_vertex.glsl", "shaders/grid_fragment.glsl");
	}
};