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

#include "VertexAttribute.h"
#include "GLUtilities.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
    template<>
    VertexAttribute VertexAttribute::Entry<float>()
    {
        return { GL_FLOAT, 1, 1, sizeof(float) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<Vector2>()
    {
        return { GL_FLOAT, 2, 1, sizeof(Vector2) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<Vector3>()
    {
        return { GL_FLOAT, 3, 1, sizeof(Vector3) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<Vector4>()
    {
        return { GL_FLOAT, 4, 1, sizeof(Vector4) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<int32_t>()
    {
        return { GL_INT, 1, 1, sizeof(int32_t) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<VectorInt2>()
    {
        return { GL_INT, 2, 1, sizeof(VectorInt2) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<VectorInt3>()
    {
        return { GL_INT, 3, 1, sizeof(VectorInt3) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<VectorInt4>()
    {
        return { GL_INT, 4, 1, sizeof(VectorInt4) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<Matrix2x2>()
    {
        return { GL_FLOAT, 2, 2, sizeof(Matrix2x2) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<Matrix3x3>()
    {
        return { GL_FLOAT, 3, 3, sizeof(Matrix3x3) };
    }

    template<>
    VertexAttribute VertexAttribute::Entry<Matrix4x4>()
    {
        return { GL_FLOAT, 4, 4, sizeof(Matrix4x4) };
    }
}