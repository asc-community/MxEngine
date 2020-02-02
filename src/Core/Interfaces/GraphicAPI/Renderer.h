#pragma once

#include "Core/Interfaces/GraphicAPI/VertexArray.h"
#include "Core/Interfaces/GraphicAPI/IndexBuffer.h"
#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Core/Interfaces/GraphicAPI/Texture.h"

namespace MomoEngine
{
    enum class BlendFactor
    {
        NONE,
        ONE_MINUS_SRC_COLOR = 0x0301,
        SRC_ALPHA = 0x0302,
        ONE_MINUS_SRC_ALPHA = 0x0303,
        DST_ALPHA = 0x0304,
        ONE_MINUS_DST_ALPHA = 0x0305,
        DST_COLOR = 0x0306,
        ONE_MINUS_DST_COLOR = 0x0307,
        CONSTANT_COLOR = 0x8001,
        ONE_MINUS_CONSTANT_COLOR = 0x8002,
        CONSTANT_ALPHA = 0x8003,
        ONE_MINUS_CONSTANT_ALPHA = 0x8004,
    };

    enum class WrapType
    {
        CLAMP_TO_EDGE = 0x812F,
        MIRRORED_REPEAT = 0x8370,
        REPEAT = 0x2901,
    };

    enum class MinFilter
    {
        NEAREST_MIPMAP_NEAREST = 0x2700,
        LINEAR_MIPMAP_NEAREST = 0x2701,
        NEAREST_MIPMAP_LINEAR = 0x2702,
        LINEAR_MIPMAP_LINEAR = 0x2703,
    };

    enum class MagFilter
    {
        NEAREST = 0x2600,
        LINEAR = 0x2601,
    };

    struct Renderer
    {
        virtual void DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const = 0;
        virtual void DrawTriangles(const VertexArray& vao, size_t vertexCount, const Shader& shader) const = 0;
        virtual void DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const = 0;
        virtual void DrawTrianglesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const = 0;
        virtual void DrawLines(const VertexArray& vao, size_t vertexCount, const Shader& shader) const = 0;
        virtual void DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const = 0;
        virtual void DrawLinesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const = 0;
        virtual void DrawLinesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const = 0;
        virtual void Clear() const = 0;
        virtual void Flush() const = 0;
        virtual void Finish() const = 0;
        virtual Renderer& UseSampling(bool value = true) = 0;
        virtual Renderer& UseDepthBuffer(bool value = true) = 0;
        virtual Renderer& UseCulling(bool value = true, bool counterClockWise = true, bool cullBack = true) = 0;
        virtual Renderer& UseClearColor(float r, float g, float b, float a = 0.0f) = 0;
        virtual Renderer& UseTextureMinFilter(MinFilter filter) = 0;
        virtual Renderer& UseTextureMagFilter(MagFilter filter) = 0;
        virtual Renderer& UseBlending(BlendFactor src, BlendFactor dist) = 0;
        virtual Renderer& UseTextureWrap(WrapType textureX, WrapType textureY) = 0;
        virtual Renderer& UseAnisotropicFiltering(float factor) = 0;
        virtual float GetLargestAnisotropicFactor() const = 0;
    };
}