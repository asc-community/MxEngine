#pragma once

#include "Core/Interfaces/GraphicAPI/VertexBuffer.h"

namespace MxEngine
{
	class GLVertexBuffer final : public VertexBuffer
	{
	public:
		explicit GLVertexBuffer();
		explicit GLVertexBuffer(BufferData& data, UsageType type);
		~GLVertexBuffer();
		GLVertexBuffer(const GLVertexBuffer&) = delete;
		GLVertexBuffer(GLVertexBuffer&& vbo);

        // Inherited via IVertexBuffer
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void Load(const BufferData& data, UsageType type) override;
    };
}