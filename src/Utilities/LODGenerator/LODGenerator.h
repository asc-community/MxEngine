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

#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Utilities/STL/MxMap.h"
#include "Utilities/STL/MxHashMap.h"

namespace MxEngine
{
    /*!
    This class is used to generate object LODs by comparing vertecies. It is passed as set comparator to filter unique vertecies with some threshold
    \warning TODO: make this class thread-safe and non-static
    */
    struct Vector3Cmp
    {
        /*!
        threshold which sets epsilon to ignore. If vec_abs(v1 - v2) == vec(threshold), v1 considered equal to v2
        */
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

    /*!
    LODGenerator is a special class which encapsulates mesh LOD generation algorithm
    The idea is quite simple, fast and straightforward (but rather inaccurate) - we try to delete vertecies which are close to each other and then reconstruct the mesh
    Ofc this results in slight holes in mesh or its deformation, but corretcly selected distance for LODs will hide such errors
    */
    class LODGenerator
    {
        /*!
        initial object reference. Used to generate LODs and does not changed by LODGenerator
        */
        const MeshData& mesh;

        using ProjectionTable = MxVector<uint32_t>;
        using WeightList = MxHashMap<size_t, size_t>;
        /*!
        table, mapping existing object vertex indicies onto new ones (for example v1->v1, v2->v2, v3->v3 can be transformed to v1->v2, v2->v2, v3->v2)
        */
        ProjectionTable projection;
        /*!
        table, mapping each vertex index to vertex indicies list and its count (to collapse vertecies we do weighted average of their parameters)
        */
        MxVector<WeightList> weights;

        /*!
        this function decies whether we should collapse vertex (f) with other already existed in mesh, or add it unaltered
        \param vertexMapping mesh data for part of already generated mesh LOD
        \param f face (vertex) index to test for (collapse or not)
        \returns mapping of vertex to itself or other existing vertex in submesh
        */
        size_t CollapseDublicate(MxMap<Vector3, size_t, Vector3Cmp>& vertexMapping, size_t f);
        /*
        generates submesh projection and weight tables, collapses vertex duplicates which are close enough to each other
        \param threshold minimal value in vertecies components from which vertecies are considered equal (see Vector3Cmp comparator)
        */
        void PrepareIndexData(float threshold);
    public:
        /*!
        construct LODGenerator object. Calls PrepareIndexData() method. Note that MeshData must not be destroyed till LODGenerator is used
        \param mesh mesh of object from which LODs will be generated
        */
        LODGenerator(const MeshData& mesh);
        /*!
        creates new LOD as MeshData object.
        \param threshold minimal value in vertecies components from which vertecies are considered equal (see Vector3Cmp comparator)
        \returns mesh LOD as MeshData
        \warning this function is not Thread-safe even across multiple LODGenerator instances, as Vector3Cmp uses static field (TODO fix this)
        */
        MeshData CreateObject(float threshold);
    };
}