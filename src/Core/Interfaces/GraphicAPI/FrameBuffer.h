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

#include "Core/Interfaces/GraphicAPI/IBindable.h"
#include "Core/Interfaces/GraphicAPI/Texture.h"
#include "Core/Interfaces/GraphicAPI/CubeMap.h"
#include "Utilities/Memory/Memory.h"

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

    struct FrameBuffer : IBindable
    {
        virtual void AttachTexture(Attachment attachment, int width, int height)      = 0;
        virtual void AttachTexture(const Texture& texture, Attachment attachment)     = 0;
        virtual void AttachTexture(UniqueRef<Texture> texture, Attachment attachment) = 0;
        virtual void AttachCubeMap(Attachment attachment, int width, int height)      = 0;
        virtual void AttachCubeMap(const CubeMap& cubemap, Attachment attachment)     = 0;
        virtual void AttachCubeMap(UniqueRef<CubeMap> cubemap, Attachment attachment) = 0;
        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual Texture* GetAttachedTexture() = 0;
        virtual const Texture* GetAttachedTexture() const = 0;
        virtual CubeMap* GetAttachedCubeMap() = 0;
        virtual const CubeMap* GetAttachedCubeMap() const = 0;
        virtual void UseDrawBuffers(size_t count) const = 0;
        virtual void CopyFrameBufferContents(const FrameBuffer& framebuffer) const = 0;
        virtual void CopyFrameBufferContents(int screenWidth, int screenHeight) const = 0;
        virtual void Validate() const = 0;
    };
}