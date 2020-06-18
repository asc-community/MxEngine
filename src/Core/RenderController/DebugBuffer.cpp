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

#include "DebugBuffer.h"

namespace MxEngine
{
    void DebugBuffer::Init()
    {
        auto VBL = GraphicFactory::Create<VertexBufferLayout>();
        VBL->PushFloat(3); // position
        VBL->PushFloat(4); // color

        this->VBO = GraphicFactory::Create<VertexBuffer>(nullptr, 0, UsageType::DYNAMIC_DRAW);
        this->VAO = GraphicFactory::Create<VertexArray>();
        VAO->AddBuffer(*this->VBO, *VBL);
    }

    void DebugBuffer::SubmitAABB(const AABB& box, const Vector4& color)
    {
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
    }

    auto InitializeSphere()
    {
        std::vector<Vector3> linesData;
        std::vector<Vector3> vertecies;
        constexpr size_t SpherePolygons = 8;
        vertecies.reserve((SpherePolygons + 1) * (SpherePolygons + 1));
        linesData.reserve((SpherePolygons + 1) * (SpherePolygons + 1) * 12);

        for (size_t m = 0; m <= SpherePolygons; m++)
        {
            for (size_t n = 0; n <= SpherePolygons; n++)
            {
                float x = std::sin(Pi<float>() * m / SpherePolygons) * std::cos(2 * Pi<float>() * n / SpherePolygons);
                float y = std::sin(Pi<float>() * m / SpherePolygons) * std::sin(2 * Pi<float>() * n / SpherePolygons);
                float z = std::cos(Pi<float>() * m / SpherePolygons);
                vertecies.emplace_back(x, y, z);
            }
        }

        for (size_t i = 0; i < SpherePolygons; i++)
        {
            size_t idx1 = i * (SpherePolygons + 1);
            size_t idx2 = idx1 + SpherePolygons + 1;

            for (size_t j = 0; j < SpherePolygons; j++, idx1++, idx2++)
            {
                if (i != 0)
                {
                    linesData.push_back(vertecies[idx1]); //-V525
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx1 + 1]);
                    linesData.push_back(vertecies[idx1 + 1]);
                    linesData.push_back(vertecies[idx1]);
                }
                if (i + 1 != SpherePolygons)
                {
                    linesData.push_back(vertecies[idx1 + 1]);
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx2 + 1]); //-V525
                    linesData.push_back(vertecies[idx2 + 1]);
                    linesData.push_back(vertecies[idx1 + 1]);
                }
            }
        }

        return linesData;
    }

    void DebugBuffer::SubmitSphere(const BoundingSphere& sphere, const Vector4 color)
    {
        static const auto sphereData = InitializeSphere();

        for (const Vector3& v : sphereData)
        {
            this->storage.push_back({ sphere.GetRedius() * v * RootThree<float>() + sphere.Center, color });
        }
    }

    void DebugBuffer::ClearBuffer()
    {
        this->storage.clear();
    }

    void DebugBuffer::SubmitBuffer()
    {
        size_t size = this->GetSize() * sizeof(Point) / sizeof(float);
        if (size <= this->VBO->GetSize())
            this->VBO->BufferSubData((float*)this->storage.data(), size);
        else
            this->VBO->Load((float*)this->storage.data(), size, UsageType::DYNAMIC_DRAW);
    }

    size_t DebugBuffer::GetSize() const
    {
        return this->storage.size();
    }

    GResource<VertexArray> DebugBuffer::GetVAO() const
    {
        return this->VAO;
    }
}