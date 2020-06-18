#pragma once

#include "Platform/GraphicAPI.h"
#include "Utilities/ECS/Component.h"

namespace MxEngine
{
    class Skybox
    {
        MAKE_COMPONENT(Skybox);

        Vector3 rotation{ 0 };
        mutable Matrix3x3 cachedRotation{ 0 };
        mutable bool needUpdate = true;
    public:
        GResource<CubeMap> Texture;
        
        void RotateX(float angle);
        void RotateY(float angle);
        void RotateZ(float angle);
        const Vector3& GetRotation() const;
        const Matrix3x3& GetRotationMatrix() const;
    };
}