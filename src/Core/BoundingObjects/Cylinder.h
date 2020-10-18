// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "Utilities/Math/Math.h"
#include "Core/BoundingObjects/AABB.h"

namespace MxEngine
{
    class Cylinder
    {
    public:
        enum class Axis
        {
            X, Y, Z
        };

        float Height  = 0.0f;
        float RadiusX = 0.0f;
        float RadiusZ = 0.0f;
        Vector3 Center = MakeVector3(0.0f);
        Quaternion Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
        Axis Orientation = Axis::Y;

        constexpr Cylinder() = default;

        constexpr Cylinder(float height, float radiusX, float radiusZ, Axis orientation)
            : Height(height), RadiusX(radiusX), RadiusZ(radiusZ), Orientation(orientation) { }
    };

    constexpr Cylinder ToCylinder(const AABB& aabb, Cylinder::Axis axis)
    {
        auto length = aabb.Length();
        float height = 0.0f, radiusX = 0.0f, radiusZ = 0.0f;
        switch (axis)
        {
        case Cylinder::Axis::X:
            height = length.x, radiusX = length.y * 0.5f, radiusZ = length.z * 0.5f;
            break;
        case Cylinder::Axis::Y:
            height = length.y, radiusX = length.x * 0.5f, radiusZ = length.z * 0.5f;
            break;
        case Cylinder::Axis::Z:
            height = length.z, radiusX = length.x * 0.5f, radiusZ = length.y * 0.5f;
            break;
        }
        return Cylinder(height, radiusX, radiusZ, axis);
    }

    inline constexpr bool operator==(const Cylinder& b1, const Cylinder& b2)
    {
        return b1.Center == b2.Center && b1.Height == b2.Height && b1.Orientation == b2.Orientation &&
            b1.RadiusX == b2.RadiusX && b1.RadiusZ == b2.RadiusZ && b1.Rotation == b2.Rotation;
    }

    inline constexpr bool operator!=(const Cylinder& b1, const Cylinder& b2)
    {
        return !(b1 == b2);
    }
}