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

#include "LODGenerator.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Format/Format.h"
#include "Utilities/Random/Random.h"

#include <array>
#include <map>

namespace MxEngine
{
    LODGenerator::LODGenerator(const MeshData& mesh)
        : mesh(mesh) { }

    void LODGenerator::PrepareIndexData(float threshold)
    {
        auto& vertecies = mesh.GetVertecies();
        auto& indicies = mesh.GetIndicies();

        projection.resize(vertecies.size(), std::numeric_limits<uint32_t>::max());
        weights.resize(vertecies.size());

        Vector3Cmp::Threshold = threshold;
        MxMap<Vector3, size_t, Vector3Cmp> vertexMapping;
        
        for (size_t i = 0; i < indicies.size(); i += 3)
        {
            std::array<size_t, 3> triangle = {
                indicies[i + 0],
                indicies[i + 1],
                indicies[i + 2],
            };

            for (size_t I = 0; I < 3; I++)
            {
                size_t newI = CollapseDublicate(vertexMapping, triangle[I]);
                projection[triangle[I]] = (uint32_t)newI;
                weights[newI][triangle[I]]++;
            }
        }
        
    }

    size_t LODGenerator::CollapseDublicate(MxMap<Vector3, size_t, Vector3Cmp>& vertexMapping, size_t f)
    {
        const Vector3& Vf = mesh.GetVertecies()[f].Position;

        auto it = vertexMapping.find(Vf);
        if (it == vertexMapping.end())
        {
            vertexMapping[Vf] = f;
            return f;
        }
        else
        {
            return it->second;
        }
    }

    MeshData LODGenerator::CreateObject(float threshold)
    {
        MeshData result;

        threshold = Clamp(threshold, 0.0f, 1.0f);
        if (threshold == 0.0f) //-V550
        {
            result = this->mesh;
            return result;
        }

        Vector3 distance = mesh.GetAABB().Length();
        float averageDistance = Dot(distance, MakeVector3(1.0f / 3.0f));
        if (averageDistance == 0.0f) averageDistance = 1.0f; //-V550
        this->PrepareIndexData(threshold * averageDistance);

        constexpr uint32_t Invalid = std::numeric_limits<uint32_t>::max();

        auto& oldVertecies = mesh.GetVertecies();
        auto& oldIndicies  = mesh.GetIndicies();
        auto& vertecies    = result.GetVertecies();
        auto& indicies     = result.GetIndicies();

        // collapse vertecies. If triangle has pair of equal verticies - ignore it
        indicies.reserve(oldIndicies.size());
        for (size_t f = 0; f < oldIndicies.size(); f += 3)
        {
            uint32_t triangle[] =
            {
                projection[oldIndicies[f + 0]],
                projection[oldIndicies[f + 1]],
                projection[oldIndicies[f + 2]],
            };
            // check if triangle is degenerate or is no longer exists
            if (((triangle[0] != triangle[1]) && (triangle[1] != triangle[2]) && (triangle[2] != triangle[0]))
                && (triangle[0] != Invalid && triangle[1] != Invalid && triangle[2] != Invalid))
            {
                for (size_t F = 0; F < 3; F++) 
                    indicies.push_back((uint32_t)triangle[F]);
            }
        }       

        // shrink vertex buffer by removing not existing vertecies
        constexpr size_t NotExists = std::numeric_limits<size_t>::max();
        std::vector<size_t> indexTable;
        indexTable.assign(projection.size(), NotExists);
        size_t lastId = 0;
        vertecies.reserve(oldVertecies.size());
        for(auto& f : indicies)
        {
            if (indexTable[f] == NotExists)
            {
                auto& newVertex = vertecies.emplace_back();

                size_t total = 0;
                for (const auto& [face, count] : weights[f])
                    total += count;

                for (const auto& [face, count] : weights[f])
                {
                    float weight = (float)count / (float)total;
                    const auto& oldVertex = oldVertecies[face];
                    newVertex.Position   += weight * oldVertex.Position;
                    newVertex.TexCoord   += weight * oldVertex.TexCoord;
                    newVertex.Normal     += weight * oldVertex.Normal;
                    newVertex.Tangent    += weight * oldVertex.Tangent;
                    newVertex.Bitangent  += weight * oldVertex.Bitangent;
                }                        
                indexTable[f] = lastId++;
            }
            f = (uint32_t)indexTable[f];
        }

        indicies.shrink_to_fit();
        vertecies.shrink_to_fit();
        result.BufferIndicies();
        result.BufferVertecies();
        return result;
    }
}