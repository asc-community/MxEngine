#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Sphere.h>
#include <Library/Primitives/Colors.h>

using namespace MxEngine;

class SphereObject : public Sphere
{
public:
    inline SphereObject()
    {
        this->ObjectTexture = Colors::MakeTexture(Colors::LIME);
        this->Translate(-13.0f, 1.0f, 2.0f);
    }
};