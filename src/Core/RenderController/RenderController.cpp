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

namespace MxEngine
{
	std::string PointLightUniform[] = 
	{
		"pointLight[?].ambient",
		"pointLight[?].diffuse",
		"pointLight[?].specular",
		"pointLight[?].position",
		"pointLight[?].K",
	};
	constexpr size_t Ambient  = 0;
	constexpr size_t Diffuse  = 1;
	constexpr size_t Specular = 2;
	constexpr size_t Position = 3;

	constexpr size_t KFactor  = 4;
	constexpr size_t PointPos = 11; // pos of '?' in "pointLight[?]"

	std::string SpotLightUniform[] =
	{
		"spotLight[?].ambient",
		"spotLight[?].diffuse",
		"spotLight[?].specular",
		"spotLight[?].position",
		"spotLight[?].direction",
		"spotLight[?].innerAngle",
		"spotLight[?].outerAngle",
	};
	constexpr size_t Direction  = 4;
	constexpr size_t InnerAngle = 5;
	constexpr size_t OuterAngle = 6;
	constexpr size_t SpotPos    = 10; // pos of '?' in "spotLight[?]"

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

	void RenderController::DrawObject(const IDrawable& object, const CameraController& viewport, const LightSystem& lights) const
	{
		// probably nothing to do at all
		if (!viewport.HasCamera()) return;
		if (!object.IsDrawable()) return;

		// getting all data for easy use
		size_t iterator = object.GetIterator();
		const auto& renderColor = object.GetRenderColor();
		const auto& ViewProjection = viewport.GetCameraMatrix();
		const auto& cameraPos = viewport.GetPosition();

		// choosing shader and setting up data per object
		const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;

		this->GetRenderEngine().SetDefaultVertexAttribute(3, object.GetModelMatrix());
		this->GetRenderEngine().SetDefaultVertexAttribute(7, object.GetNormalMatrix());

		shader.SetUniformMat4("ViewProjMatrix", ViewProjection);
		shader.SetUniformVec3("viewPos", cameraPos);
		shader.SetUniformVec4("renderColor", renderColor);

		// set direction light
		shader.SetUniformVec3("dirLight.direction", lights.Global.Direction);
		shader.SetUniformVec3("dirLight.ambient", lights.Global.GetAmbientColor());
		shader.SetUniformVec3("dirLight.diffuse", lights.Global.GetDiffuseColor());
		shader.SetUniformVec3("dirLight.specular", lights.Global.GetSpecularColor());

		// set point lights
		shader.SetUniformInt("pointLightCount", (int)lights.Point.size());
		for (size_t i = 0; i < lights.Point.size(); i++)
		{
			// replace "pointLight[?]" with "pointLight[{i}]"
			for(int j = 0; j < 5; j++) PointLightUniform[j][PointPos] = char('0' + i);

			shader.SetUniformVec3(PointLightUniform[Position], lights.Point[i].Position);
			shader.SetUniformVec3(PointLightUniform[KFactor ], lights.Point[i].GetFactors());
			shader.SetUniformVec3(PointLightUniform[Ambient ], lights.Point[i].GetAmbientColor());
			shader.SetUniformVec3(PointLightUniform[Diffuse ], lights.Point[i].GetDiffuseColor());
			shader.SetUniformVec3(PointLightUniform[Specular], lights.Point[i].GetSpecularColor());
		}

		// set spot lights
		shader.SetUniformInt("spotLightCount", (int)lights.Spot.size());
		for (size_t i = 0; i < lights.Spot.size(); i++)
		{
			// replace "spotLight[?]" with "spotLight[{i}]"
			for (int j = 0; j < 7; j++) SpotLightUniform[j][SpotPos] = char('0' + i);

			shader.SetUniformVec3(SpotLightUniform[Position   ], lights.Spot[i].Position);
			shader.SetUniformVec3(SpotLightUniform[Direction  ], lights.Spot[i].Direction);
			shader.SetUniformVec3(SpotLightUniform[Ambient    ], lights.Spot[i].GetAmbientColor());
			shader.SetUniformVec3(SpotLightUniform[Diffuse    ], lights.Spot[i].GetDiffuseColor());
			shader.SetUniformVec3(SpotLightUniform[Specular   ], lights.Spot[i].GetSpecularColor());
			shader.SetUniformFloat(SpotLightUniform[InnerAngle], lights.Spot[i].GetInnerCos());
			shader.SetUniformFloat(SpotLightUniform[OuterAngle], lights.Spot[i].GetOuterCos());
		}

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (renderObject.HasMaterial())
			{
				const Material& material = renderObject.GetMaterial();

				#define BIND_TEX(NAME, SLOT)        \
				if (material.NAME != nullptr)       \
					material.NAME->Bind(SLOT);      \
				else if (object.HasTexture())       \
					object.GetTexture().Bind(SLOT); \
				else                                \
					this->DefaultTexture->Bind(SLOT)

				BIND_TEX(map_Ka, 0);
				BIND_TEX(map_Kd, 1);
				BIND_TEX(map_Ks, 2);
				BIND_TEX(map_Ke, 3);
				//BIND_TEX(map_Kd, 4); kd not used now

				// setting materials
				shader.SetUniformVec3("material.Ka", material.Ka);
				shader.SetUniformVec3("material.Kd", material.Kd);
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
		auto ViewProjection = viewport.GetCameraMatrix();
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
}