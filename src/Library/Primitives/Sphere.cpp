#include "Sphere.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    Sphere::Sphere(size_t polygons)
    {
        this->Resize(polygons);
    }

    void Sphere::Resize(size_t polygons)
    {
        MxVector<MeshData> lods;
        AABB aabb{ MakeVector3(-1.0f), MakeVector3(1.0f) };

        for (size_t lod = 0; lod < 6 && polygons > 5; lod++)
        {
            lods.push_back(Sphere::GetSphereData(polygons));
        }
        this->SubmitData(aabb, array_view(lods));
    }

    MeshData Sphere::GetSphereData(size_t polygons)
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

                float x =  std::sin(Pi<float>() * m / polygons) * std::cos(2 * Pi<float>() * n / polygons);
                float z =  std::sin(Pi<float>() * m / polygons) * std::sin(2 * Pi<float>() * n / polygons);
                float y = -std::cos(Pi<float>() * m / polygons);

                vertex.Position  = MakeVector3(x, y, z);
                vertex.Normal    = MakeVector3(x, y, z);
                vertex.Tangent   = Normalize(Cross(MakeVector3(0.0f, 1.0f, 0.0f), MakeVector3(x, y, z)));
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

        return meshData;
    }
}