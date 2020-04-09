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
            assert(step  > 0.0f);
            assert(xsize > 0.0f);
            assert(ysize > 0.0f);
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
            std::vector<float> buffer;
            Array2D<Vector3> pointNormals;
            size_t xsize = surface.width();
            size_t ysize = surface.height();
            size_t triangleCount = 2 * (xsize - 1) * (ysize - 1);
            buffer.reserve(2 * triangleCount * VertexSize);
            pointNormals.resize(xsize, ysize, MakeVector3(0.0f));

            struct Vertex
            {
                Vector3 position;
                Vector2 texture;
                Vector3 normal;
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
                    up[0].position   = surface[x + 0][y + 0];
                    up[1].position   = surface[x + 0][y + 1];
                    up[2].position   = surface[x + 1][y + 0];
                    down[0].position = surface[x + 1][y + 1];
                    down[1].position = surface[x + 1][y + 0];
                    down[2].position = surface[x + 0][y + 1];

                    up[0].texture   = MakeVector2(0.0f, 0.0f);
                    up[1].texture   = MakeVector2(0.0f, 1.0f);
                    up[2].texture   = MakeVector2(1.0f, 0.0f);
                    down[0].texture = MakeVector2(1.0f, 1.0f);
                    down[1].texture = MakeVector2(1.0f, 0.0f);
                    down[2].texture = MakeVector2(0.0f, 1.0f);

                    Vector3 v1, v2, n;

                    v1 = up[1].position - up[0].position;
                    v2 = up[2].position - up[0].position;
                    n =  Normalize(Cross(v1, v2));

                    pointNormals[x + 0][y + 0] += n;
                    pointNormals[x + 1][y + 0] += n;
                    pointNormals[x + 0][y + 1] += n;

                    v1 = down[1].position - down[0].position;
                    v2 = down[2].position - down[0].position;
                    n =  Normalize(Cross(v1, v2));

                    pointNormals[x + 1][y + 1] += n;
                    pointNormals[x + 0][y + 1] += n;
                    pointNormals[x + 1][y + 0] += n;
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

                    auto& up       = triangles[2 * (x * (ysize - 1) + y) + 0];
                    auto& down     = triangles[2 * (x * (ysize - 1) + y) + 1];
                    up[0].normal   = pointNormals[x + 0][y + 0];
                    up[1].normal   = pointNormals[x + 0][y + 1];
                    up[2].normal   = pointNormals[x + 1][y + 0];
                    down[0].normal = pointNormals[x + 1][y + 1];
                    down[1].normal = pointNormals[x + 1][y + 0];
                    down[2].normal = pointNormals[x + 0][y + 1];
                }
            }

            for (const auto& triangle : triangles)
            {
                for (const auto& vertex : triangle)
                {
                    buffer.push_back(vertex.position.x);
                    buffer.push_back(vertex.position.y);
                    buffer.push_back(vertex.position.z);
                    buffer.push_back(vertex.texture.x);
                    buffer.push_back(vertex.texture.y);
                    buffer.push_back(vertex.normal.x);
                    buffer.push_back(vertex.normal.y);
                    buffer.push_back(vertex.normal.z);
                }
            }

            this->SubmitData(name, buffer);
        }
    };
}