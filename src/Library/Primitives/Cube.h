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
#include <array>

namespace MxEngine
{
    class Cube : public AbstractPrimitive
    {
    public:
        inline Cube()
        {
            static std::array vertex =
            {
                Vector3(0.0f, 0.0f, 0.0f),
                Vector3(0.0f, 0.0f, 1.0f),
                Vector3(0.0f, 1.0f, 0.0f),
                Vector3(0.0f, 1.0f, 1.0f),
                Vector3(1.0f, 0.0f, 0.0f),
                Vector3(1.0f, 0.0f, 1.0f),
                Vector3(1.0f, 1.0f, 0.0f),
                Vector3(1.0f, 1.0f, 1.0f),
            };
            static std::array texture =
            {
                Vector2(0.0f, 0.0f),
                Vector2(0.0f, 1.0f),
                Vector2(1.0f, 0.0f),
                Vector2(1.0f, 1.0f),
            };
            static std::array normal =
            {
                Vector3( 0.0f,  0.0f,  1.0f),
                Vector3( 0.0f,  0.0f, -1.0f),
                Vector3( 1.0f,  0.0f,  0.0f),
                Vector3(-1.0f,  0.0f,  0.0f),
                Vector3( 0.0f, -1.0f,  0.0f),
                Vector3( 0.0f,  1.0f,  0.0f),
            };
            static std::array face =
            {
                 VectorInt3(2, 1, 1), VectorInt3(6, 3, 1), VectorInt3(8, 4, 1),
                 VectorInt3(8, 4, 1), VectorInt3(4, 2, 1), VectorInt3(2, 1, 1),
                 VectorInt3(5, 3, 2), VectorInt3(1, 1, 2), VectorInt3(3, 2, 2),
                 VectorInt3(3, 2, 2), VectorInt3(7, 4, 2), VectorInt3(5, 3, 2),
                 VectorInt3(5, 1, 3), VectorInt3(7, 2, 3), VectorInt3(8, 4, 3),
                 VectorInt3(8, 4, 3), VectorInt3(6, 3, 3), VectorInt3(5, 1, 3),
                 VectorInt3(3, 1, 4), VectorInt3(1, 2, 4), VectorInt3(2, 4, 4),
                 VectorInt3(2, 4, 4), VectorInt3(4, 3, 4), VectorInt3(3, 1, 4),
                 VectorInt3(1, 1, 5), VectorInt3(5, 3, 5), VectorInt3(6, 4, 5),
                 VectorInt3(6, 4, 5), VectorInt3(2, 2, 5), VectorInt3(1, 1, 5),
                 VectorInt3(7, 1, 6), VectorInt3(3, 3, 6), VectorInt3(4, 4, 6),
                 VectorInt3(4, 4, 6), VectorInt3(8, 2, 6), VectorInt3(7, 1, 6),
            };
            static VertexBuffer::BufferData data;

            INVOKE_ONCE(
                auto center = FindCenter(vertex.data(), vertex.size());
                for (auto& v : vertex) v -= center;

                data.reserve(face.size() * AbstractPrimitive::VertexSize);
                for (auto& f : face)
                {
                    f -= VectorInt3(1);

                    const Vector3& v = vertex[f.x];
                    data.push_back(v.x);
                    data.push_back(v.y);
                    data.push_back(v.z);

                    const Vector2& vt = texture[f.y];
                    data.push_back(vt.x);
                    data.push_back(vt.y);

                    const Vector3& vn = normal[f.z];
                    data.push_back(vn.x);
                    data.push_back(vn.y);
                    data.push_back(vn.z);
                }
            );
            this->SubmitData(data);
        }
    };
}