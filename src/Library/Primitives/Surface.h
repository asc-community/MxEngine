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
#include "Utilities/Array/Array2D.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    class Surface : public AbstractPrimitive
    {
        Array2D<Vector3> surface;
        MxString name;
    public:
        Surface()
        {
            name = MxFormat(FMT_STRING("MxSurface_{0}"), Application::Get()->GenerateResourceId());
        }

        template<typename Func>
        void SetSurface(Func&& f, float xsize, float ysize, float step)
        {
            static_assert(std::is_same<float, decltype(f(float(), float()))>::value);
            MX_ASSERT(step  > 0.0f);
            MX_ASSERT(xsize > 0.0f);
            MX_ASSERT(ysize > 0.0f);
            MAKE_SCOPE_PROFILER("Surface::GenerateSurface");

            size_t intxsize = static_cast<size_t>(xsize / step);
            size_t intysize = static_cast<size_t>(ysize / step);
            surface.resize(intxsize, intysize);
            
            for (size_t x = 0; x < intxsize; x++)
            {
                for (size_t y = 0; y < intysize; y++)
                {
                    float fx = x * step;
                    float fy = y * step;
                    float fz = f(fx, fy);
                    surface[x][y] = MakeVector3(fx, fz, fy);
                }
            }
            this->Apply();
        }

        void SetSurface(ArrayView<Vector3> array, size_t xsize, size_t ysize)
        {
            surface.resize(xsize, ysize);
            for (size_t x = 0; x < xsize; x++)
            {
                for (size_t y = 0; y < ysize; y++)
                {
                    surface[x][y] = array[x * ysize + y];
                }
            }
            this->Apply();
        }

        void Apply()
        {
            MeshData meshData;

            MAKE_SCOPE_PROFILER("Surface::GenerateVertexData");

            size_t xsize = surface.width();
            size_t ysize = surface.height();
            size_t triangleCount = 2 * (xsize - 1) * (ysize - 1);

            auto& vertecies = meshData.GetVertecies();
            auto& indicies = meshData.GetIndicies();

            auto minmax = MinMaxComponents(this->surface.begin(), this->surface.size());
            AABB boundingBox{ minmax.first, minmax.second };
            auto center = boundingBox.GetCenter();
            boundingBox = boundingBox - center;

            using Triangle = std::array<Vertex, 3>;

            std::vector<Triangle> triangles;
            triangles.reserve(triangleCount);

            for (size_t x = 0; x < xsize - 1; x++)
            {
                for (size_t y = 0; y < ysize - 1; y++)
                {
                    triangles.emplace_back();
                    Triangle& up = triangles.back();
                    triangles.emplace_back();
                    Triangle& down = triangles.back();
                    up[0].Position   = surface[x + 0][y + 0] - center;
                    up[1].Position   = surface[x + 0][y + 1] - center;
                    up[2].Position   = surface[x + 1][y + 0] - center;
                    down[0].Position = surface[x + 1][y + 1] - center;
                    down[1].Position = surface[x + 1][y + 0] - center;
                    down[2].Position = surface[x + 0][y + 1] - center;

                    float x1 = float(x + 0) / (float)xsize;
                    float y1 = float(y + 0) / (float)ysize;
                    float x2 = float(x + 1) / (float)ysize;
                    float y2 = float(y + 1) / (float)ysize;
                    up[0].TexCoord   = MakeVector2(x1, y1);
                    up[1].TexCoord   = MakeVector2(x1, y2);
                    up[2].TexCoord   = MakeVector2(x2, y1);
                    down[0].TexCoord = MakeVector2(x2, y2);
                    down[1].TexCoord = MakeVector2(x2, y1);
                    down[2].TexCoord = MakeVector2(x1, y2);
                }
            }

            indicies.reserve(triangles.size() * 3);
            vertecies.reserve(triangles.size() * 2);

            for(size_t i = 0; i < triangles.size(); i += 2)
            {
                auto& up = triangles[i];
                auto& down = triangles[i + 1];

                vertecies.push_back(up[0]);
                vertecies.push_back(up[1]);
                vertecies.push_back(up[2]);
                vertecies.push_back(down[0]);
                
                indicies.push_back(2 * (unsigned int)i + 0); //-V525
                indicies.push_back(2 * (unsigned int)i + 1);
                indicies.push_back(2 * (unsigned int)i + 2);
                indicies.push_back(2 * (unsigned int)i + 3);
                indicies.push_back(2 * (unsigned int)i + 2);
                indicies.push_back(2 * (unsigned int)i + 1);
            }
            meshData.RegenerateNormals();
            this->SubmitData(boundingBox, make_view(meshData));
        }
    };
}