#pragma once

#include "Core/OpenGL/VertexArray/VertexArray.h"
#include "Core/OpenGL/Shader/Shader.h"
#include "Core/OpenGL/IndexBuffer/IndexBuffer.h"
#include "Core/OpenGL/Texture/Texture.h"
#include "Core/OpenGL/GLObject/GLObject.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"

namespace MomoEngine
{
	enum class MinFilter
	{
		NEAREST_MIPMAP_NEAREST = 0x2700,
		LINEAR_MIPMAP_NEAREST = 0x2701,
		NEAREST_MIPMAP_LINEAR = 0x2702,
		LINEAR_MIPMAP_LINEAR = 0x2703,
	};

	enum class MagFilter
	{
		NEAREST = 0x2600,
		LINEAR = 0x2601,
	};

	class RendererImpl
	{
		bool depthBufferEnabled = false;
		unsigned int clearMask = 0;
	public:
		RendererImpl();
		RendererImpl(const RendererImpl&) = delete;
		RendererImpl(RendererImpl&&) = delete;
		void DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
		void DrawTriangles(const VertexArray& vao, size_t vertexCount, const Shader& shader) const;
		void DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const;
		void DrawTrianglesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const;
		void DrawLines(const VertexArray& vao, size_t vertexCount, const Shader& shader) const;
		void DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
		void DrawLinesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const;
		void DrawLinesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const;
		void Clear() const;
		void Flush() const;
		void Finish() const;
		RendererImpl& UseSampling(bool value = true);
		RendererImpl& UseDepthBuffer(bool value = true);
		RendererImpl& UseCulling(bool value = true, bool counterClockWise = true, bool cullBack = true);
		RendererImpl& UseClearColor(float r, float g, float b, float a = 0.0f);
		RendererImpl& UseTextureMinFilter(MinFilter filter);
		RendererImpl& UseTextureMagFilter(MagFilter filter);
	};

	using Renderer = SingletonHolder<RendererImpl>;
}