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

#include "Utilities/Array/ArrayView.h"
#include "Core/Resources/AssetManager.h"
#include "Utilities/Array/Array2D.h"

namespace MxEngine
{
    class Primitives
    {
        template<typename Func>
        static Array2D<float> GenerateHeights(Func&& f, float xsize, float ysize, float step)
        {
            static_assert(std::is_same<float, decltype(f(0.0f, 0.0f))>::value, "Func must accept two floats and output one float");
            MX_ASSERT(step > 0.0f);
            MX_ASSERT(xsize > 0.0f);
            MX_ASSERT(ysize > 0.0f);

            Array2D<float> heights;
            size_t intxsize = static_cast<size_t>(xsize / step) + 1;
            size_t intysize = static_cast<size_t>(ysize / step) + 1;
            heights.resize(intxsize, intysize);

            for (size_t x = 0; x < intxsize; x++)
            {
                for (size_t y = 0; y < intysize; y++)
                {
                    float fx = x * step;
                    float fy = y * step;
                    heights[x][y] = f(fx, fy);
                }
            }
            return heights;
        }

    public:
        static MeshHandle CreateMesh(const MeshData::VertexData& vertecies, const MeshData::IndexData& indicies, const MxString& filename);
        static MeshHandle CreateMesh(const MeshData::VertexData& vertecies, const MeshData::IndexData& indicies);
        static MeshHandle CreateCube(size_t polygons = 2);
        static MeshHandle CreatePlane(size_t polygons = 2);
        static MeshHandle CreatePlane2Side(size_t polygons = 2);
        static MeshHandle CreateSphere(size_t polygons = 32);
        static MeshHandle CreateCylinder(size_t polygons = 32);
        static MeshHandle CreatePyramid(size_t polygons = 1);
        static MeshHandle CreateSurface(const Array2D<float>& heights);
        static MeshHandle CreateSurface2Side(const Array2D<float>& heights);
        static MeshHandle CreateSurface(const Array2D<float>& heights, const MxString& filename);
        static MeshHandle CreateSurface2Side(const Array2D<float>& heights, const MxString& filename);
        // static TextureHandle CreateGridTexture(size_t textureSize = 512, float borderScale = 0.01f);

        template<typename Func>
        static MeshHandle CreateSurface(Func&& f, float xsize, float ysize, float step)
        {
            return Primitives::CreateSurface(std::forward<Func>(f), xsize, ysize, step, UUIDGenerator::Get());
        }

        template<typename Func>
        static MeshHandle CreateSurface2Side(Func&& f, float xsize, float ysize, float step)
        {
            return Primitives::CreateSurface2Side(std::forward<Func>(f), xsize, ysize, step, UUIDGenerator::Get());
        }

        template<typename Func>
        static MeshHandle CreateSurface(Func&& f, float xsize, float ysize, float step, const MxString& name)
        {
            Array2D<float> heights = Primitives::GenerateHeights(std::forward<Func>(f), xsize, ysize, step);
            return Primitives::CreateSurface(heights, name);
        }

        template<typename Func>
        static MeshHandle CreateSurface2Side(Func&& f, float xsize, float ysize, float step, const MxString& name)
        {
            Array2D<float> heights = Primitives::GenerateHeights(std::forward<Func>(f), xsize, ysize, step);
            return Primitives::CreateSurface2Side(heights, name);
        }
    };
}