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
#include "Utilities/Logger/Logger.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Platform/Modules/GraphicModule.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
	GLenum BlendTable[] =
	{
		0x00000000,
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

	Renderer::Renderer()
	{
		this->clearMask |= GL_COLOR_BUFFER_BIT;
	}

	void Renderer::DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElements(GL_TRIANGLES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexTypeId(), nullptr));
	}

	void Renderer::DrawTriangles(const VertexArray& vao, size_t vertexCount, const Shader& shader) const
	{
		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexCount));
	}

	void Renderer::DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElements(GL_LINES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexTypeId(), nullptr));
	}

	void Renderer::DrawLinesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const
	{
		if (count == 0) { this->DrawLines(vao, ibo, shader); return; }

		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElementsInstanced(GL_LINES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexTypeId(), nullptr, (GLsizei)count));
	}

	void Renderer::DrawLinesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const
	{
		if (count == 0) { this->DrawLines(vao, vertexCount, shader); return; }

		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArraysInstanced(GL_LINES, 0, (GLsizei)vertexCount, (GLsizei)count));
	}

	void Renderer::DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const
	{
		if (count == 0) { this->DrawTriangles(vao, ibo, shader); return; }

		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexTypeId(), nullptr, (GLsizei)count));
	}

	void Renderer::DrawTrianglesInstanced(const VertexArray& vao, size_t vertexCount, const Shader& shader, size_t count) const
	{
		if (count == 0) { this->DrawTriangles(vao, vertexCount, shader); return; }

		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArraysInstanced(GL_TRIANGLES, 0, (GLsizei)vertexCount, (GLsizei)count));
	}

	void Renderer::DrawLines(const VertexArray& vao, size_t vertexCount, const Shader& shader) const
	{
		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArrays(GL_LINES, 0, (GLsizei)vertexCount));
	}

	void Renderer::Clear() const
	{
		GLCALL(glClear(clearMask));
	}

	void Renderer::Flush() const
	{
		MAKE_SCOPE_PROFILER("Renderer::Flush");
		GraphicModule::OnRenderDraw();

		GLCALL(glFlush());
	}

	void Renderer::Finish() const
	{
		MAKE_SCOPE_PROFILER("Renderer::Finish");
		GraphicModule::OnRenderDraw();

		GLCALL(glFinish());
	}

	void Renderer::SetViewport(int x, int y, int width, int height) const
	{
		GLCALL(glViewport(x, y, width, height));
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
			Logger::Instance().Debug("OpenGL::Renderer", "multisampling is enabled");
		}
		else
		{
			GLCALL(glDisable(GL_MULTISAMPLE));
			Logger::Instance().Debug("OpenGL::Renderer", "multisampling is disabled");
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
			GLCALL(glDepthFunc(GL_GEQUAL));
			GLCALL(glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE));
		}
		else
		{
			GLCALL(glClearDepth(1.0f));
			GLCALL(glDepthFunc(GL_LESS));
			GLCALL(glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE));
		}
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

	Renderer& Renderer::UseBlending(BlendFactor src, BlendFactor dist)
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
			Logger::Instance().Error("OpenGL::Renderer", "anisotropic filtering is not supported on your device");
		}
		else
		{
			GLCALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, factor));
			Logger::Instance().Debug("OpenGL::Renderer", "set anisotropic filtering factor to " + ToMxString((int)factor) + "x");
		}
		return *this;
	}

    Renderer& Renderer::UseLineWidth(size_t width)
    {
		GLCALL(glLineWidth((GLfloat)width));
		return *this;
    }

	float Renderer::GetLargestAnisotropicFactor() const
	{
		if (!glfwExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		{
			Logger::Instance().Warning("OpenGL::Renderer", "anisotropic filtering is not supported");
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