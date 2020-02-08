#pragma once

#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Platform/OpenGL/GraphicModule/GLGraphicModule.h"
#include "Platform/OpenGL/Window/GLWindow.h"
#include "Platform/OpenGL/IndexBuffer/GLIndexBuffer.h"
#include "Platform/OpenGL/Shader/GLShader.h"
#include "Platform/OpenGL/Texture/GLTexture.h"
#include "Platform/OpenGL/VertexBuffer/GLVertexBuffer.h"
#include "Platform/OpenGL/VertexArray/GLVertexArray.h"
#include "Platform/OpenGL/VertexBufferLayout/GLVertexBufferLayout.h"
#include "Platform/OpenGL/Renderer/GLRenderer.h"

namespace MxEngine
{
    class GLGraphicFactory final : public GraphicFactory
    {
        inline virtual Renderer& GetRenderer() override
        {
            static GLRenderer renderer;
            return renderer;
        }

        inline virtual GraphicModule& GetGraphicModule() override
        {
            static GLGraphicModule module;
            return module;
        }

        inline virtual UniqueRef<Window> CreateWindow() override
        {
            return UniqueRef<Window>(Alloc<GLWindow>());
        }

        inline virtual UniqueRef<IndexBuffer> CreateIndexBuffer() override
        {
            return UniqueRef<IndexBuffer>(Alloc<GLIndexBuffer>());
        }

        inline virtual UniqueRef<Shader> CreateShader() override
        {
            return UniqueRef<Shader>(Alloc<GLShader>());
        }

        inline virtual UniqueRef<Texture> CreateTexture() override
        {
            return UniqueRef<Texture>(Alloc<GLTexture>());
        }

        inline virtual UniqueRef<VertexArray> CreateVertexArray() override
        {
            return UniqueRef<VertexArray>(Alloc<GLVertexArray>());
        }

        inline virtual UniqueRef<VertexBuffer> CreateVertexBuffer() override
        {
            return UniqueRef<VertexBuffer>(Alloc<GLVertexBuffer>());
        }

        inline virtual UniqueRef<VertexBufferLayout> CreateVertexBufferLayout() override
        {
            return UniqueRef<VertexBufferLayout>(Alloc<GLVertexBufferLayout>());
        }

        inline virtual UniqueRef<Window> CreateWindow(int width, int height, const std::string& title)
        {
            auto window = this->CreateWindow();
            window->UseSize(width, height);
            window->UseTitle(title);
            return window;
        }

        inline virtual UniqueRef<IndexBuffer> CreateIndexBuffer(const IndexBuffer::IndexBufferType& data) override
        {
            auto ibo = this->CreateIndexBuffer();
            ibo->Load(data);
            return std::move(ibo);
        }

        inline virtual UniqueRef<Shader> CreateShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) override
        {
            auto shader = this->CreateShader();
            shader->Load(vertexShaderPath, fragmentShaderPath);
            return std::move(shader);
        }

        inline virtual UniqueRef<Texture> CreateTexture(const std::string& filepath, bool genMipmaps = true, bool flipImage = true) override
        {
            auto texture = this->CreateTexture();
            texture->Load(filepath, genMipmaps, flipImage);
            return std::move(texture);
        }

        inline virtual UniqueRef<VertexBuffer> CreateVertexBuffer(const VertexBuffer::BufferData& data, UsageType type) override
        {
            auto vbo = this->CreateVertexBuffer();
            vbo->Load(data, type);
            return std::move(vbo);
        }
    };
}