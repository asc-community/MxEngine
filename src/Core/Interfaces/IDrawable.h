#pragma once

#include "Core/OpenGL/VertexArray/VertexArray.h"
#include "Core/OpenGL/Texture/Texture.h"
#include "Core/OpenGL/Shader/Shader.h"
#include "Core/OpenGL/IndexBuffer/IndexBuffer.h"
#include <vector>

namespace MomoEngine
{
	struct IRenderable
	{
		virtual const VertexArray& GetVAO() const = 0;
		virtual const IndexBuffer& GetIBO() const = 0;
		virtual const Texture& GetTexture() const = 0;
		virtual size_t GetVertexCount() const = 0;
		virtual bool HasTexture() const = 0;

		virtual ~IRenderable() = default;
	};

	struct IDrawable
	{
		virtual size_t GetIterator() const = 0;
		virtual bool IsLast(size_t iterator) const = 0;
		virtual size_t GetNext(size_t iterator) const = 0;
		virtual const IRenderable& GetCurrent(size_t iterator) const = 0;
		virtual const glm::mat4x4& GetModel() const = 0;
		virtual const Shader& GetShader() const = 0;
		virtual bool HasShader() const = 0;

		virtual ~IDrawable() = default;
	};
}