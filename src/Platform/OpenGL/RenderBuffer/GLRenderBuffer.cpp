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

#include "GLRenderBuffer.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"
#include "Utilities/Logger/Logger.h"

namespace MxEngine
{
    GLRenderBuffer::GLRenderBuffer()
    {
        GLCALL(glGenRenderbuffers(1, &this->id));
        Logger::Instance().Debug("OpenGL::RenderBuffer", "created renderbuffer with id = " + std::to_string(this->id));
    }

    GLRenderBuffer::~GLRenderBuffer()
    {
        GLCALL(glDeleteRenderbuffers(1, &this->id));
    }

    int GLRenderBuffer::GetWidth() const
    {
        return this->width;
    }

    int GLRenderBuffer::GetHeight() const
    {
        return this->height;
    }

    int GLRenderBuffer::GetSamples() const
    {
        return this->samples;
    }

    void GLRenderBuffer::Bind() const
    {
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, this->id));
    }

    void GLRenderBuffer::Unbind() const
    {
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }

    void GLRenderBuffer::InitStorage(int width, int height, int samples)
    {
        MX_ASSERT(samples >= 0 && width >= 0 && height >= 0);
        this->width = width;
        this->height = height;
        this->samples = samples;

        this->Bind();
        GLCALL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, (GLsizei)samples, GL_DEPTH_STENCIL, (GLsizei)width, (GLsizei)height));
    }

    void GLRenderBuffer::LinkToFrameBuffer(const FrameBuffer& framebuffer) const
    {
        framebuffer.Bind();
        GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->id));
        framebuffer.Unbind();
    }
}