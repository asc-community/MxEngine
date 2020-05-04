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

#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/Texture.h"

namespace MxEngine
{
	enum class BlendFactor
	{
		NONE,
		ONE_MINUS_SRC_COLOR = 0x0301,
		SRC_ALPHA = 0x0302,
		ONE_MINUS_SRC_ALPHA = 0x0303,
		DST_ALPHA = 0x0304,
		ONE_MINUS_DST_ALPHA = 0x0305,
		DST_COLOR = 0x0306,
		ONE_MINUS_DST_COLOR = 0x0307,
		CONSTANT_COLOR = 0x8001,
		ONE_MINUS_CONSTANT_COLOR = 0x8002,
		CONSTANT_ALPHA = 0x8003,
		ONE_MINUS_CONSTANT_ALPHA = 0x8004,
	};

	struct Renderer
	{
		virtual void DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const = 0;
		virtual void DrawTriangles(const VertexArray& vao, size_t vertexCount, const Shader& shader) const = 0;
		virtual void DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const = 0;
		virtual void DrawTrianglesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const = 0;
		virtual void DrawLines(const VertexArray& vao, size_t vertexCount, const Shader& shader) const = 0;
		virtual void DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const = 0;
		virtual void DrawLinesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const = 0;
		virtual void DrawLinesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const = 0;
		virtual void SetDefaultVertexAttribute(size_t index, float v) const = 0;
		virtual void SetDefaultVertexAttribute(size_t index, const Vector2& vec) const = 0;
		virtual void SetDefaultVertexAttribute(size_t index, const Vector3& vec) const = 0;
		virtual void SetDefaultVertexAttribute(size_t index, const Vector4& vec) const = 0;
		virtual void SetDefaultVertexAttribute(size_t index, const Matrix4x4& mat) const = 0;
		virtual void SetDefaultVertexAttribute(size_t index, const Matrix3x3& mat) const = 0;
		virtual void Clear() const = 0;
		virtual void Flush() const = 0;
		virtual void Finish() const = 0;
		virtual void SetViewport(int x, int y, int width, int height) const = 0;
		virtual Renderer& UseColorMask(bool r, bool g, bool b, bool a) = 0;
		virtual Renderer& UseSampling(bool value = true) = 0;
		virtual Renderer& UseDepthBuffer(bool value = true) = 0;
		virtual Renderer& UseReversedDepth(bool value = true) = 0;
		virtual Renderer& UseCulling(bool value = true, bool counterClockWise = true, bool cullBack = true) = 0;
		virtual Renderer& UseClearColor(float r, float g, float b, float a = 0.0f) = 0;
		virtual Renderer& UseBlending(BlendFactor src, BlendFactor dist) = 0;
		virtual Renderer& UseAnisotropicFiltering(float factor) = 0;
		virtual Renderer& UseLineWidth(size_t width) = 0;
		virtual float GetLargestAnisotropicFactor() const = 0;
	};
}