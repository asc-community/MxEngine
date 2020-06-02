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
    LODGenerator::LODGenerator(const ObjectInfo& objectInfo)
        : objectLOD(objectInfo)
    {
        projection.resize(this->objectLOD.meshes.size());
        weights.resize(this->objectLOD.meshes.size());
    }

    void LODGenerator::PrepareIndexData(float threshold)
    {
        for (size_t meshIdx = 0; meshIdx < this->objectLOD.meshes.size(); meshIdx++)
        {
            auto& meshProjection = projection[meshIdx];
            auto& meshWeights = weights[meshIdx];
            const auto& mesh = this->objectLOD.meshes[meshIdx];

            size_t vertexCount = mesh.GetVertexCount();
            meshProjection.resize(vertexCount, std::numeric_limits<size_t>::max());
            meshWeights.resize(vertexCount);

            Vector3Cmp::Threshold = threshold;
            MxMap<Vector3, size_t, Vector3Cmp> vertecies;
        
            for (size_t i = 0; i < mesh.indicies.size(); i += 3)
            {
                std::array<size_t, 3> triangle = {
                    mesh.indicies[i + 0],
                    mesh.indicies[i + 1],
                    mesh.indicies[i + 2],
                };

                for (size_t I = 0; I < 3; I++)
                {
                    size_t newI = CollapseDublicate(vertecies, meshIdx, triangle[I]);
                    meshProjection[triangle[I]] = newI;
                    meshWeights[newI][triangle[I]]++;
                }
            }
        }
    }

    size_t LODGenerator::CollapseDublicate(MxMap<Vector3, size_t, Vector3Cmp>& vertecies, size_t meshId, size_t f)
    {
        const auto& mesh = this->objectLOD.meshes[meshId];
        const Vector3& Vf = mesh.vertecies[f].Position;

        auto it = vertecies.find(Vf);
        if (it == vertecies.end())
        {
            vertecies[Vf] = f;
            return f;
        }
        else
        {
            return it->second;
        }
    }

    ObjectInfo LODGenerator::CreateObject(float threshold)
    {
        ObjectInfo result;

        threshold = Clamp(threshold, 0.0f, 1.0f);
        if (threshold == 0.0f) //-V550
        {
            result = this->objectLOD;
            return std::move(result);
        }

        result.boundingBox = this->objectLOD.boundingBox;
        result.meshes.resize(this->objectLOD.meshes.size());

        Vector3 distance = result.boundingBox.Length();
        float averageDistance = (distance.x + distance.y + distance.z) / 3.0f;
        this->PrepareIndexData(threshold * averageDistance);
        
        for (size_t meshIdx = 0; meshIdx < this->objectLOD.meshes.size(); meshIdx++)
        {
            auto& mesh = result.meshes[meshIdx];
            const auto& oldMesh = objectLOD.meshes[meshIdx];
            const auto& meshProjection = this->projection[meshIdx];
            const auto& meshWeights = this->weights[meshIdx];

            mesh.material   = oldMesh.material;
            mesh.useNormal  = oldMesh.useNormal;
            mesh.useTexture = oldMesh.useTexture;
            mesh.name       = oldMesh.name;

            constexpr size_t Invalid = std::numeric_limits<size_t>::max();
            // collapse vertecies. If triangle has pair of equal verticies - ignore it
            mesh.indicies.reserve(oldMesh.indicies.size());
            for (size_t f = 0; f < oldMesh.indicies.size(); f += 3)
            {
                size_t triangle[] = 
                {
                    meshProjection[oldMesh.indicies[f + 0]],
                    meshProjection[oldMesh.indicies[f + 1]],
                    meshProjection[oldMesh.indicies[f + 2]],
                };
                if (((triangle[0] != triangle[1]) && (triangle[1] != triangle[2]) && (triangle[2] != triangle[0]))
                    && (triangle[0] != Invalid && triangle[1] != Invalid && triangle[2] != Invalid))
                {
                    for (size_t F = 0; F < 3; F++) 
                        mesh.indicies.push_back((unsigned int)triangle[F]);
                }
            }       

            // shrink vertex buffer by removing not existing vertecies
            constexpr size_t NotExists = std::numeric_limits<size_t>::max();
            std::vector<size_t> indexTable;
            indexTable.assign(meshProjection.size(), NotExists);
            size_t lastId = 0;
            mesh.vertecies.reserve(oldMesh.vertecies.size());
            for(auto& f : mesh.indicies)
            {
                if (indexTable[f] == NotExists)
                {
                    auto& newVertex = mesh.vertecies.emplace_back();

                    size_t total = 0;
                    for (const auto& [face, count] : meshWeights[f])
                        total += count;

                    for (const auto& [face, count] : meshWeights[f])
                    {
                        float weight = (float)count / (float)total;
                        const auto& oldVertex = oldMesh.vertecies[face];
                        newVertex.Position   += weight * oldVertex.Position;
                        newVertex.TexCoord   += weight * oldVertex.TexCoord;
                        newVertex.Normal     += weight * oldVertex.Normal;
                        newVertex.Tangent    += weight * oldVertex.Tangent;
                        newVertex.Bitangent  += weight * oldVertex.Bitangent;
                    }                        
                    indexTable[f] = lastId++;
                }
                f = (unsigned int)indexTable[f];
            }

            mesh.indicies.shrink_to_fit();
            mesh.vertecies.shrink_to_fit();
        }
        return std::move(result);
    }
}