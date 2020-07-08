#include "Primitives.h"

namespace MxEngine
{
    MeshHandle Primitives::CreateMesh(MeshData meshData)
    {
        auto mesh = ResourceFactory::Create<Mesh>();

        SubMesh::MaterialId materialId = 0;
        auto transform = ComponentFactory::CreateComponent<Transform>();

        auto& submeshes = mesh->GetSubmeshes();
        auto& submesh = submeshes.emplace_back(materialId, transform);
        submesh.MeshData = std::move(meshData);
        submesh.MeshData.BufferVertecies();
        submesh.MeshData.BufferIndicies();
        submesh.MeshData.UpdateBoundingBox();

        mesh->UpdateAABB();

        return mesh;
    }

    MeshHandle Primitives::CreateCube(size_t polygons)
    {
        MeshData meshData;
        polygons = polygons + 1; // polygons in [1; inf), but we need at least two points per edge

        auto& vertecies = meshData.GetVertecies();
        auto& indicies = meshData.GetIndicies();

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

        auto& indicies = meshData.GetIndicies();
        auto& vertecies = meshData.GetVertecies();

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

        auto& verteces = meshData.GetVertecies();
        auto& indicies = meshData.GetIndicies();
        verteces.reserve((polygons + 1) * (polygons + 1));

        // generate raw data for verteces (must be rearranged after)
        for (size_t m = 0; m <= polygons; m++)
        {
            for (size_t n = 0; n <= polygons; n++)
            {
                auto& vertex = verteces.emplace_back();

                float x = std::sin(Pi<float>() * m / polygons) * std::cos(2 * Pi<float>() * n / polygons);
                float z = std::sin(Pi<float>() * m / polygons) * std::sin(2 * Pi<float>() * n / polygons);
                float y = std::cos(Pi<float>() * m / polygons) * -1.0f;

                vertex.Position = MakeVector3(x, y, z);
                vertex.Normal = MakeVector3(x, y, z);

                vertex.TexCoord.x = 1.0f - static_cast<float>(n) / polygons;
                vertex.TexCoord.y = static_cast<float>(m) / polygons;
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
        meshData.RegenerateTangentSpace();
        return Primitives::CreateMesh(std::move(meshData));
    }

    MeshHandle Primitives::CreateSurface(const Array2D<float>& heights)
    {
        MeshData meshData;

        MX_ASSERT(heights.size() > 0);
        size_t xsize = heights.width();
        size_t ysize = heights.height();
        size_t triangleCount = 2 * (xsize - 1) * (ysize - 1);

        auto& vertecies = meshData.GetVertecies();
        auto& indicies = meshData.GetIndicies();

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
            }
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

    Primitives::TextureHandle Primitives::CreateGridTexture(size_t textureSize, float borderScale)
    {
        Primitives::TextureHandle gridTexture = GraphicFactory::Create<Texture>();

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
        gridTexture->Load(textureData.data(), (int)textureSize, (int)textureSize, TextureFormat::RGB);
        return gridTexture;
    }
}