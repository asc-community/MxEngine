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

#include "Core/Interfaces/GraphicAPI/Texture.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Material/Material.h"
#include "Core/Interfaces/IRenderable.h"
#include "Utilities/Math/Math.h"

#include <vector>

namespace MxEngine
{
	struct IDrawable
	{
		virtual size_t GetIterator() const = 0;
		virtual bool IsLast(size_t iterator) const = 0;
		virtual size_t GetNext(size_t iterator) const = 0;
		virtual const IRenderable& GetCurrent(size_t iterator) const = 0;
		virtual const Transform& GetTransform() const = 0;
		virtual const Shader& GetShader() const = 0;
		virtual const Texture& GetTexture() const = 0;
		virtual const Vector4& GetRenderColor() const = 0;
		virtual bool HasShader() const = 0;
		virtual bool IsDrawable() const = 0;
		virtual bool HasTexture() const = 0;
		virtual size_t GetInstanceCount() const = 0;

		virtual ~IDrawable() = default;
	};
}