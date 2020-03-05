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
#include "Core/Event/AppDestroyEvent.h"
#include <array>

namespace MxEngine
{
    class Grid : public AbstractPrimitive
    {
        inline static size_t lastSize = 0;
        inline static std::vector<float> Data;
    public:
        inline Grid(size_t size = 2000)
        {
            if (lastSize != size)
            {
                this->UpdateDataBuffer(size);
                lastSize = size;
            }
            this->SubmitData(Data);
            this->Texture = GetGridTexture();
        }

        inline void Resize(size_t size)
        {
            this->UpdateDataBuffer(size);
            lastSize = size;
            this->SubmitData(Data);
        }

        inline static void UpdateDataBuffer(size_t size)
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
            std::array normal =
            {
                Vector3(0.0f, 1.0f, 0.0f)
            };
            std::array face =
            {
                VectorInt3(0, 0, 0), VectorInt3(1, 1, 0), VectorInt3(2, 2, 0),
                VectorInt3(2, 2, 0), VectorInt3(1, 1, 0), VectorInt3(3, 3, 0),
                VectorInt3(0, 0, 0), VectorInt3(2, 2, 0), VectorInt3(1, 1, 0),
                VectorInt3(1, 1, 0), VectorInt3(2, 2, 0), VectorInt3(3, 3, 0),
            };
            Data.clear();
            auto center = FindCenter(vertex.data(), vertex.size());
            for (auto& v : vertex) v -= center;

            for (auto& f : face)
            {
                const Vector3& v = vertex[f.x];
                Data.push_back(v.x);
                Data.push_back(v.y);
                Data.push_back(v.z);

                const Vector2& vt = texture[f.y];
                Data.push_back(vt.x);
                Data.push_back(vt.y);

                const Vector3& vn = normal[f.z];
                Data.push_back(vn.x);
                Data.push_back(vn.y);
                Data.push_back(vn.z);
            }
        }

        inline static Ref<MxEngine::Texture> GetGridTexture()
        {
            static Ref<MxEngine::Texture> texture;
            INVOKE_ONCE(
                const size_t size = 512;
                const size_t border = 5;
                std::vector<uint8_t> data(size * size * 3);
                for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        if (i < border || i + border > size || 
                            j < border || j + border > size)
                        {
                            data[(i * size + j) * 3 + 0] = 0;
                            data[(i * size + j) * 3 + 1] = 0;
                            data[(i * size + j) * 3 + 2] = 0;
                        }
                        else
                        {
                            data[(i * size + j) * 3 + 0] = 192;
                            data[(i * size + j) * 3 + 1] = 192;
                            data[(i * size + j) * 3 + 2] = 192;
                        }
                    }
                }
                Context::Instance()->GetEventDispatcher().AddEventListener<AppDestroyEvent>("DeleteGrid",
                    [](AppDestroyEvent& e) { texture.reset(); });
                texture = Graphics::Instance()->CreateTexture();
                texture->Load(data.data(), 512, 512);
            );
            return texture;
        }
    };
}