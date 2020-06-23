// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "VertexBufferLayout.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Core/Macro/Macro.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
	const VertexBufferLayout::ElementBuffer& VertexBufferLayout::GetElements() const
	{
		return elements;
	}

	VertexBufferLayout::StrideType VertexBufferLayout::GetStride() const
	{
		return stride;
	}

	void VertexBufferLayout::PushFloat(size_t count)
	{
		#if defined(MXENGINE_DEBUG)
		this->layoutString += TypeToString<float>() + ToMxString(count) + ", ";
		#endif
		this->elements.push_back({ (unsigned int)count, GetGLType<float>(), false });
		this->stride += unsigned int(sizeof(float) * count);
	}

	void VertexBufferLayout::PopFloat(size_t count)
	{
		MX_ASSERT(!this->elements.empty());
		#if defined(MXENGINE_DEBUG)
		auto str = TypeToString<float>() + ToMxString(count) + ", ";
		this->layoutString.erase(this->layoutString.end() - str.size() - 1, this->layoutString.end());
		#endif
		this->elements.pop_back();
		this->stride -= unsigned int(sizeof(float) * count);
	}

	template<>
	void VertexBufferLayout::Push<Vector2>()
	{
		this->PushFloat(2);
	}

	template<>
	void VertexBufferLayout::Push<Vector3>()
	{
		this->PushFloat(3);
	}

	template<>
	void VertexBufferLayout::Push<Vector4>()
	{
		this->PushFloat(4);
	}

	template<>
	void VertexBufferLayout::Push<Matrix3x3>()
	{
		this->PushFloat(3);
		this->PushFloat(3);
		this->PushFloat(3);
	}

	template<>
	void VertexBufferLayout::Push<Matrix4x4>()
	{
		this->PushFloat(4);
		this->PushFloat(4);
		this->PushFloat(4);
		this->PushFloat(4);
	}

	template<>
	void VertexBufferLayout::Pop<Vector2>()
	{
		this->PopFloat(2);
	}

	template<>
	void VertexBufferLayout::Pop<Vector3>()
	{
		this->PopFloat(3);
	}

	template<>
	void VertexBufferLayout::Pop<Vector4>()
	{
		this->PopFloat(4);
	}

	template<>
	void VertexBufferLayout::Pop<Matrix3x3>()
	{
		this->PopFloat(3);
		this->PopFloat(3);
		this->PopFloat(3);
	}

	template<>
	void VertexBufferLayout::Pop<Matrix4x4>()
	{
		this->PopFloat(4);
		this->PopFloat(4);
		this->PopFloat(4);
		this->PopFloat(4);
	}
}