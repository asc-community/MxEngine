#pragma once

#include "Core/Interfaces/GraphicAPI/IBindable.h"
#include "Utilities/Math/Math.h"
#include <string>

namespace MxEngine
{
    struct Shader : IBindable
    {
        virtual void Load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) = 0;

        virtual void SetUniformFloat(const std::string& name, float f) const = 0;
        virtual void SetUniformVec3(const std::string& name, float f1, float f2, float f3) const = 0;
        virtual void SetUniformVec4(const std::string& name, float f1, float f2, float f3, float f4) const = 0;
        virtual void SetUniformVec3(const std::string& name, const Vector3& vec) const = 0;
        virtual void SetUniformVec4(const std::string& name, const Vector3& vec) const = 0;
        virtual void SetUniformMat4(const std::string& name, const Matrix4x4& matrix) const = 0;
        virtual void SetUniformMat3(const std::string& name, const Matrix3x3& matrix) const = 0;
        virtual void SetUniformInt(const std::string& name, int i) const = 0;
    };
}
