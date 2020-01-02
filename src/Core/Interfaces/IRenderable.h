#pragma once

#include "Core/OpenGL/VertexArray/VertexArray.h"
#include "Core/OpenGL/VertexBuffer/VertexBuffer.h"
#include "Core/OpenGL/IndexBuffer/IndexBuffer.h"
#include "Core/Material/Material.h"

namespace MomoEngine
{
	struct IRenderable
	{
		virtual const VertexArray& GetVAO() const = 0;
		virtual const IndexBuffer& GetIBO() const = 0;
		virtual const Material& GetMaterial() const = 0;
		virtual bool HasMaterial() const = 0;
		virtual size_t GetVertexCount() const = 0;

		virtual ~IRenderable() = default;
	};
}