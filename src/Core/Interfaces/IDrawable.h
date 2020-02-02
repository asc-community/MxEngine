#pragma once

#include "Core/Interfaces/GraphicAPI/Texture.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Material/Material.h"
#include "Core/Interfaces/IRenderable.h"
#include "Utilities/Math/Math.h"

#include <vector>

namespace MomoEngine
{
	struct IDrawable
	{
		virtual size_t GetIterator() const = 0;
		virtual bool IsLast(size_t iterator) const = 0;
		virtual size_t GetNext(size_t iterator) const = 0;
		virtual const IRenderable& GetCurrent(size_t iterator) const = 0;
		virtual const Matrix4x4& GetModel() const = 0;
		virtual const Shader& GetShader() const = 0;
		virtual const Texture& GetTexture() const = 0;
		virtual bool HasShader() const = 0;
		virtual bool IsDrawable() const = 0;
		virtual bool HasTexture() const = 0;
		virtual size_t GetInstanceCount() const = 0;

		virtual ~IDrawable() = default;
	};
}