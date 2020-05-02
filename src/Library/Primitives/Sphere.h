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

#include "Library/Primitives/AbstractPrimitive.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
    class Sphere : public AbstractPrimitive
    {
    public:
        inline Sphere(size_t polygons = 30)
        {
            this->Resize(polygons);
        }

        inline void Resize(size_t polygons)
        {
            auto resourceName = Format(FMT_STRING("Sphere_{0}"), Application::Get()->GenerateResourceId());

            std::vector<decltype(Sphere::GetSphereData(polygons).first)> vbos;
            std::vector<decltype(Sphere::GetSphereData(polygons).second)> ibos;
            AABB aabb{MakeVector3(-1.0f), MakeVector3(1.0f)};

            for (size_t lod = 0; lod < 6 && polygons > 5; lod++)
            {
                auto data = Sphere::GetSphereData(polygons);
                vbos.push_back(std::move(data.first));
                ibos.push_back(std::move(data.second));
                polygons = (size_t)(polygons * 0.667);
            }
            this->SubmitData(resourceName, aabb, vbos, ibos);
        }
    private:
        inline static std::pair<std::vector<float>, std::vector<unsigned int>> GetSphereData(size_t polygons)
        {
            std::vector<float> verteces;
            std::vector<unsigned int> indicies;

            verteces.reserve(polygons * polygons * AbstractPrimitive::VertexSize);
            indicies.reserve(polygons * polygons * 6); // two triangles
            // generate raw data for verteces (must be rearranged after)
            for (size_t m = 0; m <= polygons; m++)
            {
                for (size_t n = 0; n <= polygons; n++)
                {
                    float x = std::sin(Pi<float>() * m / polygons) * std::cos(2 * Pi<float>() * n / polygons);
                    float y = std::sin(Pi<float>() * m / polygons) * std::sin(2 * Pi<float>() * n / polygons);
                    float z = std::cos(Pi<float>() * m / polygons);

                    Vector3 T = Normalize(Cross(MakeVector3(0.0f, 1.0f, 0.0f), MakeVector3(x, y, z)));
                    Vector3 B = Cross(MakeVector3(x, y, z), T);
                    // position
                    verteces.push_back(x);
                    verteces.push_back(y);
                    verteces.push_back(z);
                    // texture
                    verteces.push_back(static_cast<float>(m / polygons));
                    verteces.push_back(static_cast<float>(n / polygons));
                    // normal
                    verteces.push_back(x);
                    verteces.push_back(y);
                    verteces.push_back(z);
                    // tangent
                    verteces.push_back(T.x);
                    verteces.push_back(T.y);
                    verteces.push_back(T.z);
                    // bitangent
                    verteces.push_back(B.x);
                    verteces.push_back(B.y);
                    verteces.push_back(B.z);
                }
            }

            // create indicies
            for (size_t i = 0; i < polygons; i++)
            {
                size_t idx1 = i * (polygons + 1);
                size_t idx2 = idx1 + polygons + 1;

                for (size_t j = 0; j < polygons; j++, idx1++, idx2++)
                {
                    if (i != 0)
                    {
                        indicies.push_back((unsigned int)idx1);
                        indicies.push_back((unsigned int)idx2);
                        indicies.push_back((unsigned int)idx1 + 1);
                    }
                    if (i + 1 != polygons)
                    {
                        indicies.push_back((unsigned int)idx1 + 1);
                        indicies.push_back((unsigned int)idx2);
                        indicies.push_back((unsigned int)idx2 + 1);
                    }
                }
            }

            return std::make_pair(std::move(verteces), std::move(indicies));
        }
    };
}