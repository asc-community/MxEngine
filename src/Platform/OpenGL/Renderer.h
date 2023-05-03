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

#include "Platform/GraphicAPI.h"

namespace MxEngine
{
    enum class BlendFactor
    {
        NONE = 0,
        ZERO,
        ONE,
        ONE_MINUS_SRC_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA,
    };

    enum class DepthFunction
    {
        EQUAL = 0,
        NOT_EQUAL,
        LESS,
        GREATER,
        LESS_EQUAL,
        GREATER_EQUAL,
        ALWAYS,
        NEVER,
    };

    enum class RenderPrimitive
    {
        POINTS = 0, 
        LINE_STRIP, 
        LINE_LOOP, 
        LINES, 
        LINE_STRIP_ADJACENCY, 
        LINES_ADJACENCY, 
        TRIANGLE_STRIP, 
        TRIANGLE_FAN, 
        TRIANGLES, 
        TRIANGLE_STRIP_ADJACENCY, 
        TRIANGLES_ADJACENCY,
        PATCHES,
    };

    class Renderer
    {
        bool depthBufferEnabled = false;
        unsigned int clearMask = 0;
    public:
        Renderer();

        void DrawVertices(RenderPrimitive primitive, size_t vertexCount, size_t vertexOffset);
        void DrawIndices(RenderPrimitive primitive, size_t indexCount, size_t indexOffset);
        void DrawVerticesInstanced(RenderPrimitive primitive, size_t vertexCount, size_t vertexOffset, size_t instanceCount, size_t baseInstance);
        void DrawIndicesInstanced(RenderPrimitive primitive, size_t indexCount, size_t indexOffset, size_t instanceCount, size_t baseInstance);
        void DrawIndicesBaseVertex(RenderPrimitive primitive, size_t indexCount, size_t indexOffset, size_t baseVertex);
        void DrawIndicesBaseVertexInstanced(RenderPrimitive primitive, size_t indexCount, size_t indexOffset, size_t baseVertex, size_t instanceCount, size_t baseInstance);

        void SetDefaultVertexAttribute(size_t index, float v) const;
        void SetDefaultVertexAttribute(size_t index, const Vector2& vec) const;
        void SetDefaultVertexAttribute(size_t index, const Vector3& vec) const;
        void SetDefaultVertexAttribute(size_t index, const Vector4& vec) const;
        void SetDefaultVertexAttribute(size_t index, const Matrix4x4& mat) const;
        void SetDefaultVertexAttribute(size_t index, const Matrix3x3& mat) const;
        void Clear() const;
        void Flush() const;
        void Finish() const;
        void SetViewport(int x, int y, int width, int height) const;
        void GetViewport(int& x, int& y, int& width, int& height) const;
        Renderer& UseClipDistance(size_t count);
        Renderer& UseSeamlessCubeMaps(bool value = true);
        Renderer& UseColorMask(bool r, bool g, bool b, bool a);
        Renderer& UseDepthBufferMask(bool value = true);
        Renderer& UseSampling(bool value = true);
        Renderer& UseDepthBuffer(bool value = true);
        Renderer& UseReversedDepth(bool value = true);
        Renderer& UseDepthFunction(DepthFunction function);
        Renderer& UseCulling(bool value = true, bool counterClockWise = true, bool cullBack = true);
        Renderer& UseClearColor(float r, float g, float b, float a = 0.0f);
        Renderer& UseBlendFactors(BlendFactor src, BlendFactor dist);
        Renderer& UseAnisotropicFiltering(float factor);
        float GetLargestAnisotropicFactor() const;
    };
}