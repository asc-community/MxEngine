#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class GridObject : public Object
{
public:
    inline GridObject()
    {
        auto context = Context::Instance();
        this->Load(context->LoadObjectBase("objects/grid/grid.obj"));
        this->Shader = context->CreateShader("shaders/grid_vertex.glsl", "shaders/grid_fragment.glsl");
    }
};