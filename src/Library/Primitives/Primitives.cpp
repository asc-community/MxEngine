#include "Primitives.h"

namespace MxEngine
{
    Resource<Mesh, ResourceFactory> Primitives::CreateMesh(const AABB& boundingBox, MeshData meshData)
    {
        auto mesh = ResourceFactory::Create<Mesh>();

        SubMesh::MaterialId materialId = 0;
        auto transform = ComponentFactory::CreateComponent<Transform>();

        mesh->SetAABB(boundingBox);
        auto& submeshes = mesh->GetSubmeshes();
        auto& submesh = submeshes.emplace_back(materialId, transform);
        submesh.MeshData = std::move(meshData);
        submesh.MeshData.BufferVertecies();
        submesh.MeshData.BufferIndicies();

        return mesh;
    }

    Primitives::MeshHandle Primitives::CreateCube()
    {
        MeshData meshData;
        AABB aabb{ MakeVector3(-0.5f), MakeVector3(0.5f) };

        std::array position =
        {
            Vector3(-0.5f, -0.5f, -0.5f),
            Vector3(-0.5f, -0.5f,  0.5f),
            Vector3(-0.5f,  0.5f, -0.5f),
            Vector3(-0.5f,  0.5f,  0.5f),
            Vector3(0.5f, -0.5f, -0.5f),
            Vector3(0.5f, -0.5f,  0.5f),
            Vector3(0.5f,  0.5f, -0.5f),
            Vector3(0.5f,  0.5f,  0.5f),
        };
        std::array texture =
        {
            Vector2(0.0f, 0.0f),
            Vector2(0.0f, 1.0f),
            Vector2(1.0f, 0.0f),
            Vector2(1.0f, 1.0f),
        };
        std::array normal =
        {
            Vector3(0.0f,  0.0f,  1.0f),
            Vector3(0.0f,  0.0f, -1.0f),
            Vector3(1.0f,  0.0f,  0.0f),
            Vector3(-1.0f,  0.0f,  0.0f),
            Vector3(0.0f, -1.0f,  0.0f),
            Vector3(0.0f,  1.0f,  0.0f),
        };
        std::array face =
        {
            VectorInt3(1, 0, 0), VectorInt3(5, 2, 0), VectorInt3(7, 3, 0),
            VectorInt3(7, 3, 0), VectorInt3(3, 1, 0), VectorInt3(1, 0, 0),
            VectorInt3(4, 2, 1), VectorInt3(0, 0, 1), VectorInt3(2, 1, 1),
            VectorInt3(2, 1, 1), VectorInt3(6, 3, 1), VectorInt3(4, 2, 1),
            VectorInt3(4, 0, 2), VectorInt3(6, 1, 2), VectorInt3(7, 3, 2),
            VectorInt3(7, 3, 2), VectorInt3(5, 2, 2), VectorInt3(4, 0, 2),
            VectorInt3(2, 0, 3), VectorInt3(0, 1, 3), VectorInt3(1, 3, 3),
            VectorInt3(1, 3, 3), VectorInt3(3, 2, 3), VectorInt3(2, 0, 3),
            VectorInt3(0, 0, 4), VectorInt3(4, 2, 4), VectorInt3(5, 3, 4),
            VectorInt3(5, 3, 4), VectorInt3(1, 1, 4), VectorInt3(0, 0, 4),
            VectorInt3(6, 0, 5), VectorInt3(2, 2, 5), VectorInt3(3, 3, 5),
            VectorInt3(3, 3, 5), VectorInt3(7, 1, 5), VectorInt3(6, 0, 5),
        };

        auto& indicies = meshData.GetIndicies();
        auto& vertecies = meshData.GetVertecies();

        indicies.resize(face.size());
        for (size_t i = 0; i < face.size(); i++)
        {
            indicies[i] = static_cast<uint32_t>(i);

            auto& vertex = vertecies.emplace_back();

            vertex.Position = position[face[i].x];
            vertex.TexCoord = texture[face[i].y];
            vertex.Normal   = normal[face[i].z];

            size_t tanIndex = i - i % 3;
            auto tanbitan   = ComputeTangentSpace(
                position[face[tanIndex].x], position[face[tanIndex + 1].x], position[face[tanIndex + 2].x],
                texture[face[tanIndex].y], texture[face[tanIndex + 1].y], texture[face[tanIndex + 2].y]
            );
            vertex.Tangent   = tanbitan[0];
            vertex.Bitangent = tanbitan[1];
        }
        return Primitives::CreateMesh(aabb, std::move(meshData));
    }

    Primitives::MeshHandle Primitives::CreatePlane(size_t size)
    {
        MeshData meshData;

        auto fsize = static_cast<float>(size);
        auto delta = 0.0001f;
        AABB aabb{ MakeVector3(-fsize, -delta, -fsize), MakeVector3(fsize, delta, fsize) };

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
        return Primitives::CreateMesh(aabb, std::move(meshData));
    }

    Primitives::MeshHandle Primitives::CreateSphere(size_t polygons)
    {
        MeshData meshData;
        AABB aabb{ MakeVector3(-1.0f), MakeVector3(1.0f) };

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
                vertex.Tangent = Normalize(Cross(MakeVector3(0.0f, 1.0f, 0.0f), MakeVector3(x, y, z)));
                vertex.Bitangent = Cross(MakeVector3(x, y, z), vertex.Tangent);

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
        return Primitives::CreateMesh(aabb, std::move(meshData));
    }

    Primitives::MeshHandle Primitives::CreateSurface(const Array2D<float>& heights)
    {
        MeshData meshData;

        MX_ASSERT(heights.size() > 0);
        size_t xsize = heights.width();
        size_t ysize = heights.height();
        size_t triangleCount = 2 * (xsize - 1) * (ysize - 1);

        auto& vertecies = meshData.GetVertecies();
        auto& indicies = meshData.GetIndicies();

        auto minmax = std::minmax_element(heights.begin(), heights.end());
        AABB aabb{ MakeVector3(0.0f, *minmax.first, 0.0f), MakeVector3(1.0f, *minmax.second, 1.0f) };
        auto center = aabb.GetCenter();
        aabb = aabb - center;

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
                vertex.Position -= center;
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
        return Primitives::CreateMesh(aabb, std::move(meshData));
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
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
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