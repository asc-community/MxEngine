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
#include "Utilities/Logging/Logger.h"
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
        MXLOG_DEBUG("OpenGL::FrameBuffer", "created framebuffer with id = " + ToMxString(id));
    }

    void FrameBuffer::OnTextureAttach(const Texture& texture, Attachment attachment)
    {
        GLenum mode = AttachmentTable[int(attachment)];
        GLint textureId = texture.GetNativeHandle();

        this->Bind();
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, mode, texture.GetTextureType(), textureId, 0));
    }

    void FrameBuffer::OnCubeMapAttach(const CubeMap& cubemap, Attachment attachment)
    {
        GLenum mode = AttachmentTable[int(attachment)];
        GLint cubemapId = cubemap.GetNativeHandle();

        this->Bind();
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, mode, cubemapId, 0));
    }

    void FrameBuffer::FreeFrameBuffer()
    {
        this->DetachRenderTarget();
        if (this->id != 0)
        {
            GLCALL(glDeleteFramebuffers(1, &id));
            MXLOG_DEBUG("OpenGL::FrameBuffer", "deleted framebuffer with id = " + ToMxString(id));
        }
        this->id = 0;
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
            MXLOG_ERROR("OpenGL::FrameBuffer", "framebuffer validation failed: incomplete");
    }

    void FrameBuffer::DetachRenderTarget()
    {
        if (this->currentAttachment == AttachmentType::TEXTURE)
            std::launder(reinterpret_cast<TextureHandle*>(&this->attachmentStorage))->~Resource();
        else if (this->currentAttachment == AttachmentType::CUBEMAP)
            std::launder(reinterpret_cast<CubeMapHandle*>(&this->attachmentStorage))->~Resource();

        this->currentAttachment = AttachmentType::NONE;

        #if defined(MXENGINE_DEBUG)
        this->_texturePtr = nullptr;
        this->_cubemapPtr = nullptr;
        #endif
    }

    void FrameBuffer::DetachExtraTarget(Attachment attachment)
    {
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, AttachmentTable[(int)attachment], 0, 0));
    }

    bool FrameBuffer::HasTextureAttached() const
    {
        return this->currentAttachment == AttachmentType::TEXTURE;
    }

    bool FrameBuffer::HasCubeMapAttached() const
    {
        return this->currentAttachment == AttachmentType::CUBEMAP;
    }

    void FrameBuffer::UseDrawBuffers(ArrayView<Attachment> attachments) const
    {
        std::array<GLenum, 20> attachmentTypes{ };
        MX_ASSERT(attachments.size() <= attachmentTypes.size());

        this->Bind();
        for (size_t i = 0; i < attachments.size(); i++)
        {
            attachmentTypes[i] = AttachmentTable[(int)attachments[i]];
        }
        GLCALL(glDrawBuffers((GLsizei)attachments.size(), attachmentTypes.data()));
    }

    void FrameBuffer::UseOnlyDepth() const
    {
        this->Bind();
        GLCALL(glDrawBuffer(GL_NONE));
    }

    size_t FrameBuffer::GetWidth() const
    {
        auto* texture = reinterpret_cast<const Resource<Texture, GraphicFactory>*>(&this->attachmentStorage);
        auto* cubemap = reinterpret_cast<const Resource<CubeMap, GraphicFactory>*>(&this->attachmentStorage);

        if (this->currentAttachment == AttachmentType::TEXTURE && std::launder(texture)->IsValid())
            return (*texture)->GetWidth();
        if (this->currentAttachment == AttachmentType::CUBEMAP && std::launder(cubemap)->IsValid())
            return (*cubemap)->GetWidth();

        return 0;
    }

    size_t FrameBuffer::GetHeight() const
    {
        auto* texture = reinterpret_cast<const Resource<Texture, GraphicFactory>*>(&this->attachmentStorage);
        auto* cubemap = reinterpret_cast<const Resource<CubeMap, GraphicFactory>*>(&this->attachmentStorage);

        if (this->currentAttachment == AttachmentType::TEXTURE && std::launder(texture)->IsValid())
            return (*std::launder(texture))->GetHeight();
        if (this->currentAttachment == AttachmentType::CUBEMAP && std::launder(cubemap)->IsValid())
            return (*std::launder(cubemap))->GetHeight();

        return 0;
    }

    FrameBuffer::~FrameBuffer()
    {
        this->FreeFrameBuffer();
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
        this->FreeFrameBuffer();

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