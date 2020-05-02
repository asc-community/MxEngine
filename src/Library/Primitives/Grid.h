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
#include "Core/Macro/Macro.h"
#include "Core/Application/Application.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Core/Event/Events/AppDestroyEvent.h"
#include <array>

namespace MxEngine
{
    class Grid : public AbstractPrimitive
    {
    public:
        inline Grid(size_t size = 2000)
        {
            this->ObjectTexture = GetGridTexture();
            this->Resize(size);
        }

        inline void Resize(size_t size)
        {
            auto data = Grid::GetGridData(size);
            AABB aabb{ MakeVector3(-(float)size, -0.0001f, -(float)size), MakeVector3((float)size, 0.0001f, (float)size) };
            this->SubmitData(Format(FMT_STRING("MxGrid_{0}"), size), aabb, make_view(data.first), make_view(data.second));
        }
    private:
        inline static std::pair<std::vector<float>, std::vector<unsigned int>> GetGridData(size_t size)
        {
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
                Vector2(       0.0f,        0.0f),
                Vector2(       0.0f, size * 1.0f),
                Vector2(size * 1.0f,        0.0f),
                Vector2(size * 1.0f, size * 1.0f),
            };
            constexpr std::array normal =
            {
                Vector3(0.0f, 1.0f, 0.0f)
            };
            constexpr std::array face =
            {
                VectorInt3(0, 0, 0), VectorInt3(1, 1, 0), VectorInt3(2, 2, 0),
                VectorInt3(2, 2, 0), VectorInt3(1, 1, 0), VectorInt3(3, 3, 0),
                VectorInt3(0, 0, 0), VectorInt3(2, 2, 0), VectorInt3(1, 1, 0),
                VectorInt3(1, 1, 0), VectorInt3(2, 2, 0), VectorInt3(3, 3, 0),
            };
            constexpr size_t dataSize = face.size() * AbstractPrimitive::VertexSize;
            std::vector<float> data(dataSize);
            std::vector<unsigned int> indicies(face.size());

            for (size_t i = 0; i < face.size(); i++)
            {
                indicies[i] = (unsigned int)i;

                const Vector3& v = vertex[face[i].x];
                data[VertexSize * i + 0] = v.x;
                data[VertexSize * i + 1] = v.y;
                data[VertexSize * i + 2] = v.z;

                const Vector2& vt = texture[face[i].y];
                data[VertexSize * i + 3] = vt.x;
                data[VertexSize * i + 4] = vt.y;

                const Vector3& vn = normal[face[i].z];
                data[VertexSize * i + 5] = vn.x;
                data[VertexSize * i + 6] = vn.y;
                data[VertexSize * i + 7] = vn.z;

                size_t tanIndex = i - i % 3;
                auto tanbitan = ComputeTangentSpace(
                    vertex[face[tanIndex].x],  vertex[face[tanIndex + 1].x],  vertex[face[tanIndex + 2].x],
                    texture[face[tanIndex].y], texture[face[tanIndex + 1].y], texture[face[tanIndex + 2].y]
                );
                data[VertexSize * i + 8 ] = tanbitan[0].x;
                data[VertexSize * i + 9 ] = tanbitan[0].y;
                data[VertexSize * i + 10] = tanbitan[0].z;
                data[VertexSize * i + 11] = tanbitan[1].x;
                data[VertexSize * i + 12] = tanbitan[1].y;
                data[VertexSize * i + 13] = tanbitan[1].z;
            }
            return std::make_pair(std::move(data), std::move(indicies));
        }

        inline static void DrawBorder(uint8_t* data, size_t size, size_t borderSize)
        {
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    if (i < borderSize || i + borderSize >= size ||
                        j < borderSize || j + borderSize >= size)
                    {
                        data[(i * size + j) * 3 + 0] = 0;
                        data[(i * size + j) * 3 + 1] = 0;
                        data[(i * size + j) * 3 + 2] = 0;
                    }
                }
            }
        }

        inline static Texture* GetGridTexture()
        {
            auto& manager = Application::Get()->GetCurrentScene().GetResourceManager<Texture>();
            static std::string textureName = "MxTexGrid";
            if (!manager.Exists(textureName))
            {
                constexpr size_t size = 512;
                std::vector<uint8_t> data;
                data.resize(size * size * 3, 255);
                DrawBorder(data.data(), size, 6);
           
                auto texture = Graphics::Instance()->CreateTexture();
                texture->Load(data.data(), size, size);
                manager.Add(textureName, std::move(texture));
            }
            return manager.Get(textureName);
        }
    };
}