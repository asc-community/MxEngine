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

#include "GLFrameBuffer.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"
#include "Utilities/Logger/Logger.h"
#include "Core/Macro/Macro.h"

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

    GLFrameBuffer::GLFrameBuffer()
    {
        GLCALL(glGenFramebuffers(1, &id));
        Logger::Instance().Debug("OpenGL::FrameBuffer", "created framebuffer with id = " + std::to_string(id));
    }

    void GLFrameBuffer::AttachTexture(Attachment attachment, int width, int height)
    {
        this->width = width, this->height = height;
        this->texture = Graphics::Instance()->CreateTexture();
        this->cubemap = nullptr;
        GLenum mode = AttachmentTable[int(attachment)];
        this->Bind();

        if (mode == GL_DEPTH_ATTACHMENT)
        {
            this->texture->LoadDepth(width, height);
            GLCALL(glDrawBuffer(GL_NONE));
        }
        else
        {
            this->texture->Load(nullptr, width, height);
        }
        GLint textureId = this->texture->GetNativeHandler();
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, mode, GL_TEXTURE_2D, textureId, 0));
    }

    void GLFrameBuffer::AttachTexture(const Texture& texture, Attachment attachment)
    {
        this->width  = (int)texture.GetWidth();
        this->height = (int)texture.GetHeight();
        GLenum mode = AttachmentTable[int(attachment)];
        GLint textureId = texture.GetNativeHandler();

        this->Bind();
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, mode, texture.GetTextureType(), textureId, 0));
        if (mode == GL_DEPTH_ATTACHMENT)
        {
            GLCALL(glDrawBuffer(GL_NONE));
        }
    }

    void GLFrameBuffer::AttachTexture(UniqueRef<Texture> texture, Attachment attachment)
    {
        this->texture = std::move(texture);
        this->AttachTexture(*this->texture, attachment);
    }

    void GLFrameBuffer::AttachCubeMap(Attachment attachment, int width, int height)
    {
        this->width = width, this->height = height;
        this->cubemap = Graphics::Instance()->CreateCubeMap();
        this->texture = nullptr;
        GLenum mode = AttachmentTable[int(attachment)];
        this->Bind();

        if (mode == GL_DEPTH_ATTACHMENT)
        {
            this->cubemap->LoadDepth(width, height);
            GLCALL(glDrawBuffer(GL_NONE));
        }
        else
        {
            this->cubemap->Load({ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }, width, height);
        }
        GLint cubemapId = this->cubemap->GetNativeHandler();
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, mode, cubemapId, 0));
    }

    void GLFrameBuffer::AttachCubeMap(const CubeMap& cubemap, Attachment attachment)
    {
        this->width = (int)cubemap.GetWidth();
        this->height = (int)cubemap.GetHeight();
        GLenum mode = AttachmentTable[int(attachment)];
        GLint cubemapId = cubemap.GetNativeHandler();

        this->Bind();
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, mode, cubemapId, 0));
        if (mode == GL_DEPTH_ATTACHMENT)
        {
            GLCALL(glDrawBuffer(GL_NONE));
        }
    }

    void GLFrameBuffer::AttachCubeMap(UniqueRef<CubeMap> cubemap, Attachment attachment)
    {
        this->cubemap = std::move(cubemap);
        this->AttachCubeMap(*this->cubemap, attachment);
    }

    void GLFrameBuffer::CopyFrameBufferContents(int screenWidth, int screenHeight) const
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST));
    }

    void GLFrameBuffer::Validate() const
    {
        this->Bind();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Logger::Instance().Error("OpenGL::FrameBuffer", "framebuffer validation failed: incomplete");
    }

    Texture* GLFrameBuffer::GetAttachedTexture()
    {
        return this->texture.get();
    }

    const Texture* GLFrameBuffer::GetAttachedTexture() const
    {
        return this->texture.get();
    }

    CubeMap* GLFrameBuffer::GetAttachedCubeMap()
    {
        return this->cubemap.get();
    }

    const CubeMap* GLFrameBuffer::GetAttachedCubeMap() const
    {
        return this->cubemap.get();
    }

    void GLFrameBuffer::UseDrawBuffers(size_t count) const
    {
        this->Bind();
        GLCALL(glDrawBuffers((GLsizei)count, AttachmentTable));
    }

    int GLFrameBuffer::GetWidth() const
    {
        return this->width;
    }

    int GLFrameBuffer::GetHeight() const
    {
        return this->height;
    }

    GLFrameBuffer::~GLFrameBuffer()
    {
        GLCALL(glDeleteFramebuffers(1, &id));
    }

    void GLFrameBuffer::Bind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, id));
    }

    void GLFrameBuffer::Unbind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void GLFrameBuffer::CopyFrameBufferContents(const FrameBuffer& framebuffer) const
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.GetNativeHandler()));
        GLCALL(glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, framebuffer.GetWidth(), framebuffer.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST));
    }
}