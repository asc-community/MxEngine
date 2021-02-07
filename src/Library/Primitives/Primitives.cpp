#include "Primitives.h"

namespace MxEngine
{
    MeshHandle Primitives::CreateMesh(MeshData meshData)
    {
        auto mesh = ResourceFactory::Create<Mesh>();

        auto& submesh = mesh->AddSubMesh((SubMesh::MaterialId)0);
        submesh.Data = std::move(meshData);
        submesh.Data.BufferVertecies();
        submesh.Data.BufferIndicies();
        submesh.Data.UpdateBoundingGeometry();

        mesh->UpdateBoundingGeometry();
        mesh->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("primitive"));

        return mesh;
    }

    MeshHandle Primitives::CreateCube(size_t polygons)
    {
        MeshData meshData;
        auto& vertecies = meshData.GetVerteciesReference();
        auto& indicies = meshData.GetIndiciesReference();

        polygons = polygons + 1; // polygons in [1; inf), but we need at least two points per edge

        for (size_t i = 0; i < polygons; i++)
        {
            for (size_t j = 0; j < polygons; j++)
            {
                auto& vertex = vertecies.emplace_back();
                float offset = 1.0f / (polygons - 1);
                float init = offset < 0.0f ? 0.5f : -0.5f;

                float x = init + offset * j;
                float y = init + offset * i;
                float z = 0.5f;
                vertex.Position  = MakeVector3(x, y, z);
                vertex.TexCoord  = MakeVector2(x, y) + 0.5f;
                vertex.Normal    = MakeVector3(0.0f, 0.0f, 1.0f);
            }
        }
        for (size_t i = 0; i < polygons; i++)
        {
            for (size_t j = 0; j < polygons; j++)
            {
                auto& vertex = vertecies.emplace_back();
                float offset = 1.0f / (polygons - 1);
                float init = offset < 0 ? 0.5f : -0.5f;

                float x = 0.5f;
                float y = init + offset * j;
                float z = init + offset * i;
                vertex.Position = MakeVector3(x, y, z);
                vertex.TexCoord = MakeVector2(-z, y) + 0.5f;
                vertex.Normal = MakeVector3(1.0f, 0.0f, 0.0f);
            }
        }
        for (size_t i = 0; i < polygons; i++)
        {
            for (size_t j = 0; j < polygons; j++)
            {
                auto& vertex = vertecies.emplace_back();
                float offset = -1.0f / (polygons - 1);
                float init = offset < 0 ? 0.5f : -0.5f;

                float x = init + offset * j;
                float y = -0.5f;
                float z = init + offset * i;
                vertex.Position = MakeVector3(x, y, z);
                vertex.TexCoord = MakeVector2(x, z) + 0.5f;
                vertex.Normal = MakeVector3(0.0f, -1.0f, 0.0f);
            }
        }

        for (size_t i = 0; i < polygons; i++)
        {
            for (size_t j = 0; j < polygons; j++)
            {
                auto& vertex = vertecies.emplace_back();
                float offset = 1.0f / (polygons - 1);
                float init = offset < 0 ? 0.5f : -0.5f;

                float x = init + offset * j;
                float y = init + offset * i;
                float z = -0.5f;
                vertex.Position = MakeVector3(x, y, z);
                vertex.TexCoord = MakeVector2(-x, y) + 0.5f;
                vertex.Normal = MakeVector3(0.0f, 0.0f, -1.0f);
            }
        }
        for (size_t i = 0; i < polygons; i++)
        {
            for (size_t j = 0; j < polygons; j++)
            {
                auto& vertex = vertecies.emplace_back();
                float offset = -1.0f / (polygons - 1);
                float init = offset < 0 ? 0.5f : -0.5f;

                float x = init + offset * j;
                float y = 0.5f;
                float z = init + offset * i;
                vertex.Position = MakeVector3(x, y, z);
                vertex.TexCoord = MakeVector2(-x, z) + 0.5f;
                vertex.Normal = MakeVector3(0.0f, 1.0f, 0.0f);
            }
        }
        for (size_t i = 0; i < polygons; i++)
        {
            for (size_t j = 0; j < polygons; j++)
            {
                auto& vertex = vertecies.emplace_back();
                float offset = -1.0f / (polygons - 1);
                float init = offset < 0 ? 0.5f : -0.5f;

                float x = -0.5f;
                float y = init + offset * j;
                float z = init + offset * i;
                vertex.Position = MakeVector3(x, y, z);
                vertex.TexCoord = MakeVector2(z, y) + 0.5f;
                vertex.Normal = MakeVector3(-1.0f, 0.0f, 0.0f);
            }
        }

        for (size_t i = 0; i < 3; i++)
        {
            auto offset = uint32_t(i * polygons * polygons);
            for (size_t x = 0; x < polygons - 1; x++)
            {
                for (size_t y = 0; y < polygons - 1; y++)
                {
                    indicies.push_back(uint32_t(offset + y + x * polygons));
                    indicies.push_back(uint32_t(offset + y + x * polygons + 1));
                    indicies.push_back(uint32_t(offset + y + x * polygons + polygons));
                    indicies.push_back(uint32_t(offset + y + x * polygons + 1));
                    indicies.push_back(uint32_t(offset + y + x * polygons + 1 + polygons));
                    indicies.push_back(uint32_t(offset + y + x * polygons + polygons));
                }
            }
        }
        for (size_t i = 0; i < 3; i++)
        {
            auto offset = uint32_t((i + 3) * polygons * polygons);
            for (size_t x = 0; x < polygons - 1; x++)
            {
                for (size_t y = 0; y < polygons - 1; y++)
                {
                    indicies.push_back(uint32_t(offset + y + x * polygons));
                    indicies.push_back(uint32_t(offset + y + x * polygons + polygons));
                    indicies.push_back(uint32_t(offset + y + x * polygons + 1));
                    indicies.push_back(uint32_t(offset + y + x * polygons + 1));
                    indicies.push_back(uint32_t(offset + y + x * polygons + polygons));
                    indicies.push_back(uint32_t(offset + y + x * polygons + 1 + polygons));
                }
            }
        }
        meshData.RegenerateTangentSpace();

        return Primitives::CreateMesh(std::move(meshData));
    }

    MeshHandle Primitives::CreatePlane(size_t size)
    {
        MeshData meshData;
        auto& vertecies = meshData.GetVerteciesReference();
        auto& indicies = meshData.GetIndiciesReference();

        float gridSize = float(size) / 2.0f;
        std::array vertex =
        {
            Vector3(-gridSize, 0.0f, -gridSize),
            Vector3(-gridSize, 0.0f,  gridSize),
            Vector3( gridSize, 0.0f, -gridSize),
            Vector3( gridSize, 0.0f,  gridSize),
        };
        std::array texture =
        {
            Vector2(0.0f,               0.0f),
            Vector2(0.0f,        size * 1.0f),
            Vector2(size * 1.0f,        0.0f),
            Vector2(size * 1.0f, size * 1.0f),
        };

        vertecies.resize(vertex.size());
        for (size_t i = 0; i < vertecies.size(); i++)
        {
            vertecies[i].Position = vertex[i];
            vertecies[i].TexCoord = texture[i];
            vertecies[i].Normal = MakeVector3(0.0f, 1.0f, 0.0f);
            vertecies[i].Tangent = MakeVector3(1.0f, 0.0f, 0.0f);
            vertecies[i].Bitangent  = MakeVector3(0.0f, 0.0f, 1.0f);
        }
        indicies = { 0, 1, 2, 2, 1, 3, 0, 2, 1, 1, 2, 3 };
        return Primitives::CreateMesh(std::move(meshData));
    }

    MeshHandle Primitives::CreateSphere(size_t polygons)
    {
        MeshData meshData;
        auto& vertecies = meshData.GetVerteciesReference();
        auto& indicies = meshData.GetIndiciesReference();

        polygons -= polygons % 4;
        vertecies.reserve((polygons + 1) * (polygons + 1));

        // generate raw data for verteces (must be rearranged after)
        for (size_t m = 0; m <= polygons; m++)
        {
            for (size_t n = 0; n <= polygons; n++)
            {
                auto& vertex = vertecies.emplace_back();

                float x = std::sin(Pi<float>() * m / polygons) * std::cos(2 * Pi<float>() * n / polygons);
                float z = std::sin(Pi<float>() * m / polygons) * std::sin(2 * Pi<float>() * n / polygons);
                float y = std::cos(Pi<float>() * m / polygons) * -1.0f;

                vertex.Position = 0.5f * MakeVector3(x, y, z);
                vertex.Normal = MakeVector3(x, y, z);

                vertex.TexCoord.x = 1.0f - static_cast<float>(n) / polygons;
                vertex.TexCoord.y = static_cast<float>(m) / polygons;

                if (z == 0.0f && x == 0.0f)
                    vertex.Tangent = MakeVector3(-1.0f, 0.0f, 1.0f);
                else
                    vertex.Tangent =  MakeVector3(-z, 0, x);
                vertex.Bitangent = Cross(vertex.Normal, vertex.Tangent);
            }
        }

        // create indicies
        indicies.reserve(polygons * (polygons - 1) * 6);
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
        return Primitives::CreateMesh(std::move(meshData));
    }

    MeshHandle Primitives::CreateCylinder(size_t polygons)
    {
        MeshData meshData;
        auto& vertecies = meshData.GetVerteciesReference();
        auto& indicies = meshData.GetIndiciesReference();

        polygons = Max(polygons - polygons % 4, 4);
        MxVector<Vector2> circle(polygons);
        for (size_t i = 0; i < polygons; i++)
        {
            float angle = TwoPi<float>() * float(i) / float(polygons);
            circle[i].x = 0.5f * std::sin(angle);
            circle[i].y = 0.5f * std::cos(angle);
        }

        // format: 
        // [center upper circle vertex]
        // [x vertecies for upper circle]
        // [center lower circle vertex]
        // [x vertecies for lower circle]
        // [2x vertecies for cylinder face]

        Vertex upper;
        upper.Position = MakeVector3(0.0f, 0.5f, 0.0f);
        upper.TexCoord = MakeVector2(0.5f, 0.5f);
        vertecies.push_back(upper);
        for (const auto& pos : circle)
        {
            Vertex v;
            v.Position = MakeVector3(pos.x, 0.5f, pos.y);
            v.TexCoord = pos + 0.5f;
            vertecies.push_back(v);
        }

        Vertex lower;
        lower.Position = MakeVector3(0.0f, -0.5f, 0.0f);
        lower.TexCoord = MakeVector2(0.5f, 0.5f);
        vertecies.push_back(lower);
        for (const auto& pos : circle)
        {
            Vertex v;
            v.Position = MakeVector3(pos.x, -0.5f, pos.y);
            v.TexCoord = pos + 0.5f;
            vertecies.push_back(v);
        }

        for (size_t i = 0; i < circle.size(); i++)
        {
            const auto& pos = circle[i];
            Vertex v1, v2;
            v1.Position = MakeVector3(pos.x, 0.5f, pos.y);
            v1.TexCoord = MakeVector2(float(i) / float(circle.size()), 1.0f);
            v2.Position = MakeVector3(pos.x, -0.5f, pos.y);
            v2.TexCoord = MakeVector2(float(i) / float(circle.size()), 0.0f);
            vertecies.push_back(v1);
            vertecies.push_back(v2);
        }

        size_t offset = 0;
        // upper circle
        offset = 1;
        for (size_t i = 1; i < polygons; i++)
        {
            indicies.push_back(uint32_t(offset + i - 1));
            indicies.push_back(uint32_t(offset + i));
            indicies.push_back(uint32_t(offset - 1));
        }
        indicies.push_back(uint32_t(offset + polygons - 1));
        indicies.push_back(uint32_t(offset));
        indicies.push_back(uint32_t(offset - 1));

        // lower circle
        offset = 2 + polygons;
        for (size_t i = 1; i < polygons; i++)
        {
            indicies.push_back(uint32_t(offset + i - 1));
            indicies.push_back(uint32_t(offset - 1));
            indicies.push_back(uint32_t(offset + i));
        }
        indicies.push_back(uint32_t(offset + polygons - 1));
        indicies.push_back(uint32_t(offset - 1));
        indicies.push_back(uint32_t(offset));

        offset = 2 + 2 * polygons;

        for (size_t i = 1; i < polygons; i++)
        {
            size_t upperL = offset + 2 * i - 2;
            size_t lowerL = offset + 2 * i - 1;
            size_t upperR = offset + 2 * i;
            size_t lowerR = offset + 2 * i + 1;
            indicies.push_back(uint32_t(upperL));
            indicies.push_back(uint32_t(lowerL));
            indicies.push_back(uint32_t(upperR));
            indicies.push_back(uint32_t(upperR));
            indicies.push_back(uint32_t(lowerL));
            indicies.push_back(uint32_t(lowerR));
        }
        size_t upperL = offset + 2 * polygons - 2;
        size_t lowerL = offset + 2 * polygons - 1;
        size_t upperR = offset;
        size_t lowerR = offset + 1;
        indicies.push_back(uint32_t(upperL));
        indicies.push_back(uint32_t(lowerL));
        indicies.push_back(uint32_t(upperR));
        indicies.push_back(uint32_t(upperR));
        indicies.push_back(uint32_t(lowerL));
        indicies.push_back(uint32_t(lowerR));

        meshData.RegenerateNormals();
        return Primitives::CreateMesh(std::move(meshData));
    }

    MeshHandle Primitives::CreatePyramid()
    {
        MeshData meshData;
        auto& vertecies = meshData.GetVerteciesReference();
        auto& indicies = meshData.GetIndiciesReference();

        vertecies.resize(5);
        vertecies[0].Position = Vector3( 0.0f,  0.0f, 0.0f);
        vertecies[0].TexCoord = Vector2( 0.5f,  0.5f);
        vertecies[1].Position = Vector3(-1.0f, -1.0f, 1.0f);
        vertecies[1].TexCoord = Vector2( 0.0f,  0.0f);
        vertecies[2].Position = Vector3(-1.0f,  1.0f, 1.0f);
        vertecies[2].TexCoord = Vector2( 0.0f,  1.0f);
        vertecies[3].Position = Vector3( 1.0f,  1.0f, 1.0f);
        vertecies[3].TexCoord = Vector2( 1.0f,  1.0f);
        vertecies[4].Position = Vector3( 1.0f, -1.0f, 1.0f);
        vertecies[4].TexCoord = Vector2( 1.0f,  0.0f);

        indicies = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 1,
            1, 3, 2,
            1, 4, 3,
        };

        meshData.RegenerateNormals();
        return Primitives::CreateMesh(std::move(meshData));
    }

    MeshHandle Primitives::CreateSurface(const Array2D<float>& heights)
    {
        MeshData meshData;
        auto& vertecies = meshData.GetVerteciesReference();
        auto& indicies = meshData.GetIndiciesReference();

        MX_ASSERT(heights.size() > 0);
        size_t xsize = heights.width();
        size_t ysize = heights.height();

        float minY = 0.0f;
        float maxY = 0.0f;

        vertecies.reserve(xsize * ysize);
        for (size_t x = 0; x < xsize; x++)
        {
            for (size_t y = 0; y < ysize; y++)
            {
                auto& vertex = vertecies.emplace_back();
                float fx = float(x) / (float)xsize;
                float fy = float(y) / (float)ysize;
                float fz = heights[x][y];

                vertex.TexCoord = MakeVector2(fx, fy);

                // yes, z component is actually y component, because in such case its easier to think 
                // of plane as OXY, with heights pointing towards z axis
                vertex.Position = MakeVector3(fx, fz, fy);
                minY = Min(minY, fz);
                maxY = Max(maxY, fz);
            }
        }

        for (auto& vertex : vertecies)
        {
            vertex.Position -= MakeVector3(0.5f, (maxY + minY) * 0.5f, 0.5f);
        }

        indicies.reserve((xsize - 1) * (ysize - 1) * 6);
        for (size_t x = 0; x < xsize - 1; x++)
        {
            for (size_t y = 0; y < ysize - 1; y++)
            {
                indicies.push_back(uint32_t(y + x * ysize));
                indicies.push_back(uint32_t(y + x * ysize + 1));
                indicies.push_back(uint32_t(y + x * ysize + ysize));
                indicies.push_back(uint32_t(y + x * ysize + 1));
                indicies.push_back(uint32_t(y + x * ysize + 1 + ysize));
                indicies.push_back(uint32_t(y + x * ysize + ysize));
            }
        }
        meshData.RegenerateNormals();
        return Primitives::CreateMesh(std::move(meshData));
    }

    TextureHandle Primitives::CreateGridTexture(size_t textureSize, float borderScale)
    {
        TextureHandle gridTexture = GraphicFactory::Create<Texture>();

        Array2D<uint8_t> textureData;
        textureData.resize(textureSize, textureSize * 3);

        constexpr auto draw_border = [](Array2D<uint8_t>& texture, size_t borderSize)
        {
            size_t width = texture.width();
            size_t height = texture.height() / 3;
            for (size_t i = 0; i < width; i++)
            {
                for (size_t j = 0; j < height; j++)
                {
                    if (i < borderSize || i + borderSize >= width ||
                        j < borderSize || j + borderSize >= height)
                    {
                        texture[i][j * 3 + 0] = 0;
                        texture[i][j * 3 + 1] = 0;
                        texture[i][j * 3 + 2] = 0;
                    }
                }
            }
        };

        draw_border(textureData, static_cast<size_t>(textureSize * borderScale));
        gridTexture->Load(textureData.data(), (int)textureSize, (int)textureSize, 3, false, TextureFormat::RGB);
        return gridTexture;
    }
}