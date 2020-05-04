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
#include "Utilities/Array/Array2D.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    class Surface : public AbstractPrimitive
    {
        Array2D<Vector3> surface;
        std::string name;
    public:
        Surface()
        {
            name = Format(FMT_STRING("MxSurface_{0}"), Application::Get()->GenerateResourceId());
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
            MAKE_SCOPE_PROFILER("Surface::GenerateVertexData");

            struct NormalSpace
            {
                Vector3 normal = MakeVector3(0.0f);
                Vector3 tangent = MakeVector3(0.0f);
                Vector3 bitangent = MakeVector3(0.0f);
            };

            std::vector<float> buffer;
            std::vector<unsigned int> indicies;
            Array2D<NormalSpace> normalSpace;
            size_t xsize = surface.width();
            size_t ysize = surface.height();
            size_t triangleCount = 2 * (xsize - 1) * (ysize - 1);
            buffer.reserve(2 * triangleCount * 3 * VertexSize);
            indicies.reserve(2 * triangleCount * 3);
            normalSpace.resize(xsize, ysize);

            AABB boundingBox{ this->surface[0][0], this->surface[0][0] };
            for (const auto& v : this->surface)
            {
                boundingBox.Min = VectorMin(boundingBox.Min, v);
                boundingBox.Max = VectorMax(boundingBox.Max, v);
            }
            auto center = boundingBox.GetCenter();
            boundingBox = boundingBox - center;

            struct Vertex
            {
                Vector3 position;
                Vector2 texture;
                Vector3 normal;
                Vector3 tangent;
                Vector3 bitangent;
            };
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
                    up[0].position   = surface[x + 0][y + 0] - center;
                    up[1].position   = surface[x + 0][y + 1] - center;
                    up[2].position   = surface[x + 1][y + 0] - center;
                    down[0].position = surface[x + 1][y + 1] - center;
                    down[1].position = surface[x + 1][y + 0] - center;
                    down[2].position = surface[x + 0][y + 1] - center;

                    float x1 = float(x + 0) / (float)xsize;
                    float y1 = float(y + 0) / (float)ysize;
                    float x2 = float(x + 1) / (float)ysize;
                    float y2 = float(y + 1) / (float)ysize;
                    up[0].texture   = MakeVector2(x1, y1);
                    up[1].texture   = MakeVector2(x1, y2);
                    up[2].texture   = MakeVector2(x2, y1);
                    down[0].texture = MakeVector2(x2, y2);
                    down[1].texture = MakeVector2(x2, y1);
                    down[2].texture = MakeVector2(x1, y2);

                    Vector3 v1, v2, n;

                    v1 = up[1].position - up[0].position;
                    v2 = up[2].position - up[0].position;
                    n =  Normalize(Cross(v1, v2));
                    auto tanbitan = ComputeTangentSpace(
                        up[0].position, up[1].position, up[2].position,
                        up[0].texture,  up[1].texture,  up[2].texture
                    );

                    normalSpace[x + 0][y + 0].normal += n;
                    normalSpace[x + 1][y + 0].normal += n;
                    normalSpace[x + 0][y + 1].normal += n;
                    normalSpace[x + 0][y + 0].tangent += tanbitan[0];
                    normalSpace[x + 1][y + 0].tangent += tanbitan[0];
                    normalSpace[x + 0][y + 1].tangent += tanbitan[0];
                    normalSpace[x + 0][y + 0].bitangent += tanbitan[1];
                    normalSpace[x + 1][y + 0].bitangent += tanbitan[1];
                    normalSpace[x + 0][y + 1].bitangent += tanbitan[1];

                    v1 = down[1].position - down[0].position;
                    v2 = down[2].position - down[0].position;
                    n =  Normalize(Cross(v1, v2));
                    tanbitan = ComputeTangentSpace(
                        down[0].position, down[1].position, down[2].position,
                        down[0].texture,  down[1].texture,  down[2].texture
                    );

                    normalSpace[x + 1][y + 1].normal += n;
                    normalSpace[x + 0][y + 1].normal += n;
                    normalSpace[x + 1][y + 0].normal += n;
                    normalSpace[x + 1][y + 1].tangent += tanbitan[0];
                    normalSpace[x + 0][y + 1].tangent += tanbitan[0];
                    normalSpace[x + 1][y + 0].tangent += tanbitan[0];
                    normalSpace[x + 1][y + 1].bitangent += tanbitan[1];
                    normalSpace[x + 0][y + 1].bitangent += tanbitan[1];
                    normalSpace[x + 1][y + 0].bitangent += tanbitan[1];
                }
            }

            for (size_t x = 0; x < xsize - 1; x++)
            {
                for (size_t y = 0; y < ysize - 1; y++)
                {
                    auto normalize = [](auto& p)
                    {
                        return p / 6.0f;
                    };

                    auto& up          = triangles[2 * (x * (ysize - 1) + y) + 0];
                    auto& down        = triangles[2 * (x * (ysize - 1) + y) + 1];
                    up[0].normal      = normalSpace[x + 0][y + 0].normal;
                    up[1].normal      = normalSpace[x + 0][y + 1].normal;
                    up[2].normal      = normalSpace[x + 1][y + 0].normal;
                    up[0].tangent     = normalSpace[x + 0][y + 0].tangent;
                    up[1].tangent     = normalSpace[x + 0][y + 1].tangent;
                    up[2].tangent     = normalSpace[x + 1][y + 0].tangent;
                    up[0].bitangent   = normalSpace[x + 0][y + 0].bitangent;
                    up[1].bitangent   = normalSpace[x + 0][y + 1].bitangent;
                    up[2].bitangent   = normalSpace[x + 1][y + 0].bitangent;
                    down[0].normal    = normalSpace[x + 1][y + 1].normal;
                    down[1].normal    = normalSpace[x + 1][y + 0].normal;
                    down[2].normal    = normalSpace[x + 0][y + 1].normal;
                    down[0].tangent   = normalSpace[x + 1][y + 1].tangent;
                    down[1].tangent   = normalSpace[x + 1][y + 0].tangent;
                    down[2].tangent   = normalSpace[x + 0][y + 1].tangent;
                    down[0].bitangent = normalSpace[x + 1][y + 1].bitangent;
                    down[1].bitangent = normalSpace[x + 1][y + 0].bitangent;
                    down[2].bitangent = normalSpace[x + 0][y + 1].bitangent;
                }
            }

            unsigned int indexCounter = 0;
            for(size_t i = 0; i < triangles.size(); i += 2)
            {
                auto& up = triangles[i];
                auto& down = triangles[i + 1];

                constexpr auto add_vertex = [](auto& buffer, auto& vertex)
                {
                    buffer.push_back(vertex.position.x);
                    buffer.push_back(vertex.position.y);
                    buffer.push_back(vertex.position.z);
                    buffer.push_back(vertex.texture.x);
                    buffer.push_back(vertex.texture.y);
                    buffer.push_back(vertex.normal.x);
                    buffer.push_back(vertex.normal.y);
                    buffer.push_back(vertex.normal.z);
                    buffer.push_back(vertex.tangent.x);
                    buffer.push_back(vertex.tangent.y);
                    buffer.push_back(vertex.tangent.z);
                    buffer.push_back(vertex.bitangent.x);
                    buffer.push_back(vertex.bitangent.y);
                    buffer.push_back(vertex.bitangent.z);
                };

                add_vertex(buffer,   up[0]);
                add_vertex(buffer,   up[1]);
                add_vertex(buffer,   up[2]);
                add_vertex(buffer, down[0]);
                
                indicies.push_back(2 * (unsigned int)i + 0);
                indicies.push_back(2 * (unsigned int)i + 1);
                indicies.push_back(2 * (unsigned int)i + 2);
                indicies.push_back(2 * (unsigned int)i + 3);
                indicies.push_back(2 * (unsigned int)i + 2);
                indicies.push_back(2 * (unsigned int)i + 1);
            }

            this->SubmitData(name, boundingBox, make_view(buffer), make_view(indicies));
        }
    };
}