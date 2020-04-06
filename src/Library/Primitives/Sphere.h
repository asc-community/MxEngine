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
            auto resourceName = Format(FMT_STRING("MxSphere_{0}"), Application::Get()->GenerateResourceId());
            auto data = Sphere::GetSphereData(polygons);
            this->SubmitData(resourceName, data);
        }
    private:
        inline static ArrayView<float> GetSphereData(size_t polygons)
        {
            static std::vector<float> data; // data must be static as its view is returned

            data.clear();
            data.reserve(polygons * polygons * 6 * AbstractPrimitive::VertexSize);
            std::vector<float> verteces;
            verteces.reserve(polygons * polygons * AbstractPrimitive::VertexSize);
            // generate raw data for verteces (must be rearranged after)
            for (size_t m = 0; m <= polygons; m++)
            {
                for (size_t n = 0; n <= polygons; n++)
                {
                    float x = std::sin(Pi<float>() * m / polygons) * std::cos(2 * Pi<float>() * n / polygons);
                    float y = std::sin(Pi<float>() * m / polygons) * std::sin(2 * Pi<float>() * n / polygons);
                    float z = std::cos(Pi<float>() * m / polygons);
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
                }
            }

            // rearrange data
            for (size_t i = 0; i < polygons; i++)
            {
                size_t idx1 = i * (polygons + 1);
                size_t idx2 = idx1 + polygons + 1;

                for (size_t j = 0; j < polygons; j++, idx1++, idx2++)
                {
                    if (i != 0)
                    {
                        data.insert(data.end(), verteces.begin() + idx1 * VertexSize, verteces.begin() + (idx1 + 1) * VertexSize);
                        data.insert(data.end(), verteces.begin() + idx2 * VertexSize, verteces.begin() + (idx2 + 1) * VertexSize);
                        data.insert(data.end(), verteces.begin() + (idx1 + 1) * VertexSize, verteces.begin() + (idx1 + 2) * VertexSize);
                    }
                    if (i + 1 != polygons)
                    {
                        data.insert(data.end(), verteces.begin() + (idx1 + 1) * VertexSize, verteces.begin() + (idx1 + 2) * VertexSize);
                        data.insert(data.end(), verteces.begin() + idx2 * VertexSize, verteces.begin() + (idx2 + 1) * VertexSize);
                        data.insert(data.end(), verteces.begin() + (idx2 + 1) * VertexSize, verteces.begin() + (idx2 + 2) * VertexSize);
                    }
                }
            }

            return ArrayView<float>(data);
        }
    };
}