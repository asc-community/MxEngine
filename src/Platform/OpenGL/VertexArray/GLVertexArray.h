#pragma once

#include "Core/Interfaces/GraphicAPI/VertexArray.h"

namespace MomoEngine
{
	class GLVertexArray final : public VertexArray
	{
		int attributeIndex = 0;
	public:
		GLVertexArray();
		~GLVertexArray();
		GLVertexArray(const GLVertexArray&) = delete;
		GLVertexArray(GLVertexArray&& array) noexcept;
		GLVertexArray& operator=(GLVertexArray&& array) noexcept;

        // Inherited via IVertexArray
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout) override;
        virtual void AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout) override;
    };
}