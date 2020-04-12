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
#include "Utilities/Format/Format.h"
#include "Core/Application/Application.h"
#include <array>

namespace MxEngine
{
    class Cube : public AbstractPrimitive
    {
    public:

        inline Cube()
        {
            auto resourceName = Format(FMT_STRING("MxCube_{0}"), Application::Get()->GenerateResourceId());
            auto data = Cube::GetCubeData();
            this->SubmitData(resourceName, data);
        }

    private:
        static ArrayView<float> GetCubeData()
        {
            constexpr std::array vertex =
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
            constexpr std::array texture =
            {
                Vector2(0.0f, 0.0f),
                Vector2(0.0f, 1.0f),
                Vector2(1.0f, 0.0f),
                Vector2(1.0f, 1.0f),
            };
            constexpr std::array normal =
            {
                Vector3(0.0f,  0.0f,  1.0f),
                Vector3(0.0f,  0.0f, -1.0f),
                Vector3(1.0f,  0.0f,  0.0f),
                Vector3(-1.0f,  0.0f,  0.0f),
                Vector3(0.0f, -1.0f,  0.0f),
                Vector3(0.0f,  1.0f,  0.0f),
            };
            constexpr std::array face =
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
            constexpr size_t dataSize = face.size() * AbstractPrimitive::VertexSize;
            static std::array<float, dataSize> data; // data MUST be static as its view is returned
            INVOKE_ONCE(
                for (size_t i = 0; i < face.size(); i++)
                {
                    const Vector3& v = vertex[face[i].x];
                    data[8 * i + 0] = v.x;
                    data[8 * i + 1] = v.y;
                    data[8 * i + 2] = v.z;

                    const Vector2& vt = texture[face[i].y];
                    data[8 * i + 3] = vt.x;
                    data[8 * i + 4] = vt.y;

                    const Vector3& vn = normal[face[i].z];
                    data[8 * i + 5] = vn.x;
                    data[8 * i + 6] = vn.y;
                    data[8 * i + 7] = vn.z;
                }
            );
            return ArrayView<float>(data);
        }
    };
}