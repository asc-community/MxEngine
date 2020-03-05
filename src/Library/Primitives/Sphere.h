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

namespace MxEngine
{
    class Sphere : public AbstractPrimitive
    {
        inline static size_t lastPolygons = 0;
        inline static std::vector<float> Data;
    public:
        inline Sphere(size_t polygons = 30)
        {
            if (lastPolygons != polygons)
            {
                UpdateDataBuffer(polygons);
                lastPolygons = polygons;
            }
            this->SubmitData(Data);
        }

        inline void Resize(size_t polygons)
        {
            this->UpdateDataBuffer(polygons);
            lastPolygons = polygons;
            this->SubmitData(Data);
        }
    private:
        inline static void UpdateDataBuffer(size_t polygons)
        {
            Data.clear();
            Data.reserve(polygons * polygons * 6 * AbstractPrimitive::VertexSize);
            std::vector<float> vertex;
            vertex.reserve(polygons * polygons * AbstractPrimitive::VertexSize);
            for (size_t m = 0; m <= polygons; m++)
            {
                for (size_t n = 0; n <= polygons; n++)
                {
                    float x = std::sin(Pi<float>() * m / polygons) * std::cos(2 * Pi<float>() * n / polygons);
                    float y = std::sin(Pi<float>() * m / polygons) * std::sin(2 * Pi<float>() * n / polygons);
                    float z = std::cos(Pi<float>() * m / polygons);
                    // position
                    vertex.push_back(x);
                    vertex.push_back(y);
                    vertex.push_back(z);
                    // texture
                    vertex.push_back(static_cast<float>(m / polygons));
                    vertex.push_back(static_cast<float>(n / polygons));
                    // normal
                    vertex.push_back(x);
                    vertex.push_back(y);
                    vertex.push_back(z);
                }
            }

            for (size_t i = 0; i < polygons; i++)
            {
                size_t idx1 = i * (polygons + 1);
                size_t idx2 = idx1 + polygons + 1;

                for (size_t j = 0; j < polygons; j++, idx1++, idx2++)
                {
                    if (i != 0)
                    {
                        Data.insert(Data.end(), vertex.begin() + idx1 * VertexSize, vertex.begin() + (idx1 + 1) * VertexSize);
                        Data.insert(Data.end(), vertex.begin() + idx2 * VertexSize, vertex.begin() + (idx2 + 1) * VertexSize);
                        Data.insert(Data.end(), vertex.begin() + (idx1 + 1) * VertexSize, vertex.begin() + (idx1 + 2) * VertexSize);
                    }
                    if (i + 1 != polygons)
                    {
                        Data.insert(Data.end(), vertex.begin() + (idx1 + 1) * VertexSize, vertex.begin() + (idx1 + 2) * VertexSize);
                        Data.insert(Data.end(), vertex.begin() + idx2 * VertexSize, vertex.begin() + (idx2 + 1) * VertexSize);
                        Data.insert(Data.end(), vertex.begin() + (idx2 + 1) * VertexSize, vertex.begin() + (idx2 + 2) * VertexSize);
                    }
                }
            }
        }
    };
}