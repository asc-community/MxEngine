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
#include "AABB.h"

namespace MxEngine
{
    class BoundingBox
    {
    public:
        Vector3 Center{ 0.0f, 0.0f, 0.0f };
        Vector3 Min = MakeVector3(0.0f);
        Vector3 Max = MakeVector3(0.0f);
        Quaternion Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };

        constexpr BoundingBox() = default;

        constexpr BoundingBox(const Vector3& center, const Vector3& halfSize)
            : Center(center), Min(-halfSize), Max(halfSize) { }

        constexpr Vector3 Length() const
        {
            return Max - Min;
        }
    };

    inline constexpr BoundingBox ToBoundingBox(const AABB& aabb)
    {
        BoundingBox result;
        result.Max = aabb.Max;
        result.Min = aabb.Min;
        return result;
    }

    inline constexpr bool operator==(const BoundingBox& b1, const BoundingBox& b2)
    {
        return b1.Center == b2.Center && b1.Max == b2.Max && b1.Min == b2.Min && b1.Rotation == b2.Rotation;
    }

    inline constexpr bool operator!=(const BoundingBox& b1, const BoundingBox& b2)
    {
        return !(b1 == b2);
    }
}