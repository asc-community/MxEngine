// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "Utilities/ObjectLoader/ObjectLoader.h"
#include <set>
#include <map>

namespace MxEngine
{
    struct Vector3Cmp
    {
        inline static float Threshold = 100.0f;

        static bool EqF(float x, float y)
        {
            return std::abs(x - y) <= Threshold;
        }

        static bool LessF(float x, float y) 
        {
            return y - x >= Threshold;
        }

        bool operator()(const Vector3& v1, const Vector3& v2) const
        {
            if (EqF(v1.x, v2.x))
                if (EqF(v1.y, v2.y))
                    return LessF(v1.z, v2.z);
                else
                    return LessF(v1.y, v2.y);
            else
                return LessF(v1.x, v2.x);
        }
    };

    class LODGenerator
    {
        const ObjectInfo& objectLOD;

        using ProjectionTable = std::vector<size_t>;
        using WeightList = std::map<size_t, size_t>;
        std::vector<ProjectionTable> projection;
        std::vector<std::vector<WeightList>> weights;

        size_t CollapseDublicate(std::map<Vector3, size_t, Vector3Cmp>& vertecies, size_t meshId, size_t f);
        void PrepareIndexData(float threshold);
    public:
        LODGenerator(const ObjectInfo& objectInfo);
        // fow now this function can be called only from a single thread!
        ObjectInfo CreateObject(float threshold);
    };
}