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

#include "FrameBuffer.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Logger/Logger.h"
#include "Core/Macro/Macro.h"

#include <array>

namespace MxEngine
{
    GLenum AttachmentTable[]
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT9,
        GL_COLOR_ATTACHMENT10,
        GL_COLOR_ATTACHMENT11,
        GL_COLOR_ATTACHMENT12,
        GL_COLOR_ATTACHMENT13,
        GL_COLOR_ATTACHMENT14,
        GL_COLOR_ATTACHMENT15,
        GL_DEPTH_ATTACHMENT,
        GL_STENCIL_ATTACHMENT,
        GL_DEPTH_STENCIL_ATTACHMENT,
    };

    FrameBuffer::FrameBuffer()
    {
        GLCALL(glGenFramebuffers(1, &id));
        Logger::Instance().Debug("OpenGL::FrameBuffer", "created framebuffer with id = " + ToMxString(id));
    }

    void FrameBuffer::AttachTexture(Attachment attachment, int width, int height)
    {
        this->width = width, this->height = height;
        this->cubemap = { };
        GLenum mode = AttachmentTable[int(attachment)];
        this->Bind();

        if (mode == GL_DEPTH_ATTACHMENT)
        {
            this->texture.LoadDepth(width, height);
            GLCALL(glDrawBuffer(GL_NONE));
        }
        else
        {
            this->texture.Load(nullptr, width, height);
        }
        GLint textureId = this->texture.GetNativeHandle();
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, mode, GL_TEXTURE_2D, textureId, 0));
    }

    void FrameBuffer::AttachTexture(const Texture& texture, Attachment attachment)
    {
        this->width  = (int)texture.GetWidth();
        this->height = (int)texture.GetHeight();
        GLenum mode = AttachmentTable[int(attachment)];
        GLint textureId = texture.GetNativeHandle();

        this->Bind();
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, mode, texture.GetTextureType(), textureId, 0));
        if (mode == GL_DEPTH_ATTACHMENT)
        {
            GLCALL(glDrawBuffer(GL_NONE));
        }
    }

    void FrameBuffer::AttachTexture(Texture&& texture, Attachment attachment)
    {
        this->texture = std::move(texture);
        this->AttachTexture(this->texture, attachment);
    }

    void FrameBuffer::AttachCubeMap(Attachment attachment, int width, int height)
    {
        this->width = width, this->height = height;
        this->texture = { };
        GLenum mode = AttachmentTable[int(attachment)];
        this->Bind();

        if (mode == GL_DEPTH_ATTACHMENT)
        {
            this->cubemap.LoadDepth(width, height);
            GLCALL(glDrawBuffer(GL_NONE));
        }
        else
        {
            this->cubemap.Load(std::array<uint8_t*, 6>{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }, width, height);
        }
        GLint cubemapId = this->cubemap.GetNativeHandle();
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, mode, cubemapId, 0));
    }

    void FrameBuffer::AttachCubeMap(const CubeMap& cubemap, Attachment attachment)
    {
        this->width = (int)cubemap.GetWidth();
        this->height = (int)cubemap.GetHeight();
        GLenum mode = AttachmentTable[int(attachment)];
        GLint cubemapId = cubemap.GetNativeHandle();

        this->Bind();
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, mode, cubemapId, 0));
        if (mode == GL_DEPTH_ATTACHMENT)
        {
            GLCALL(glDrawBuffer(GL_NONE));
        }
    }

    void FrameBuffer::AttachCubeMap(CubeMap&& cubemap, Attachment attachment)
    {
        this->cubemap = std::move(cubemap);
        this->AttachCubeMap(this->cubemap, attachment);
    }

    void FrameBuffer::CopyFrameBufferContents(int screenWidth, int screenHeight) const
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST));
    }

    void FrameBuffer::Validate() const
    {
        this->Bind();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Logger::Instance().Error("OpenGL::FrameBuffer", "framebuffer validation failed: incomplete");
    }

    Texture& FrameBuffer::GetAttachedTexture()
    {
        return this->texture;
    }

    CubeMap& FrameBuffer::GetAttachedCubeMap()
    {
        return this->cubemap;
    }

    const Texture& FrameBuffer::GetAttachedTexture() const
    {
        return this->texture;
    }

    const CubeMap& FrameBuffer::GetAttachedCubeMap() const
    {
        return this->cubemap;
    }

    void FrameBuffer::UseDrawBuffers(size_t count) const
    {
        this->Bind();
        GLCALL(glDrawBuffers((GLsizei)count, AttachmentTable));
    }

    int FrameBuffer::GetWidth() const
    {
        return this->width;
    }

    int FrameBuffer::GetHeight() const
    {
        return this->height;
    }

    FrameBuffer::~FrameBuffer()
    {
        GLCALL(glDeleteFramebuffers(1, &id));
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& framebuffer) noexcept
    {
        this->id = framebuffer.id;
        this->width = framebuffer.width;
        this->height = framebuffer.height;
        this->texture = std::move(framebuffer.texture);
        this->cubemap = std::move(framebuffer.cubemap);
        framebuffer.id = 0;
        framebuffer.width = 0;
        framebuffer.height = 0;
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& framebuffer) noexcept
    {
        this->id = framebuffer.id;
        this->width = framebuffer.width;
        this->height = framebuffer.height;
        this->texture = std::move(framebuffer.texture);
        this->cubemap = std::move(framebuffer.cubemap);
        framebuffer.id = 0;
        framebuffer.width = 0;
        framebuffer.height = 0;
        return *this;
    }

    void FrameBuffer::Bind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, id));
    }

    void FrameBuffer::Unbind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    FrameBuffer::BindableId FrameBuffer::GetNativeHandle() const
    {
        return id;
    }

    void FrameBuffer::CopyFrameBufferContents(const FrameBuffer& framebuffer) const
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.GetNativeHandle()));
        GLCALL(glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, framebuffer.GetWidth(), framebuffer.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST));
    }
}