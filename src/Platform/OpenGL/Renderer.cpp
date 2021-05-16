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

#include "Renderer.h"
#include "Utilities/Logging/Logger.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Platform/Modules/GraphicModule.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
    GLenum BlendTable[] =
    {
        0x00000000,
        GL_ZERO,
        GL_ONE,
        GL_ONE_MINUS_SRC_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA,
    };

    GLenum PrimitiveTable[] = {
        GL_POINTS,
        GL_LINE_STRIP,
        GL_LINE_LOOP,
        GL_LINES,
        GL_LINE_STRIP_ADJACENCY,
        GL_LINES_ADJACENCY,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN,
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP_ADJACENCY,
        GL_TRIANGLES_ADJACENCY,
        GL_PATCHES,
    };

    Renderer::Renderer()
    {
        this->clearMask |= GL_COLOR_BUFFER_BIT;
    }

    void Renderer::Clear() const
    {
        GLCALL(glClear(clearMask));
    }

    void Renderer::Flush() const
    {
        MAKE_SCOPE_PROFILER("Renderer::Flush");
        GraphicModule::OnRenderDraw();

        glFlush();
    }

    void Renderer::Finish() const
    {
        MAKE_SCOPE_PROFILER("Renderer::Finish");
        GraphicModule::OnRenderDraw();

        glFinish();
    }

    void Renderer::SetViewport(int x, int y, int width, int height) const
    {
        GLCALL(glViewport(x, y, width, height));
    }

    Renderer& Renderer::UseClipDistance(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            GLCALL(glEnable(GL_CLIP_DISTANCE0 + i));
        }
        return *this;
    }

    Renderer& Renderer::UseSeamlessCubeMaps(bool value)
    {
        if (value)
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        else
            glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        return *this;
    }

    void Renderer::DrawVertecies(RenderPrimitive primitive, size_t vertexCount, size_t vertexOffset)
    {
        GLCALL(glDrawArrays(PrimitiveTable[(size_t)primitive], (GLint)vertexOffset, (GLsizei)vertexCount));
    }

    void Renderer::DrawVerteciesInstanced(RenderPrimitive primitive, size_t vertexCount, size_t vertexOffset, size_t instanceCount)
    {
        GLCALL(glDrawArraysInstanced(PrimitiveTable[(size_t)primitive], (GLint)vertexOffset, (GLsizei)vertexCount, instanceCount));
    }

    void Renderer::DrawIndicies(RenderPrimitive primitive, size_t indexCount, size_t indexOffset)
    {
        GLCALL(glDrawElements(
            PrimitiveTable[(size_t)primitive],
            indexCount,
            GetGLType<IndexBuffer::IndexType>(),
            (const void*)(indexOffset * sizeof(IndexBuffer::IndexType))
        ));
    }

    void Renderer::DrawIndiciesInstanced(RenderPrimitive primitive, size_t indexCount, size_t indexOffset, size_t instanceCount)
    {
        GLCALL(glDrawElementsInstanced(
            PrimitiveTable[(size_t)primitive], 
            indexCount, 
            GetGLType<IndexBuffer::IndexType>(), 
            (const void*)(indexOffset * sizeof(IndexBuffer::IndexType)), 
            instanceCount
        ));
    }

    Renderer& Renderer::UseColorMask(bool r, bool g, bool b, bool a)
    {
        GLCALL(glColorMask(r, g, b, a));
        return *this;
    }

    Renderer& Renderer::UseDepthBufferMask(bool value)
    {
        GLCALL(glDepthMask(value));
        return *this;
    }

    Renderer& Renderer::UseSampling(bool value)
    {
        if (value)
        {
            GLCALL(glEnable(GL_MULTISAMPLE));
            MXLOG_DEBUG("OpenGL::Renderer", "native multisampling is enabled");
        }
        else
        {
            GLCALL(glDisable(GL_MULTISAMPLE));
            MXLOG_DEBUG("OpenGL::Renderer", "native multisampling is disabled");
        }
        return *this;
    }

    Renderer& Renderer::UseDepthBuffer(bool value)
    {
        depthBufferEnabled = value;
        if (value)
        {
            GLCALL(glEnable(GL_DEPTH_TEST));
            clearMask |= GL_DEPTH_BUFFER_BIT;
        }
        else
        {
            GLCALL(glDisable(GL_DEPTH_TEST));
            clearMask &= ~GL_DEPTH_BUFFER_BIT;
        }
        return *this;
    }

    Renderer& Renderer::UseReversedDepth(bool value)
    {
        if (value)
        {
            GLCALL(glClearDepth(0.0f));
            GLCALL(glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE));
            this->UseDepthFunction(DepthFunction::GREATER_EQUAL);
        }
        else
        {
            GLCALL(glClearDepth(1.0f));
            GLCALL(glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE));
            this->UseDepthFunction(DepthFunction::LESS);
        }
        return *this;
    }

    GLenum depthFuncTable[] = {
            GL_EQUAL,
            GL_NOTEQUAL,
            GL_LESS,
            GL_GREATER,
            GL_LEQUAL,
            GL_GEQUAL,
            GL_ALWAYS,
            GL_NEVER,
    };

    Renderer& Renderer::UseDepthFunction(DepthFunction function)
    {
        GLCALL(glDepthFunc(depthFuncTable[(size_t)function]));
        return *this;
    }

    Renderer& Renderer::UseCulling(bool value, bool counterClockWise, bool cullBack)
    {
        // culling 
        if (value)
        {
            GLCALL(glEnable(GL_CULL_FACE));
        }
        else
        {
            GLCALL(glDisable(GL_CULL_FACE));
        }

        // point order
        if (counterClockWise)
        {
            GLCALL(glFrontFace(GL_CCW));
        }
        else
        {
            GLCALL(glFrontFace(GL_CW));
        }

        // back / front culling
        if (cullBack)
        {
            GLCALL(glCullFace(GL_BACK));
        }
        else
        {
            GLCALL(glCullFace(GL_FRONT));
        }

        return *this;
    }

    Renderer& Renderer::UseClearColor(float r, float g, float b, float a)
    {
        GLCALL(glClearColor(r, g, b, a));
        return *this;
    }

    Renderer& Renderer::UseBlendFactors(BlendFactor src, BlendFactor dist)
    {
        if (src == BlendFactor::NONE || dist == BlendFactor::NONE)
        {
            GLCALL(glDisable(GL_BLEND));
        }
        else
        {
            GLCALL(glEnable(GL_BLEND));
            GLCALL(glBlendFunc(BlendTable[(size_t)src], BlendTable[(size_t)dist]));
        }
        return *this;
    }

    Renderer& Renderer::UseAnisotropicFiltering(float factor)
    {
        if (!glfwExtensionSupported("GL_EXT_texture_filter_anisotropic"))
        {
            MXLOG_WARNING("OpenGL::Renderer", "anisotropic filtering is not supported on your device");
        }
        else
        {
            GLCALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, factor));
            MXLOG_DEBUG("OpenGL::Renderer", "set anisotropic filtering factor to " + ToMxString((int)factor) + "x");
        }
        return *this;
    }

    float Renderer::GetLargestAnisotropicFactor() const
    {
        if (!glfwExtensionSupported("GL_EXT_texture_filter_anisotropic"))
        {
            MXLOG_WARNING("OpenGL::Renderer", "anisotropic filtering is not supported");
            return 0.0f;
        }
        float factor;
        GLCALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &factor));
        return factor;
    }

    void Renderer::SetDefaultVertexAttribute(size_t index, float v) const
    {
        GLCALL(glVertexAttrib1f((GLuint)index, v));
    }

    void Renderer::SetDefaultVertexAttribute(size_t index, const Vector2& vec) const
    {
        GLCALL(glVertexAttrib2f((GLuint)index, vec.x, vec.y));
    }

    void Renderer::SetDefaultVertexAttribute(size_t index, const Vector3& vec) const
    {
        GLCALL(glVertexAttrib3f((GLuint)index, vec.x, vec.y, vec.z));
    }

    void Renderer::SetDefaultVertexAttribute(size_t index, const Vector4& vec) const
    {
        GLCALL(glVertexAttrib4f((GLuint)index, vec.x, vec.y, vec.z, vec.w));
    }

    void Renderer::SetDefaultVertexAttribute(size_t index, const Matrix4x4& mat) const
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->SetDefaultVertexAttribute(index + i, mat[(glm::length_t)i]);
        }
    }

    void Renderer::SetDefaultVertexAttribute(size_t index, const Matrix3x3& mat) const
    {
        for (size_t i = 0; i < 3; i++)
        {
            this->SetDefaultVertexAttribute(index + i, mat[(glm::length_t)i]);
        }
    }

}