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

#include "Core/Interfaces/GraphicAPI/GraphicModule.h"
#include "Core/Interfaces/GraphicAPI/Window.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/Texture.h"
#include "Core/Interfaces/GraphicAPI/CubeMap.h"
#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/VertexBuffer.h"
#include "Core/Interfaces/GraphicAPI/VertexBufferLayout.h"
#include "Core/Interfaces/GraphicAPI/FrameBuffer.h"
#include "Core/Interfaces/GraphicAPI/RenderBuffer.h"
#include "Core/Interfaces/GraphicAPI/Renderer.h"

#include "Utilities/Memory/Memory.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"
#include "Utilities/FileSystem/FileSystem.h"

#include <filesystem>

namespace MxEngine
{
	struct GraphicFactory
	{
		virtual GraphicModule& GetGraphicModule() = 0;
		virtual Renderer& GetRenderer() = 0;

		virtual UniqueRef<Window> CreateWindow() = 0;
		virtual UniqueRef<IndexBuffer> CreateIndexBuffer() = 0;
		virtual UniqueRef<Shader> CreateShader() = 0;
		virtual UniqueRef<Texture> CreateTexture() = 0;
		virtual UniqueRef<CubeMap> CreateCubeMap() = 0;
		virtual UniqueRef<VertexArray> CreateVertexArray() = 0;
		virtual UniqueRef<VertexBuffer> CreateVertexBuffer() = 0;
		virtual UniqueRef<VertexBufferLayout> CreateVertexBufferLayout() = 0;
		virtual UniqueRef<FrameBuffer> CreateFrameBuffer() = 0;
		virtual UniqueRef<RenderBuffer> CreateRenderBuffer() = 0;

		virtual UniqueRef<Window> CreateWindow(int width, int height, const std::string& name) = 0;
		virtual UniqueRef<IndexBuffer> CreateIndexBuffer(IndexBuffer::IndexData data, size_t count) = 0;
		virtual UniqueRef<Shader> CreateShader(const FilePath& vertex, const FilePath& fragment) = 0;
		virtual UniqueRef<Shader> CreateShader(const FilePath& vertex, const FilePath& geometry, const FilePath& fragment) = 0;
		virtual UniqueRef<Texture> CreateTexture(const FilePath& texture, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true, bool flipImage = true) = 0;
		virtual UniqueRef<CubeMap> CreateCubeMap(const FilePath& cubemap, bool genMipMaps = true, bool flipImage = true) = 0;
		virtual UniqueRef<VertexBuffer> CreateVertexBuffer(VertexBuffer::BufferData data, size_t count, UsageType type) = 0;

		virtual ~GraphicFactory() = default;
	};

	using Graphics = SingletonHolder<GraphicFactory*>;
}