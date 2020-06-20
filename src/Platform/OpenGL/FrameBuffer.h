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

#include "Core/Macro/Macro.h"
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
        enum class AttachmentType : uint8_t
        {
            NONE,
            TEXTURE,
            CUBEMAP,
        };

        using BindableId = unsigned int;

        BindableId id = 0;
        AttachmentType currentAttachment = AttachmentType::NONE;
        #if defined(MXENGINE_DEBUG)
        std::aligned_storage_t<32> attachmentStorage;
        #else
        std::aligned_storage_t<24> attachmentStorage;
        #endif

        #if defined(MXENGINE_DEBUG)
        mutable const Texture* _texturePtr = nullptr;
        mutable const CubeMap* _cubemapPtr = nullptr;
        #endif

        void OnTextureAttach(const Texture& texture, Attachment attachment);
        void OnCubeMapAttach(const CubeMap& cubemap, Attachment attachment);
    public:
        FrameBuffer();
        ~FrameBuffer();
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer(FrameBuffer&&) noexcept;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        FrameBuffer& operator=(FrameBuffer&&) noexcept;
        void CopyFrameBufferContents(const FrameBuffer& framebuffer) const;
        void CopyFrameBufferContents(int screenWidth, int screenHeight) const;
        void Validate() const;
        void DetachRenderTarget();
        bool HasTextureAttached() const;
        bool HasCubeMapAttached() const;
        void UseDrawBuffers(size_t count) const;
        size_t GetWidth() const;
        size_t GetHeight() const;
        void Bind() const;
        void Unbind() const;
        BindableId GetNativeHandle() const;

        template<template<typename, typename> typename Resource, typename Factory>
        void AttachTexture(const Resource<Texture, Factory>& texture, Attachment attachment = Attachment::COLOR_ATTACHMENT0)
        {
            static_assert(sizeof(this->attachmentStorage) == sizeof(Resource<Texture, Factory>), "storage size must match object size");

            this->DetachRenderTarget();
            auto* attachedTexture = new(&this->attachmentStorage) Resource<Texture, Factory>(texture);
            this->currentAttachment = AttachmentType::TEXTURE;
            this->OnTextureAttach(**attachedTexture, attachment);

            #if defined(MXENGINE_DEBUG)
            this->_texturePtr = attachedTexture->GetUnchecked();
            this->_cubemapPtr = nullptr;
            #endif
        }

        template<template<typename, typename> typename Resource, typename Factory>
        void AttachTextureExtra(const Resource<Texture, Factory>& texture, Attachment attachment)
        {
            MX_ASSERT(this->currentAttachment != AttachmentType::NONE);
            this->OnTextureAttach(*texture, attachment);
        }

        template<template<typename, typename> typename Resource, typename Factory>
        void AttachCubeMapExtra(const Resource<CubeMap, Factory>& cubemap, Attachment attachment)
        {
            MX_ASSERT(this->currentAttachment != AttachmentType::NONE);
            this->OnCubeMapAttach(*cubemap, attachment);
        }

        template<template<typename, typename> typename Resource, typename Factory>
        void AttachCubeMap(const Resource<CubeMap, Factory>& cubemap, Attachment attachment = Attachment::COLOR_ATTACHMENT0)
        {
            static_assert(sizeof(this->attachmentStorage) == sizeof(Resource<CubeMap, Factory>), "storage size must match object size");

            this->DetachRenderTarget();
            auto* attachedCubeMap = new(&this->attachmentStorage) Resource<CubeMap, Factory>(cubemap);
            this->currentAttachment = AttachmentType::CUBEMAP;
            this->OnCubeMapAttach(**attachedCubeMap, attachment);

            #if defined(MXENGINE_DEBUG)
            this->_cubemapPtr = attachedCubeMap->GetUnchecked();
            this->_texturePtr = nullptr;
            #endif
        }

        template<template<typename, typename> typename Resource, typename Factory>
        Resource<Texture, Factory> GetAttachedTexture() const
        {
            if (!this->HasTextureAttached())
                return Resource<Texture, Factory>{ };

            const auto& texture = *reinterpret_cast<const Resource<Texture, Factory>*>(&this->attachmentStorage);

            #if defined(MXENGINE_DEBUG)
            this->_texturePtr = texture.GetUnchecked();
            #endif

            return texture;
        }

        template<template<typename, typename> typename Resource, typename Factory>
        Resource<CubeMap, Factory> GetAttachedCubeMap() const
        {
            if (!this->HasCubeMapAttached())
                return Resource<CubeMap, Factory>{ };

            const auto& cubemap = *reinterpret_cast<const Resource<CubeMap, Factory>*>(&this->attachmentStorage);

            #if defined(MXENGINE_DEBUG)
            this->_cubemapPtr = cubemap.GetUnchecked();
            #endif

            return cubemap;
        }
    };
}