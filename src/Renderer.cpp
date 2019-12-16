#include "Renderer.h"
#include "Logger.h"

namespace MomoEngine
{
	Renderer::Renderer()
	{
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			Logger::Get().Error("glew", "glew init failed");
		}
		else
		{
			Logger::Get().Debug("glew", "OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
		}
	}

	void Renderer::DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElements(GL_TRIANGLES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexType(), nullptr));
	}

	void Renderer::DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElements(GL_LINES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexType(), nullptr));
	}

	void Renderer::DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexType(), nullptr, (GLsizei)count));
	}

	void Renderer::Clear() const
	{
		GLCALL(glClear(clearMask));
	}

	void Renderer::Flush() const
	{
		GLCALL(glFlush());
	}

	Renderer& Renderer::UseSampling(bool value)
	{
		if (value)
		{
			GLCALL(glEnable(GL_MULTISAMPLE));
		}
		else
		{
			GLCALL(glDisable(GL_MULTISAMPLE));
		}
		return *this;
	}

	Renderer& Renderer::UseDepthBuffer(bool value)
	{
		depthBufferEnabled = value;
		if (value)
		{
			GLCALL(glEnable(GL_DEPTH_TEST));
			GLCALL(glDepthFunc(GL_LESS));
			clearMask |= GL_DEPTH_BUFFER_BIT;
		}
		else
		{
			GLCALL(glDisable(GL_DEPTH_TEST));
			clearMask &= ~GL_DEPTH_BUFFER_BIT;
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
}