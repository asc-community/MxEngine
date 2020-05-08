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

#include "Core/Interfaces/GraphicAPI/FrameBuffer.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"

namespace MxEngine
{
    class GLFrameBuffer : public FrameBuffer
    {
        int width = 0, height = 0;
        UniqueRef<Texture> texture;
        UniqueRef<CubeMap> cubemap;
    public:
        GLFrameBuffer();
        ~GLFrameBuffer();
        GLFrameBuffer(const GLFrameBuffer&) = delete;

        // Inherited via FrameBuffer
        virtual void AttachTexture(Attachment attachment, int width, int height) override;
        virtual void AttachTexture(const Texture& texture, Attachment attachment) override;
        virtual void AttachTexture(UniqueRef<Texture> texture, Attachment attachment) override;
        virtual void AttachCubeMap(Attachment attachment, int width, int height) override;
        virtual void AttachCubeMap(const CubeMap& cubemap, Attachment attachment) override;
        virtual void AttachCubeMap(UniqueRef<CubeMap> cubemap, Attachment attachment) override;
        virtual void CopyFrameBufferContents(const FrameBuffer& framebuffer) const override;
        virtual void CopyFrameBufferContents(int screenWidth, int screenHeight) const override;
        virtual void Validate() const override;
        virtual Texture* GetAttachedTexture() override;
        virtual const Texture* GetAttachedTexture() const override;
        virtual CubeMap* GetAttachedCubeMap() override;
        virtual const CubeMap* GetAttachedCubeMap() const override;
        virtual void UseDrawBuffers(size_t count) const override;
        virtual int GetWidth() const override;
        virtual int GetHeight() const override;
        virtual void Bind() const override;
        virtual void Unbind() const override;
    };
}