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

#include "Platform/OpenGL/Texture.h"
#include "Platform/OpenGL/CubeMap.h"

namespace MxEngine
{
    enum class Attachment
    {
        COLOR_ATTACHMENT0,
        COLOR_ATTACHMENT1,
        COLOR_ATTACHMENT2,
        COLOR_ATTACHMENT3,
        COLOR_ATTACHMENT4,
        COLOR_ATTACHMENT5,
        COLOR_ATTACHMENT6,
        COLOR_ATTACHMENT9,
        COLOR_ATTACHMENT10,
        COLOR_ATTACHMENT11,
        COLOR_ATTACHMENT12,
        COLOR_ATTACHMENT13,
        COLOR_ATTACHMENT14,
        COLOR_ATTACHMENT15,
        DEPTH_ATTACHMENT,
        STENCIL_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT,
    };

    class FrameBuffer
    {
        using BindableId = unsigned int;

        BindableId id = 0;
        int width = 0, height = 0;
        Texture texture;
        CubeMap cubemap;
    public:
        FrameBuffer();
        ~FrameBuffer();
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer(FrameBuffer&&) noexcept;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        FrameBuffer& operator=(FrameBuffer&&) noexcept;

        void AttachTexture(Attachment attachment, int width, int height);
        void AttachTexture(const Texture& texture, Attachment attachment);
        void AttachTexture(Texture&& texture, Attachment attachment);
        void AttachCubeMap(Attachment attachment, int width, int height);
        void AttachCubeMap(const CubeMap& cubemap, Attachment attachment);
        void AttachCubeMap(CubeMap&& cubemap, Attachment attachment);
        void CopyFrameBufferContents(const FrameBuffer& framebuffer) const;
        void CopyFrameBufferContents(int screenWidth, int screenHeight) const;
        void Validate() const;
        Texture& GetAttachedTexture();
        const Texture& GetAttachedTexture() const;
        CubeMap& GetAttachedCubeMap();
        const CubeMap& GetAttachedCubeMap() const;
        void UseDrawBuffers(size_t count) const;
        int GetWidth() const;
        int GetHeight() const;
        void Bind() const;
        void Unbind() const;
        BindableId GetNativeHandle() const;
    };
}