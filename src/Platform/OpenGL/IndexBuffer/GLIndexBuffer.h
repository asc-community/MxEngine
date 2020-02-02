#pragma once

#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"

namespace MomoEngine
{
	class GLIndexBuffer final : public IndexBuffer
	{
		size_t count = 0;
		#ifdef _DEBUG
		const IndexType* indicies = nullptr;
		#endif
	public:
		explicit GLIndexBuffer();
		explicit GLIndexBuffer(const IndexBufferType& data);
		GLIndexBuffer(const GLIndexBuffer&) = delete;
		GLIndexBuffer(GLIndexBuffer&& ibo) noexcept;
		~GLIndexBuffer();

        // Inherited via IIndexBuffer
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void Load(const IndexBufferType& data) override;
        virtual size_t GetCount() const override;
        virtual size_t GetIndexTypeId() const override;
    };
}