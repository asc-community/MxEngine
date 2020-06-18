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
#include "Platform/GraphicAPI.h"

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

    void FrameBuffer::OnTextureAttach(const Texture& texture, Attachment attachment)
    {
        GLenum mode = AttachmentTable[int(attachment)];
        GLint textureId = texture.GetNativeHandle();

        this->Bind();
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, mode, texture.GetTextureType(), textureId, 0));
        if (mode == GL_DEPTH_ATTACHMENT)
        {
            GLCALL(glDrawBuffer(GL_NONE));
        }
    }

    void FrameBuffer::OnCubeMapAttach(const CubeMap& cubemap, Attachment attachment)
    {
        GLenum mode = AttachmentTable[int(attachment)];
        GLint cubemapId = cubemap.GetNativeHandle();

        this->Bind();
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, mode, cubemapId, 0));
        if (mode == GL_DEPTH_ATTACHMENT)
        {
            GLCALL(glDrawBuffer(GL_NONE));
        }
    }

    void FrameBuffer::CopyFrameBufferContents(int screenWidth, int screenHeight) const
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glBlitFramebuffer(0, 0, (GLint)this->GetWidth(), (GLint)this->GetHeight(), 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST));
    }

    void FrameBuffer::Validate() const
    {
        this->Bind();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Logger::Instance().Error("OpenGL::FrameBuffer", "framebuffer validation failed: incomplete");
    }

    void FrameBuffer::DetachRenderTarget()
    {
        if (this->currentAttachment == AttachmentType::TEXTURE)
            reinterpret_cast<GResource<Texture>*>(&this->attachmentStorage)->~Resource();
        else if (this->currentAttachment == AttachmentType::CUBEMAP)
            reinterpret_cast<GResource<CubeMap>*>(&this->attachmentStorage)->~Resource();

        this->currentAttachment = AttachmentType::NONE;

        #if defined(MXENGINE_DEBUG)
        this->_texturePtr = nullptr;
        this->_cubemapPtr = nullptr;
        #endif
    }

    bool FrameBuffer::HasTextureAttached() const
    {
        return this->currentAttachment == AttachmentType::TEXTURE;
    }

    bool FrameBuffer::HasCubeMapAttached() const
    {
        return this->currentAttachment == AttachmentType::CUBEMAP;
    }

    void FrameBuffer::UseDrawBuffers(size_t count) const
    {
        this->Bind();
        GLCALL(glDrawBuffers((GLsizei)count, AttachmentTable));
    }

    size_t FrameBuffer::GetWidth() const
    {
        auto* texture = reinterpret_cast<const Resource<Texture, GraphicFactory>*>(&this->attachmentStorage);
        auto* cubemap = reinterpret_cast<const Resource<CubeMap, GraphicFactory>*>(&this->attachmentStorage);

        if (this->currentAttachment == AttachmentType::TEXTURE && texture->IsValid())
            return (*texture)->GetWidth();
        if (this->currentAttachment == AttachmentType::CUBEMAP && cubemap->IsValid())
            return (*cubemap)->GetWidth();

        return 0;
    }

    size_t FrameBuffer::GetHeight() const
    {
        auto* texture = reinterpret_cast<const Resource<Texture, GraphicFactory>*>(&this->attachmentStorage);
        auto* cubemap = reinterpret_cast<const Resource<CubeMap, GraphicFactory>*>(&this->attachmentStorage);

        if (this->currentAttachment == AttachmentType::TEXTURE && texture->IsValid())
            return (*texture)->GetHeight();
        if (this->currentAttachment == AttachmentType::CUBEMAP && cubemap->IsValid())
            return (*cubemap)->GetHeight();

        return 0;
    }

    FrameBuffer::~FrameBuffer()
    {
        this->DetachRenderTarget();
        GLCALL(glDeleteFramebuffers(1, &id));
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& framebuffer) noexcept
    {
        this->id = framebuffer.id;
        this->currentAttachment = framebuffer.currentAttachment;
        this->attachmentStorage = framebuffer.attachmentStorage;
        framebuffer.currentAttachment = AttachmentType::NONE;
        framebuffer.id = 0;
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& framebuffer) noexcept
    {
        this->id = framebuffer.id;
        this->currentAttachment = framebuffer.currentAttachment;
        this->attachmentStorage = framebuffer.attachmentStorage;
        framebuffer.currentAttachment = AttachmentType::NONE;
        framebuffer.id = 0;
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
        GLCALL(glBlitFramebuffer(0, 0, (GLint)this->GetWidth(), (GLint)this->GetHeight(), 0, 0, (GLint)framebuffer.GetWidth(), (GLint)framebuffer.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST));
    }
}