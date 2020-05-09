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
        auto resourceName = Format(FMT_STRING("Sphere_{0}"), Application::Get()->GenerateResourceId());

        std::vector<decltype(Sphere::GetSphereData(polygons).first)> vbos;
        std::vector<decltype(Sphere::GetSphereData(polygons).second)> ibos;
        AABB aabb{ MakeVector3(-1.0f), MakeVector3(1.0f) };

        for (size_t lod = 0; lod < 6 && polygons > 5; lod++)
        {
            auto data = Sphere::GetSphereData(polygons);
            vbos.push_back(std::move(data.first));
            ibos.push_back(std::move(data.second));
            polygons = (size_t)(polygons * 0.667);
        }
        this->SubmitData(resourceName, aabb, vbos, ibos);
    }

    std::pair<std::vector<float>, std::vector<unsigned int>> Sphere::GetSphereData(size_t polygons)
    {
        std::vector<float> verteces;
        std::vector<unsigned int> indicies;

        verteces.reserve((polygons + 1) * (polygons + 1) * AbstractPrimitive::VertexSize);
        indicies.reserve((polygons + 1) * (polygons + 1) * 6); // two triangles
        // generate raw data for verteces (must be rearranged after)
        for (size_t m = 0; m <= polygons; m++)
        {
            for (size_t n = 0; n <= polygons; n++)
            {
                float x = std::sin(Pi<float>() * m / polygons) * std::cos(2 * Pi<float>() * n / polygons);
                float z = std::sin(Pi<float>() * m / polygons) * std::sin(2 * Pi<float>() * n / polygons);
                float y = -std::cos(Pi<float>() * m / polygons);

                Vector3 T = Normalize(Cross(MakeVector3(0.0f, 1.0f, 0.0f), MakeVector3(x, y, z)));
                Vector3 B = Cross(MakeVector3(x, y, z), T);
                // position
                verteces.push_back(x);
                verteces.push_back(y);
                verteces.push_back(z);
                // texture
                verteces.push_back(1.0f - static_cast<float>(n) / polygons);
                verteces.push_back(static_cast<float>(m) / polygons);
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
}