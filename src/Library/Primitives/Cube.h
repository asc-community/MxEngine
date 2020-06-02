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

#include "Library/Primitives/AbstractPrimitive.h"
#include "Utilities/Format/Format.h"
#include <array>

namespace MxEngine
{
    class Cube : public AbstractPrimitive
    {
    public:

        inline Cube()
        {
            auto data = Cube::GetCubeData();
            AABB aabb { MakeVector3(-0.5f), MakeVector3(0.5f) };
            this->SubmitData(aabb, make_view(data));
        }

    private:
        static MeshData GetCubeData()
        {
            MeshData meshData;
            constexpr std::array position =
            {
                Vector3(-0.5f, -0.5f, -0.5f),
                Vector3(-0.5f, -0.5f,  0.5f),
                Vector3(-0.5f,  0.5f, -0.5f),
                Vector3(-0.5f,  0.5f,  0.5f),
                Vector3(0.5f, -0.5f, -0.5f),
                Vector3(0.5f, -0.5f,  0.5f),
                Vector3(0.5f,  0.5f, -0.5f),
                Vector3(0.5f,  0.5f,  0.5f),
            };
            constexpr std::array texture =
            {
                Vector2(0.0f, 0.0f),
                Vector2(0.0f, 1.0f),
                Vector2(1.0f, 0.0f),
                Vector2(1.0f, 1.0f),
            };
            constexpr std::array normal =
            {
                Vector3(0.0f,  0.0f,  1.0f),
                Vector3(0.0f,  0.0f, -1.0f),
                Vector3(1.0f,  0.0f,  0.0f),
                Vector3(-1.0f,  0.0f,  0.0f),
                Vector3(0.0f, -1.0f,  0.0f),
                Vector3(0.0f,  1.0f,  0.0f),
            };
            constexpr std::array face =
            {
                VectorInt3(1, 0, 0), VectorInt3(5, 2, 0), VectorInt3(7, 3, 0),
                VectorInt3(7, 3, 0), VectorInt3(3, 1, 0), VectorInt3(1, 0, 0),
                VectorInt3(4, 2, 1), VectorInt3(0, 0, 1), VectorInt3(2, 1, 1),
                VectorInt3(2, 1, 1), VectorInt3(6, 3, 1), VectorInt3(4, 2, 1),
                VectorInt3(4, 0, 2), VectorInt3(6, 1, 2), VectorInt3(7, 3, 2),
                VectorInt3(7, 3, 2), VectorInt3(5, 2, 2), VectorInt3(4, 0, 2),
                VectorInt3(2, 0, 3), VectorInt3(0, 1, 3), VectorInt3(1, 3, 3),
                VectorInt3(1, 3, 3), VectorInt3(3, 2, 3), VectorInt3(2, 0, 3),
                VectorInt3(0, 0, 4), VectorInt3(4, 2, 4), VectorInt3(5, 3, 4),
                VectorInt3(5, 3, 4), VectorInt3(1, 1, 4), VectorInt3(0, 0, 4),
                VectorInt3(6, 0, 5), VectorInt3(2, 2, 5), VectorInt3(3, 3, 5),
                VectorInt3(3, 3, 5), VectorInt3(7, 1, 5), VectorInt3(6, 0, 5),
            };
            
            auto& indicies = meshData.GetIndicies();
            auto& vertecies = meshData.GetVertecies();

            indicies.resize(face.size());
            for (size_t i = 0; i < face.size(); i++)
            {
                indicies[i] = (unsigned int)i;
                
                auto& vertex = vertecies.emplace_back();

                vertex.Position = position[face[i].x];
                vertex.TexCoord = texture[face[i].y];
                vertex.Normal   = normal[face[i].z];

                size_t tanIndex = i - i % 3;
                auto tanbitan = ComputeTangentSpace(
                    position[face[tanIndex].x], position[face[tanIndex + 1].x], position[face[tanIndex + 2].x],
                     texture[face[tanIndex].y],  texture[face[tanIndex + 1].y],  texture[face[tanIndex + 2].y]
                );
                vertex.Tangent   = tanbitan[0];
                vertex.Bitangent = tanbitan[1];
            }
            return meshData;
        }
    };
}