#pragma once

#include "Core/BaseObject/BaseObject.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"
#include "Core/Interfaces/IDrawable.h"
#include "Core/Camera/CameraController.h"
#include "Core/Interfaces/GraphicAPI/Renderer.h"

namespace MomoEngine
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