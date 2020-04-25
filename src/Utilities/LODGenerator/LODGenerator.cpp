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

#include "LODGenerator.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Format/Format.h"
#include "Utilities/Random/Random.h"

#include <array>
#include <map>

namespace MxEngine
{
    LODGenerator::LODGenerator(const ObjectInfo& objectInfo)
        : objectInfo(objectInfo) { }

    void LODGenerator::PrepareIndexData(float threshold)
    {
        projection.resize(objectInfo.meshes.size());
        weights.resize(objectInfo.meshes.size());

        for (size_t meshIdx = 0; meshIdx < objectInfo.meshes.size(); meshIdx++)
        {
            auto& meshProjection = projection[meshIdx];
            auto& meshWeights = weights[meshIdx];
            const auto& mesh = objectInfo.meshes[meshIdx];

            Vector3Cmp::Threshold = threshold;
            std::map<Vector3, size_t, Vector3Cmp> vertecies;
        
            meshProjection.resize(mesh.GetVertexCount(), std::numeric_limits<size_t>::max());
            meshWeights.resize(mesh.GetVertexCount(), { });
            for (size_t i = 0; i < mesh.faces.size(); i += 3)
            {
                std::array<size_t, 3> triangle = {
                    mesh.faces[i + 0],
                    mesh.faces[i + 1],
                    mesh.faces[i + 2],
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

    size_t LODGenerator::CollapseDublicate(std::map<Vector3, size_t, Vector3Cmp>& vertecies, size_t meshId, size_t f)
    {
        const auto& mesh = objectInfo.meshes[meshId];
        const Vector3& Vf = *reinterpret_cast<const Vector3*>(mesh.buffer.data() + f * mesh.VertexSize + 0);
        const Vector3& Nf = *reinterpret_cast<const Vector3*>(mesh.buffer.data() + f * mesh.VertexSize + 5);

        auto it = vertecies.find(Vf);
        if (it == vertecies.end())
        {
            vertecies[Vf] = f;
            return f;
        }
        else
        {
            // const Vector3& N = *reinterpret_cast<const Vector3*>(mesh.buffer.data() + it->second * mesh.VertexSize + 5);
            return it->second;
            // if (std::abs(Dot(N, Nf)) < 0.01f)
            //     return f;
            // else
            //     return vertecies[Vf];
        }
    }

    ObjectInfo LODGenerator::CreateObject(float threshold)
    {
        ObjectInfo result;

        threshold = Clamp(threshold, 0.0f, 1.0f);
        if (threshold == 0.0f)
        {
            result = objectInfo;
            return std::move(result);
        }

        result.boundingBox = this->objectInfo.boundingBox;
        result.meshes.resize(this->objectInfo.meshes.size());

        Vector3 distance = (result.boundingBox.second - result.boundingBox.first) * 0.5f;
        float averageDistance = (distance.x + distance.y + distance.z) / 3.0f;
        this->PrepareIndexData(threshold * averageDistance);
        
        for (size_t meshIdx = 0; meshIdx < this->objectInfo.meshes.size(); meshIdx++)
        {
            auto& mesh = result.meshes[meshIdx];
            const auto& oldMesh = objectInfo.meshes[meshIdx];
            const auto& meshProjection = this->projection[meshIdx];
            const auto& meshWeights = this->weights[meshIdx];

            mesh.material   = oldMesh.material;
            mesh.useNormal  = oldMesh.useNormal;
            mesh.useTexture = oldMesh.useTexture;
            mesh.name       = oldMesh.name;

            constexpr size_t Invalid = std::numeric_limits<size_t>::max();
            // collapse vertecies. If triangle has pair of equal verticies - ignore it
            mesh.faces.reserve(oldMesh.faces.size());
            for (size_t f = 0; f < oldMesh.faces.size(); f += 3)
            {
                size_t triangle[] = 
                {
                    meshProjection[oldMesh.faces[f + 0]],
                    meshProjection[oldMesh.faces[f + 1]],
                    meshProjection[oldMesh.faces[f + 2]],
                };
                if (((triangle[0] != triangle[1]) && (triangle[1] != triangle[2]) && (triangle[2] != triangle[0]))
                    && (triangle[0] != Invalid && triangle[1] != Invalid && triangle[2] != Invalid))
                {
                    for (size_t F = 0; F < 3; F++) 
                        mesh.faces.push_back((unsigned int)triangle[F]);
                }
            }       

            // shrink vertex buffer by removing not existing vertecies
            constexpr size_t NotExists = std::numeric_limits<size_t>::max();
            std::vector<size_t> indexTable;
            indexTable.assign(meshProjection.size(), NotExists);
            size_t lastId = 0;
            mesh.buffer.reserve(oldMesh.buffer.size());
            for(auto& f : mesh.faces)
            {
                if (indexTable[f] == NotExists)
                {
                    auto V = MakeVector3(0.0f);
                    auto T = MakeVector2(0.0f);
                    auto N = MakeVector3(0.0f);
                    
                    size_t total = 0;
                    for (const auto& [face, count] : meshWeights[f])
                        total += count;

                    for (const auto& [face, count] : meshWeights[f])
                    {
                        float weight = (float)count / (float)total;
                        const Vector3& Vf = *reinterpret_cast<const Vector3*>(oldMesh.buffer.data() + face * oldMesh.VertexSize + 0);
                        const Vector2& Tf = *reinterpret_cast<const Vector3*>(oldMesh.buffer.data() + face * oldMesh.VertexSize + 3);
                        const Vector3& Nf = *reinterpret_cast<const Vector3*>(oldMesh.buffer.data() + face * oldMesh.VertexSize + 5);
                        V += weight * Vf;
                        T += weight * Tf;
                        N += weight * Nf;
                    }

                    mesh.buffer.push_back(V.x);
                    mesh.buffer.push_back(V.y);
                    mesh.buffer.push_back(V.z);
                    mesh.buffer.push_back(T.x);
                    mesh.buffer.push_back(T.y);
                    mesh.buffer.push_back(N.x);
                    mesh.buffer.push_back(N.y);
                    mesh.buffer.push_back(N.z);
                    indexTable[f] = lastId++;
                }
                f = (unsigned int)indexTable[f];
            }

            mesh.faces.shrink_to_fit();
            mesh.buffer.shrink_to_fit();
        }
        return std::move(result);
    }
}