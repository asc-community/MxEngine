#include "Renderer.h"
#include "Utilities/Logger/Logger.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"

namespace MomoEngine
{
	RendererImpl::RendererImpl()
	{
		this->clearMask |= GL_COLOR_BUFFER_BIT;
	}

	void RendererImpl::DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const MomoEngine::Shader& shader) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElements(GL_TRIANGLES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexType(), nullptr));
	}

	void RendererImpl::DrawTriangles(const VertexArray& vao, size_t vertexCount, const MomoEngine::Shader& shader) const
	{
		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexCount));
	}

	void RendererImpl::DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const MomoEngine::Shader& shader) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElements(GL_LINES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexType(), nullptr));
	}

	void RendererImpl::DrawLinesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const MomoEngine::Shader& shader, size_t count) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElementsInstanced(GL_LINES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexType(), nullptr, (GLsizei)count));
	}

	void RendererImpl::DrawLinesInstanced(const VertexArray& vao, size_t vertexCount, const MomoEngine::Shader& shader, size_t count) const
	{
		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArraysInstanced(GL_LINES, 0, (GLsizei)vertexCount, (GLsizei)count));
	}

	void RendererImpl::Draw(const IDrawable& object) const
	{
		size_t iterator = object.GetIterator();
		auto MVP = this->Camera.GetMatrix() * object.GetModel();
		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasTexture())
			{
				renderObject.GetTexture().Bind();
				if (object.HasShader())
				{
					object.GetShader().SetUniformMat4("MVP", MVP);
					DrawTriangles(renderObject.GetVAO(), renderObject.GetVertexCount(), object.GetShader());
				}
				else
				{
					this->ObjectShader->SetUniformMat4("MVP", MVP);
					DrawTriangles(renderObject.GetVAO(), renderObject.GetVertexCount(), *this->ObjectShader);
				}
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RendererImpl::DrawInstanced(const IDrawable& object, size_t count) const
	{
		size_t iterator = object.GetIterator();
		auto MVP = this->Camera.GetMatrix() * object.GetModel();
		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasTexture())
			{
				renderObject.GetTexture().Bind();
				if (object.HasShader())
				{
					object.GetShader().SetUniformMat4("MVP", MVP);
					DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetVertexCount(), object.GetShader(), count);
				}
				else
				{
					this->ObjectShader->SetUniformMat4("MVP", MVP);
					DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetVertexCount(), *this->ObjectShader, count);
				}
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RendererImpl::DrawObjectMesh(const IDrawable& object) const
	{
		size_t iterator = object.GetIterator();
		auto MVP = this->Camera.GetMatrix() * object.GetModel();
		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasTexture())
			{
				this->MeshShader->SetUniformMat4("MVP", MVP);
				DrawLines(renderObject.GetVAO(), renderObject.GetIBO(), *this->MeshShader);
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RendererImpl::DrawObjectMeshInstanced(const IDrawable& object, size_t count) const
	{
		size_t iterator = object.GetIterator();
		auto MVP = this->Camera.GetMatrix() * object.GetModel();
		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasTexture())
			{
				this->MeshShader->SetUniformMat4("MVP", MVP);
				DrawLinesInstanced(renderObject.GetVAO(), renderObject.GetIBO(), *this->MeshShader, count);
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RendererImpl::DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const MomoEngine::Shader& shader, size_t count) const
	{
		vao.Bind();
		ibo.Bind();
		shader.Bind();
		GLCALL(glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)ibo.GetCount(), (GLenum)ibo.GetIndexType(), nullptr, (GLsizei)count));
	}

	void RendererImpl::DrawTrianglesInstanced(const VertexArray& vao, size_t vertexCount, const MomoEngine::Shader& shader, size_t count) const
	{
		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArraysInstanced(GL_TRIANGLES, 0, (GLsizei)vertexCount, (GLsizei)count));
	}

	void RendererImpl::DrawLines(const VertexArray& vao, size_t vertexCount, const MomoEngine::Shader& shader) const
	{
		vao.Bind();
		shader.Bind();
		GLCALL(glDrawArrays(GL_LINES, 0, (GLsizei)vertexCount));
	}

	void RendererImpl::Clear() const
	{
		GLCALL(glClear(clearMask));
	}

	void RendererImpl::Flush() const
	{
		GLCALL(glFlush());
	}

	void RendererImpl::Finish() const
	{
		GLCALL(glFinish());
	}

	RendererImpl& RendererImpl::UseSampling(bool value)
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

	RendererImpl& RendererImpl::UseDepthBuffer(bool value)
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

	RendererImpl& RendererImpl::UseCulling(bool value, bool counterClockWise, bool cullBack)
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

	RendererImpl& RendererImpl::UseClearColor(float r, float g, float b, float a)
	{
		GLCALL(glClearColor(r, g, b, a));
		return *this;
	}
	RendererImpl& RendererImpl::UseTextureMinFilter(MinFilter filter)
	{
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)filter));
		return *this;
	}

	RendererImpl& RendererImpl::UseTextureMagFilter(MagFilter filter)
	{
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)filter));
		return *this;
	}

	RendererImpl& RendererImpl::UseBlending(BlendFactor src, BlendFactor dist)
	{
		if (src == BlendFactor::NONE || dist == BlendFactor::NONE)
		{
			GLCALL(glDisable(GL_BLEND));
		}
		else
		{
			GLCALL(glEnable(GL_BLEND));
			GLCALL(glBlendFunc((GLenum)GL_SRC_ALPHA, (GLenum)GL_ONE_MINUS_SRC_ALPHA));
		}
		return *this;
	}

	RendererImpl& RendererImpl::UseTextureWrap(WrapType textureX, WrapType textureY)
	{
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)textureX));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)textureY));
		return *this;
	}

	RendererImpl& RendererImpl::UseAnisotropicFiltering(float factor)
	{
		if (!glfwExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		{
			Logger::Instance().Warning("OpenGL", "anisotropic filtering is not supported");
		}
		else
		{
			GLCALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, factor));
			Logger::Instance().Debug("OpenGL", "set anisotropic filtering factor to " + std::to_string(factor));
		}
		return *this;
	}

	float RendererImpl::GetLargestAnisotropicFactor() const
	{
		if (!glfwExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		{
			Logger::Instance().Warning("OpenGL", "anisotropic filtering is not supported");
			return 0.0f;
		}
		float factor;
		GLCALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &factor));
		return factor;
	}
}