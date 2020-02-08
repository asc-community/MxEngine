#pragma once

#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/VertexBuffer.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Material/Material.h"

namespace MxEngine
{
	struct IRenderable
	{
		virtual const VertexArray& GetVAO() const = 0;
		virtual const IndexBuffer& GetMeshIBO() const = 0;
		virtual const Material& GetMaterial() const = 0;
		virtual bool HasMaterial() const = 0;
		virtual size_t GetVertexCount() const = 0;

		virtual ~IRenderable() = default;
	};
}