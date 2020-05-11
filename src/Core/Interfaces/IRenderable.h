// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#pragma once

#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/VertexBuffer.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Material/Material.h"
#include "Core/Components/Transform/Transform.h"

namespace MxEngine
{
	struct IRenderable
	{
		virtual const VertexArray& GetVAO() const = 0;
		virtual const IndexBuffer& GetIBO() const = 0;
		virtual const IndexBuffer& GetMeshIBO() const = 0;
		virtual const Material& GetMaterial() const = 0;
		virtual bool HasMaterial() const = 0;
		virtual size_t GetVertexBufferSize() const = 0;
		virtual const Transform& GetTransform() const = 0;
		virtual const Vector4& GetRenderColor() const = 0;

		virtual ~IRenderable() = default;
	};
}