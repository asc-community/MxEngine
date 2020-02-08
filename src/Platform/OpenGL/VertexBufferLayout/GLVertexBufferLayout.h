#pragma once

#include "Core/Interfaces/GraphicAPI/VertexBufferLayout.h"

#ifdef _DEBUG
#include <string>
#endif // _DEBUG

namespace MxEngine
{
	class GLVertexBufferLayout final : public VertexBufferLayout
	{
		#ifdef _DEBUG
		std::string layoutString;
		#endif

		ElementBuffer elements;
		StrideType stride = 0;
	public:
		GLVertexBufferLayout() = default;

        // Inherited via IVertexBufferLayout
        virtual const ElementBuffer& GetElements() const override;
        virtual StrideType GetStride() const override;
        virtual void PushFloat(size_t count) override;
    };
}