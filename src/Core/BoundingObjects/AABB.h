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

namespace MxEngine
{
    class AABB
    {
    public:
        Vector3 Min = MakeVector3(0.0f);
        Vector3 Max = MakeVector3(0.0f);

        constexpr Vector3 Length() const
        {
            return Max - Min;
        }

        constexpr Vector3 GetCenter() const
        {
            return (Max + Min) * 0.5f;
        }
    };

    inline constexpr AABB operator*(const AABB& box, const Vector3& scale)
    {
        return AABB{ box.Min * scale, box.Max * scale };
    }

    inline constexpr AABB operator/(const AABB& box, const Vector3& scale)
    {
        return AABB{ box.Min / scale, box.Max / scale };
    }

    inline constexpr AABB operator+(const AABB& box, const Vector3& translate)
    {
        return AABB{ box.Min + translate, box.Max + translate };
    }

    inline constexpr AABB operator-(const AABB& box, const Vector3& translate)
    {
        return AABB{ box.Min - translate, box.Max - translate };
    }

    inline AABB operator*(const AABB& box, const Matrix4x4& matrix)
    {
        std::array<Vector3, 8> vecs;
        vecs =
        {
            matrix * MakeVector4(box.Min.x, box.Min.y, box.Min.z, 1.0f),
            matrix * MakeVector4(box.Min.x, box.Min.y, box.Max.z, 1.0f),
            matrix * MakeVector4(box.Min.x, box.Max.y, box.Min.z, 1.0f),
            matrix * MakeVector4(box.Min.x, box.Max.y, box.Max.z, 1.0f),
            matrix * MakeVector4(box.Max.x, box.Min.y, box.Min.z, 1.0f),
            matrix * MakeVector4(box.Max.x, box.Min.y, box.Max.z, 1.0f),
            matrix * MakeVector4(box.Max.x, box.Max.y, box.Min.z, 1.0f),
            matrix * MakeVector4(box.Max.x, box.Max.y, box.Max.z, 1.0f),
        };
        auto minmax = MinMaxComponents(vecs.data(), vecs.size());
        return AABB{ minmax.first, minmax.second };
    }
}