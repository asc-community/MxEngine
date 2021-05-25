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
#include <array>

namespace MxEngine
{
    // thanks to https://gist.github.com/podgorskiy/e698d18879588ada9014768e3e82a644
    class FrustrumCuller
    {
    public:
        FrustrumCuller() = default;

        // m = ProjectionMatrix * ViewMatrix 
        FrustrumCuller(const Matrix4x4& m);

        // http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
        bool IsAABBVisible(const Vector3& minp, const Vector3& maxp) const;

    private:
        enum Planes
        {
            LEFT = 0,
            RIGHT,
            BOTTOM,
            TOP,
            NEAR,
            FAR,
            COUNT,
        };

        template<Planes i, Planes j>
        struct ij2k
        {
            constexpr static int k = i * (9 - i) / 2 + j - 1;
        };

        template<Planes a, Planes b, Planes c, size_t N>
        Vector3 Intersection(const std::array<Vector3, N>& crosses) const
        {
            float D = Dot(Vector3(this->planes[a]), crosses[ij2k<b, c>::k]);
            Vector3 res = Matrix3x3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
                Vector3(this->planes[a].w, this->planes[b].w, this->planes[c].w);
            return res * (-1.0f / D);
        }

        std::array<Vector4, Planes::COUNT> planes;
        std::array<Vector3, 8> points;
    };

    inline FrustrumCuller::FrustrumCuller(const Matrix4x4& mat)
    {
        auto m = Transpose(mat);
        this->planes[LEFT]   = m[3] + m[0];
        this->planes[RIGHT]  = m[3] - m[0];
        this->planes[BOTTOM] = m[3] + m[1];
        this->planes[TOP]    = m[3] - m[1];
        this->planes[NEAR]   = m[3] + m[2];
        this->planes[FAR]    = m[3] - m[2];

        std::array crosses = {
            Cross(Vector3(this->planes[LEFT]),   Vector3(this->planes[RIGHT])),
            Cross(Vector3(this->planes[LEFT]),   Vector3(this->planes[BOTTOM])),
            Cross(Vector3(this->planes[LEFT]),   Vector3(this->planes[TOP])),
            Cross(Vector3(this->planes[LEFT]),   Vector3(this->planes[NEAR])),
            Cross(Vector3(this->planes[LEFT]),   Vector3(this->planes[FAR])),
            Cross(Vector3(this->planes[RIGHT]),  Vector3(this->planes[BOTTOM])),
            Cross(Vector3(this->planes[RIGHT]),  Vector3(this->planes[TOP])),
            Cross(Vector3(this->planes[RIGHT]),  Vector3(this->planes[NEAR])),
            Cross(Vector3(this->planes[RIGHT]),  Vector3(this->planes[FAR])),
            Cross(Vector3(this->planes[BOTTOM]), Vector3(this->planes[TOP])),
            Cross(Vector3(this->planes[BOTTOM]), Vector3(this->planes[NEAR])),
            Cross(Vector3(this->planes[BOTTOM]), Vector3(this->planes[FAR])),
            Cross(Vector3(this->planes[TOP]),    Vector3(this->planes[NEAR])),
            Cross(Vector3(this->planes[TOP]),    Vector3(this->planes[FAR])),
            Cross(Vector3(this->planes[NEAR]),   Vector3(this->planes[FAR]))
        };

        this->points[0] = this->Intersection<LEFT,  BOTTOM, NEAR>(crosses);
        this->points[1] = this->Intersection<LEFT,  TOP,    NEAR>(crosses);
        this->points[2] = this->Intersection<RIGHT, BOTTOM, NEAR>(crosses);
        this->points[3] = this->Intersection<RIGHT, TOP,    NEAR>(crosses);
        this->points[4] = this->Intersection<LEFT,  BOTTOM, FAR> (crosses);
        this->points[5] = this->Intersection<LEFT,  TOP,    FAR> (crosses);
        this->points[6] = this->Intersection<RIGHT, BOTTOM, FAR> (crosses);
        this->points[7] = this->Intersection<RIGHT, TOP,    FAR> (crosses);

    }

    // http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
    inline bool FrustrumCuller::IsAABBVisible(const Vector3& minp, const Vector3& maxp) const
    {
        // check box outside/inside of frustum
        for (const auto& plane : this->planes)
        {
            if ((Dot(plane, Vector4(minp.x, minp.y, minp.z, 1.0f)) < 0.0f) &&
                (Dot(plane, Vector4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0f) &&
                (Dot(plane, Vector4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0f) &&
                (Dot(plane, Vector4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0f) &&
                (Dot(plane, Vector4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0f) &&
                (Dot(plane, Vector4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0f) &&
                (Dot(plane, Vector4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0f) &&
                (Dot(plane, Vector4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0f))
            {
                return false;
            }
        }

        // this checks somehow break everything. Commented them for now

        // // check frustum outside/inside box
        // int out;
        // out = 0; for (const auto& point : this->points) out += int(point.x > maxp.x); if (out == 8) return false;
        // out = 0; for (const auto& point : this->points) out += int(point.x < minp.x); if (out == 8) return false;
        // out = 0; for (const auto& point : this->points) out += int(point.y > maxp.y); if (out == 8) return false;
        // out = 0; for (const auto& point : this->points) out += int(point.y < minp.y); if (out == 8) return false;
        // out = 0; for (const auto& point : this->points) out += int(point.z > maxp.z); if (out == 8) return false;
        // out = 0; for (const auto& point : this->points) out += int(point.z < minp.z); if (out == 8) return false;

        return true;
     }
}