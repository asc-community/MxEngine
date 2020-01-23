#pragma once

#include "Core/OpenGL/VertexArray/VertexArray.h"
#include "Core/OpenGL/Shader/Shader.h"
#include "Core/OpenGL/IndexBuffer/IndexBuffer.h"
#include "Core/OpenGL/Texture/Texture.h"
#include "Core/OpenGL/GLObject/GLObject.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"
#include "Core/Interfaces/IDrawable.h"
#include "Core/Camera/CameraController.h"

namespace chaiscript
{
	class ChaiScript;
}

namespace MomoEngine
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

	enum class WrapType
	{
		CLAMP_TO_EDGE = 0x812F,
		MIRRORED_REPEAT = 0x8370,
		REPEAT = 0x2901,
	};

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

	enum class ImguiStyle
	{
		CLASSIC,
		LIGHT,
		DARK,
	};

	class RendererImpl
	{
		bool depthBufferEnabled = false;
		unsigned int clearMask = 0;
	public:
		CameraController ViewPort;
		Ref<Shader> ObjectShader, MeshShader;
		Ref<Texture> DefaultTexture;
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
		void Draw(const IDrawable& object) const;
		void DrawObjectMesh(const IDrawable& object) const;
		void Clear() const;
		void Flush() const;
		void Finish() const;
		RendererImpl& UseSampling(bool value = true);
		RendererImpl& UseDepthBuffer(bool value = true);
		RendererImpl& UseCulling(bool value = true, bool counterClockWise = true, bool cullBack = true);
		RendererImpl& UseClearColor(float r, float g, float b, float a = 0.0f);
		RendererImpl& UseTextureMinFilter(MinFilter filter);
		RendererImpl& UseTextureMagFilter(MagFilter filter);
		RendererImpl& UseBlending(BlendFactor src, BlendFactor dist);
		RendererImpl& UseTextureWrap(WrapType textureX, WrapType textureY);
		RendererImpl& UseAnisotropicFiltering(float factor);
		RendererImpl& UseImGuiStyle(ImguiStyle style);

		float GetLargestAnisotropicFactor() const;
	};

	using Renderer = SingletonHolder<RendererImpl>;

	class ChaiScriptRenderer
	{
	public:
		static void Init(chaiscript::ChaiScript& chai);
	};
}