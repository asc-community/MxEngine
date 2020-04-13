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

#include "RenderController.h"
#include "Core/Interfaces/GraphicAPI/FrameBuffer.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
	RenderController::RenderController(Renderer& renderer)
		: renderer(renderer)
	{
		
	}

	Renderer& RenderController::GetRenderEngine() const
	{
		return this->renderer;
	}

	void RenderController::Render() const
	{
		this->renderer.Finish();
	}

	void RenderController::Clear() const
	{
		this->renderer.Clear();
	}

	void RenderController::AttachDepthTexture(const Texture& texture)
	{
		this->DepthBuffer->AttachTexture(texture, Attachment::DEPTH_ATTACHMENT);
		this->SetViewport(0, 0, this->DepthBuffer->GetWidth(), this->DepthBuffer->GetHeight());
		this->Clear();
	}

    void RenderController::AttachDepthCubeMap(const CubeMap& cubemap)
    {
		this->DepthBuffer->AttachCubeMap(cubemap, Attachment::DEPTH_ATTACHMENT);
		this->SetViewport(0, 0, this->DepthBuffer->GetWidth(), this->DepthBuffer->GetHeight());
		this->Clear();
    }

	void RenderController::DetachDepthBuffer(int viewportWidth, int viewportHeight)
	{
		this->DepthBuffer->Unbind();
		this->SetViewport(0, 0, viewportWidth, viewportHeight);
	}
	
	void RenderController::ToggleReversedDepth(bool value) const
	{
		this->renderer.UseReversedDepth(value);
	}

	void RenderController::ToggleFaceCulling(bool value, bool counterClockWise, bool cullBack) const
	{
		this->GetRenderEngine().UseCulling(value, counterClockWise, cullBack);
	}

	void RenderController::SetAnisotropicFiltering(float value) const
	{
		this->GetRenderEngine().UseAnisotropicFiltering(value);
	}

	void RenderController::SetViewport(int x, int y, int width, int height) const
	{
		this->renderer.SetViewport(x, y, width, height);
	}

    void RenderController::DrawObject(const IDrawable& object, const CameraController& viewport) const
    {
		// probably nothing to do at all
		if (!viewport.HasCamera()) return;
		if (!object.IsDrawable()) return;

		// getting all data for easy use
		size_t iterator = object.GetIterator();
		const auto& renderColor = object.GetRenderColor();
		const auto& ViewProjection = viewport.GetMatrix();

		// choosing shader and setting up data per object
		const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;

		this->GetRenderEngine().SetDefaultVertexAttribute(3, object.GetModelMatrix());
		this->GetRenderEngine().SetDefaultVertexAttribute(7, object.GetNormalMatrix());

		shader.SetUniformMat4("ViewProjMatrix", ViewProjection);
		shader.SetUniformVec4("renderColor", renderColor);

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasMaterial())
			{
				const Material& material = renderObject.GetMaterial();

				#define BIND_TEX(NAME, SLOT)         \
				if (material.NAME != nullptr)        \
					material.NAME->Bind(SLOT);       \
				else if (object.HasTexture())        \
					object.GetTexture().Bind(SLOT);  \
				else                                 \
					this->DefaultTexture->Bind(SLOT);\
				shader.SetUniformInt(#NAME, SLOT)

				BIND_TEX(map_Ka, 0);

				// setting materials
				shader.SetUniformFloat("material.d", material.d);

				if (object.GetInstanceCount() == 0)
				{
					this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetVertexBufferSize(), shader);
				}
				else
				{
					this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetVertexBufferSize(), shader, object.GetInstanceCount());
				}
			}
			iterator = object.GetNext(iterator);
		}
    }

	void RenderController::DrawObject(const IDrawable& object, const CameraController& viewport, const LightSystem& lights) const
	{
		// probably nothing to do at all
		if (!viewport.HasCamera()) return;
		if (!object.IsDrawable()) return;

		// getting all data for easy use
		size_t iterator = object.GetIterator();
		const auto& renderColor = object.GetRenderColor();
		const auto& ViewProjection = viewport.GetMatrix();
		const auto& cameraPos = viewport.GetPosition();

		// choosing shader and setting up data per object
		const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;

		this->GetRenderEngine().SetDefaultVertexAttribute(3, object.GetModelMatrix());
		this->GetRenderEngine().SetDefaultVertexAttribute(7, object.GetNormalMatrix());

		shader.SetUniformMat4("ViewProjMatrix", ViewProjection);
		shader.SetUniformVec3("viewPos", cameraPos);
		shader.SetUniformVec4("renderColor", renderColor);

		// set shadow mapping
		shader.SetUniformInt("PCFdistance", this->PCFdistance);
		shader.SetUniformMat4("DirLightProjMatrix", MakeBiasMatrix() * lights.Global->GetMatrix());
		for (size_t i = 0; i < lights.Spot.size(); i++)
		{
			shader.SetUniformMat4(Format(FMT_STRING("SpotLightProjMatrix[{0}]"), i), MakeBiasMatrix() * lights.Spot[i].GetMatrix());
		}

		// set direction light
		shader.SetUniformVec3("dirLight.direction", lights.Global->Direction);
		shader.SetUniformVec3("dirLight.ambient", lights.Global->GetAmbientColor());
		shader.SetUniformVec3("dirLight.diffuse", lights.Global->GetDiffuseColor());
		shader.SetUniformVec3("dirLight.specular", lights.Global->GetSpecularColor());

		// set point lights
		shader.SetUniformInt("pointLightCount", (int)lights.Point.size());
		for (size_t i = 0; i < lights.Point.size(); i++)
		{
			shader.SetUniformVec3 (Format(FMT_STRING("pointLight[{0}].position"), i), lights.Point[i].Position);
			shader.SetUniformFloat(Format(FMT_STRING("pointLight[{0}].zfar"),     i), lights.Point[i].FarDistance);
			shader.SetUniformVec3 (Format(FMT_STRING("pointLight[{0}].K"),        i), lights.Point[i].GetFactors());
			shader.SetUniformVec3 (Format(FMT_STRING("pointLight[{0}].ambient"),  i), lights.Point[i].GetAmbientColor());
			shader.SetUniformVec3 (Format(FMT_STRING("pointLight[{0}].diffuse"),  i), lights.Point[i].GetDiffuseColor());
			shader.SetUniformVec3 (Format(FMT_STRING("pointLight[{0}].specular"), i), lights.Point[i].GetSpecularColor());
		}

		// set spot lights
		shader.SetUniformInt("spotLightCount", (int)lights.Spot.size());
		for (size_t i = 0; i < lights.Spot.size(); i++)
		{
			shader.SetUniformVec3 (Format(FMT_STRING("spotLight[{0}].position"),   i), lights.Spot[i].Position);
			shader.SetUniformVec3 (Format(FMT_STRING("spotLight[{0}].direction"),  i), lights.Spot[i].GetDirection());
			shader.SetUniformVec3 (Format(FMT_STRING("spotLight[{0}].ambient"),    i), lights.Spot[i].GetAmbientColor());
			shader.SetUniformVec3 (Format(FMT_STRING("spotLight[{0}].diffuse"),    i), lights.Spot[i].GetDiffuseColor());
			shader.SetUniformVec3 (Format(FMT_STRING("spotLight[{0}].specular"),   i), lights.Spot[i].GetSpecularColor());
			shader.SetUniformFloat(Format(FMT_STRING("spotLight[{0}].innerAngle"), i), lights.Spot[i].GetInnerCos());
			shader.SetUniformFloat(Format(FMT_STRING("spotLight[{0}].outerAngle"), i), lights.Spot[i].GetOuterCos());
		}

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasMaterial())
			{
				const Material& material = renderObject.GetMaterial();

				#define BIND_TEX(NAME, SLOT)         \
				if (material.NAME != nullptr)        \
					material.NAME->Bind(SLOT);       \
				else if (object.HasTexture())        \
					object.GetTexture().Bind(SLOT);  \
				else                                 \
					this->DefaultTexture->Bind(SLOT);\
				shader.SetUniformInt(#NAME, SLOT)

				BIND_TEX(map_Ka, 0);
				BIND_TEX(map_Kd, 1);
				BIND_TEX(map_Ks, 2);
				BIND_TEX(map_Ke, 3);

				lights.Global->GetDepthTexture()->Bind(4);
				shader.SetUniformInt("map_dirLight_shadow", 4);

				for (int i = 0; i < lights.Spot.size(); i++)
				{
					int bindIndex = 5 + i;
					lights.Spot[i].GetDepthTexture()->Bind(bindIndex);
					shader.SetUniformInt(Format(FMT_STRING("map_spotLight_shadow[{0}]"), i), bindIndex);
				}

				for (int i = 0; i < lights.Point.size(); i++)
				{
					int bindIndex = (5 + (int)lights.Spot.size()) + i;
					lights.Point[i].GetDepthCubeMap()->Bind(bindIndex);
					shader.SetUniformInt(Format(FMT_STRING("map_pointLight_shadow[{0}]"), i), bindIndex);
				}

				// dont ask - OpenGL requires all samplerCubes to be bound
				constexpr size_t MAX_POINT_SOURCES = 2;
				for (int i = (int)lights.Point.size(); i < MAX_POINT_SOURCES; i++)
				{
					int bindIndex = int(5 + lights.Spot.size() + lights.Point.size()) + i;
					lights.Global->GetDepthTexture()->Bind(bindIndex);
					shader.SetUniformInt(Format(FMT_STRING("map_pointLight_shadow[{0}]"), i), bindIndex);
				}

				// setting materials (ka, kd not used for now
				// shader.SetUniformVec3("material.Ka", material.Ka);
				// shader.SetUniformVec3("material.Kd", material.Kd);
				shader.SetUniformVec3("material.Ks", material.Ks);
				shader.SetUniformVec3("material.Ke", material.Ke);
				shader.SetUniformFloat("material.Ns", material.Ns);
				shader.SetUniformFloat("material.d", material.d);

				shader.SetUniformFloat("Ka", material.f_Ka);
				shader.SetUniformFloat("Kd", material.f_Kd);

				if (object.GetInstanceCount() == 0)
				{
					this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetVertexBufferSize(), shader);
				}
				else
				{
					this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetVertexBufferSize(), shader, object.GetInstanceCount());
				}
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RenderController::DrawObjectMesh(const IDrawable& object, const CameraController& viewport) const
	{
		// probably nothing to do at all
		if (!viewport.HasCamera()) return;
		if (!object.IsDrawable()) return;

		size_t iterator = object.GetIterator();
		auto ViewProjection = viewport.GetMatrix();
		this->MeshShader->SetUniformMat4("ViewProjMatrix", ViewProjection);

		this->GetRenderEngine().SetDefaultVertexAttribute(3, object.GetModelMatrix());
		this->GetRenderEngine().SetDefaultVertexAttribute(7, object.GetNormalMatrix());

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);

			if (object.GetInstanceCount() == 0)
			{
				this->GetRenderEngine().DrawLines(renderObject.GetVAO(), renderObject.GetMeshIBO(), *this->MeshShader);
			}
			else
			{
				this->GetRenderEngine().DrawLinesInstanced(renderObject.GetVAO(), renderObject.GetMeshIBO(), *this->MeshShader, object.GetInstanceCount());
			}
			iterator = object.GetNext(iterator);
		}
	}

    void RenderController::DrawSkybox(const Skybox& skybox, const CameraController& viewport)
    {
		if (!viewport.HasCamera()) return;
		if (skybox.SkyboxTexture == nullptr) return;

		auto& shader = *skybox.SkyboxShader;
		auto View = (Matrix3x3)viewport.GetCamera().GetViewMatrix();
		auto Projection = viewport.GetCamera().GetProjectionMatrix();
		shader.SetUniformMat4("ViewProjection", Projection * (Matrix4x4)View);
		shader.SetUniformMat3("Rotation", skybox.GetRotationMatrix());

		skybox.SkyboxTexture->Bind(0);
		shader.SetUniformInt("skybox", 0);

		this->GetRenderEngine().DrawTriangles(*skybox.VAO, skybox.VBO->GetSize(), shader);
    }

	void RenderController::SetPCFDistance(int value)
	{
		this->PCFdistance = value;
	}
}