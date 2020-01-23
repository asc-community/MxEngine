#include "Renderer.h"
#include "Utilities/Logger/Logger.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"
#include "Core/ChaiScript/ChaiScriptUtils.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
		if (!this->ViewPort.HasCamera()) return;
		if (!object.IsDrawable()) return;
		size_t iterator = object.GetIterator();
		auto MVP = this->ViewPort.GetCameraMatrix() * object.GetModel();
		Matrix3x3 NormalMatrix = Transpose(Inverse(object.GetModel()));	
		auto cameraPos = this->ViewPort.GetPosition();

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasMaterial())
			{
				const Material& material = renderObject.GetMaterial();

				#define BIND_TEX(name, slot) \
				if (material.name != nullptr)\
					material.name->Bind(slot);\
				else if (object.HasTexture())\
					object.GetTexture().Bind(slot);\
				else\
					this->DefaultTexture->Bind(slot)

				BIND_TEX(map_Ka, 0);
				BIND_TEX(map_Kd, 1);
				BIND_TEX(map_Ks, 2);
				BIND_TEX(map_Ke, 3);
				//BIND_TEX(map_Kd, 4); kd not used now

				const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;
				shader.SetUniformMat4("MVP", MVP);
				shader.SetUniformMat4("Model", object.GetModel());
				shader.SetUniformMat3("NormalMatrix", NormalMatrix);
				shader.SetUniformVec3("material.Ka", material.Ka.r, material.Ka.g, material.Ka.b);
				shader.SetUniformVec3("material.Kd", material.Kd.r, material.Kd.g, material.Kd.b);
				shader.SetUniformVec3("material.Ks", material.Ks.r, material.Ks.g, material.Ks.b);
				shader.SetUniformVec3("material.Ke", material.Ke.r, material.Ke.g, material.Ke.b);
				shader.SetUniformFloat("material.Ns", material.Ns);
				shader.SetUniformFloat("material.d", material.d);
				shader.SetUniformVec3("globalLight", cameraPos.x, cameraPos.y, cameraPos.z);
				shader.SetUniformVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

				if (object.GetInstanceCount() == 0)
					DrawTriangles(renderObject.GetVAO(), renderObject.GetVertexCount(), shader);
				else
					DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetVertexCount(), shader, object.GetInstanceCount());
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RendererImpl::DrawObjectMesh(const IDrawable& object) const
	{
		if (!object.IsDrawable()) return;
		size_t iterator = object.GetIterator();
		auto MVP = this->ViewPort.GetCameraMatrix() * object.GetModel();
		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			this->MeshShader->SetUniformMat4("MVP", MVP);
			if (object.GetInstanceCount() == 0)
				DrawLines(renderObject.GetVAO(), renderObject.GetIBO(), *this->MeshShader);
			else
				DrawLinesInstanced(renderObject.GetVAO(), renderObject.GetIBO(), *this->MeshShader, object.GetInstanceCount());
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
		// draw imgui 
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GLCALL(glFlush());
	}

	void RendererImpl::Finish() const
	{
		// draw imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
			Logger::Instance().Debug("OpenGL", "set anisotropic filtering factor to " + std::to_string((int)factor) + "x");
		}
		return *this;
	}

	RendererImpl& RendererImpl::UseImGuiStyle(ImguiStyle style)
	{
		switch (style)
		{
		case ImguiStyle::CLASSIC:
			ImGui::StyleColorsClassic();
			break;
		case ImguiStyle::LIGHT:
			ImGui::StyleColorsLight();
			break;
		case ImguiStyle::DARK:
			ImGui::StyleColorsDark();
			break;
		default:
			break;
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

	void ChaiScriptRenderer::Init(chaiscript::ChaiScript& chai)
	{
		chai.add(chaiscript::fun(
			[](float zFar) { Renderer::Instance().ViewPort.GetCamera().SetZFar(zFar); }), "set_zfar");
		chai.add(chaiscript::fun(
			[](float zNear) { Renderer::Instance().ViewPort.GetCamera().SetZNear(zNear); }), "set_znear");
		chai.add(chaiscript::fun(
			[]() { return Renderer::Instance().ViewPort.GetCamera().GetZFar(); }), "zfar");
		chai.add(chaiscript::fun(
			[]() { return Renderer::Instance().ViewPort.GetCamera().GetZNear(); }), "znear");
	}
}