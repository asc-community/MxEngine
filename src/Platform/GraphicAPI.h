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

#pragma once

#include "Utilities/Factory/Factory.h"

#include "Platform/OpenGL/CubeMap.h"
#include "Platform/OpenGL/FrameBuffer.h"
#include "Platform/OpenGL/IndexBuffer.h"
#include "Platform/OpenGL/RenderBuffer.h"
#include "Platform/OpenGL/Shader.h"
#include "Platform/OpenGL/Texture.h"
#include "Platform/OpenGL/VertexArray.h"
#include "Platform/OpenGL/VertexBuffer.h"
#include "Platform/OpenGL/ShaderStorageBuffer.h"
#include "Platform/OpenGL/ComputeShader.h"
#include "Platform/OpenGL/VertexLayout.h"

namespace MxEngine
{
    MXENGINE_MAKE_FACTORY(CubeMap);
    MXENGINE_MAKE_FACTORY(FrameBuffer);
    MXENGINE_MAKE_FACTORY(IndexBuffer);
    MXENGINE_MAKE_FACTORY(RenderBuffer);
    MXENGINE_MAKE_FACTORY(Shader);
    MXENGINE_MAKE_FACTORY(Texture);
    MXENGINE_MAKE_FACTORY(VertexArray);
    MXENGINE_MAKE_FACTORY(VertexBuffer);
    MXENGINE_MAKE_FACTORY(ShaderStorageBuffer);
    MXENGINE_MAKE_FACTORY(ComputeShader);

    #undef MAKE_FACTORY

    inline auto GetAttachedTexture(const FrameBuffer& framebuffer)
    {
        return framebuffer.GetAttachedTexture<Resource, TextureFactory>();
    }

    inline auto GetAttachedTexture(const FrameBufferHandle& framebuffer)
    {
        return GetAttachedTexture(*framebuffer);
    }

    inline auto GetAttachedCubeMap(const FrameBuffer& framebuffer)
    {
        return framebuffer.GetAttachedCubeMap<Resource, CubeMapFactory>();
    }

    inline auto GetAttachedCubeMap(const FrameBufferHandle& framebuffer)
    {
        return GetAttachedCubeMap(*framebuffer);
    }
}