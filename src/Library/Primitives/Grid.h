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
#include "Core/Macro/Macro.h"
#include "Core/Application/Application.h"
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
            this->SubmitData(aabb, make_view(data));
        }
    private:
        inline static MeshData GetGridData(size_t size)
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
                Vector2(       0.0f,        0.0f),
                Vector2(       0.0f, size * 1.0f),
                Vector2(size * 1.0f,        0.0f),
                Vector2(size * 1.0f, size * 1.0f),
            };
            constexpr std::array face =
            {
                VectorInt2(0, 0), VectorInt2(1, 1), VectorInt2(2, 2),
                VectorInt2(2, 2), VectorInt2(1, 1), VectorInt2(3, 3),
                VectorInt2(0, 0), VectorInt2(2, 2), VectorInt2(1, 1),
                VectorInt2(1, 1), VectorInt2(2, 2), VectorInt2(3, 3),
            };

            indicies.resize(face.size());
            vertecies.resize(face.size());
            for (size_t i = 0; i < face.size(); i++)
            {
                indicies[i] = static_cast<uint32_t>(i);

                vertecies[i].Position = vertex[face[i].x];
                vertecies[i].TexCoord = texture[face[i].y];
            }
            meshData.RegenerateNormals();
            return meshData;
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

        inline static GResource<Texture> GetGridTexture()
        {
            GResource<Texture> result;

            constexpr size_t size = 512;
            std::vector<uint8_t> data;
            data.resize(size * size * 3, 255);
            DrawBorder(data.data(), size, 6);
            
            result = GraphicFactory::Create<Texture>();
            result->Load(data.data(), size, size);
            return result;
        }
    };
}