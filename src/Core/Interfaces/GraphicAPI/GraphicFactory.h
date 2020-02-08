#pragma once

#include "Core/Interfaces/GraphicAPI/GraphicModule.h"
#include "Core/Interfaces/GraphicAPI/Window.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/Texture.h"
#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/VertexBuffer.h"
#include "Core/Interfaces/GraphicAPI/VertexBufferLayout.h"
#include "Core/Interfaces/GraphicAPI/Renderer.h"

#include "Utilities/Memory/Memory.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"

namespace MxEngine
{
    struct GraphicFactory
    {
        virtual GraphicModule& GetGraphicModule() = 0;
        virtual Renderer& GetRenderer() = 0;

        virtual UniqueRef<Window> CreateWindow() = 0;
        virtual UniqueRef<IndexBuffer> CreateIndexBuffer() = 0;
        virtual UniqueRef<Shader> CreateShader() = 0;
        virtual UniqueRef<Texture> CreateTexture() = 0;
        virtual UniqueRef<VertexArray> CreateVertexArray() = 0;
        virtual UniqueRef<VertexBuffer> CreateVertexBuffer() = 0;
        virtual UniqueRef<VertexBufferLayout> CreateVertexBufferLayout() = 0;

        virtual UniqueRef<Window> CreateWindow(int width, int height, const std::string& name) = 0;
        virtual UniqueRef<IndexBuffer> CreateIndexBuffer(const IndexBuffer::IndexBufferType& data) = 0;
        virtual UniqueRef<Shader> CreateShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) = 0;
        virtual UniqueRef<Texture> CreateTexture(const std::string& filepath, bool genMipmaps = true, bool flipImage = true) = 0;
        virtual UniqueRef<VertexBuffer> CreateVertexBuffer(const VertexBuffer::BufferData& data, UsageType type) = 0;

        virtual ~GraphicFactory() = default;
    };

    using Graphics = SingletonHolder<UniqueRef<GraphicFactory>>;
}