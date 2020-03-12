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

#include "Core/MxObject/MxObject.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"
#include "Core/Interfaces/IDrawable.h"
#include "Core/Camera/CameraController.h"
#include "Core/Interfaces/GraphicAPI/Renderer.h"

namespace MxEngine
{
	class GLRenderer : public Renderer
	{
		bool depthBufferEnabled = false;
		unsigned int clearMask = 0;
	public:
		GLRenderer();

		void DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const override;
		void DrawTriangles(const VertexArray& vao, size_t vertexCount, const Shader& shader) const override;
		void DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const override;
		void DrawTrianglesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const override;
		void DrawLines(const VertexArray& vao, size_t vertexCount, const Shader& shader) const override;
		void DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const override;
		void DrawLinesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const override;
		void DrawLinesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const override;
		virtual void SetDefaultVertexAttribute(size_t index, float v) const override;
		virtual void SetDefaultVertexAttribute(size_t index, const Vector2& vec) const override;
		virtual void SetDefaultVertexAttribute(size_t index, const Vector3& vec) const override;
		virtual void SetDefaultVertexAttribute(size_t index, const Vector4& vec) const override;
		virtual void SetDefaultVertexAttribute(size_t index, const Matrix4x4& mat) const override;
		void Clear() const override;
		void Flush() const override;
		void Finish() const override;
		GLRenderer& UseSampling(bool value = true) override;
		GLRenderer& UseDepthBuffer(bool value = true) override;
		GLRenderer& UseCulling(bool value = true, bool counterClockWise = true, bool cullBack = true) override;
		GLRenderer& UseClearColor(float r, float g, float b, float a = 0.0f) override;
		GLRenderer& UseTextureMinFilter(MinFilter filter) override;
		GLRenderer& UseTextureMagFilter(MagFilter filter) override;
		GLRenderer& UseBlending(BlendFactor src, BlendFactor dist) override;
		GLRenderer& UseTextureWrap(WrapType textureX, WrapType textureY) override;
		GLRenderer& UseAnisotropicFiltering(float factor) override;
		float GetLargestAnisotropicFactor() const override;
	};
}