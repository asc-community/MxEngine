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
		: renderer(renderer) { }

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

	void RenderController::DrawObject(const IDrawable& object) const
	{
		// probably nothing to do at all
		if (!this->ViewPort.HasCamera()) return;
		if (!object.IsDrawable()) return;

		// getting all data for easy use
		size_t iterator = object.GetIterator();
		auto MVP = this->ViewPort.GetCameraMatrix() * object.GetModelMatrix();
		Matrix3x3 NormalMatrix = Transpose(Inverse(object.GetModelMatrix()));
		auto cameraPos = this->ViewPort.GetPosition();

		// choosing shader and setting up data per object
		const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;
		shader.SetUniformMat4("MVP", MVP);
		shader.SetUniformMat4("Model", object.GetModelMatrix());
		shader.SetUniformMat3("NormalMatrix", NormalMatrix);
		shader.SetUniformVec3("viewPos", cameraPos);

		// set direction light
		shader.SetUniformVec3("dirLight.direction", this->GlobalLight.Direction);
		shader.SetUniformVec3("dirLight.ambient", this->GlobalLight.GetAmbientColor());
		shader.SetUniformVec3("dirLight.diffuse", this->GlobalLight.GetDiffuseColor());
		shader.SetUniformVec3("dirLight.specular", this->GlobalLight.GetSpecularColor());

		// set point lights
		shader.SetUniformInt("pointLightCount", this->PointLights.GetCount());
		for (size_t i = 0; i < this->PointLights.GetCount(); i++)
		{
			// replace "pointLight[?]" with "pointLight[{i}]"
			for(int j = 0; j < 5; j++) PointLightUniform[j][PointPos] = char('0' + i);

			shader.SetUniformVec3(PointLightUniform[Position], this->PointLights[i].Position);
			shader.SetUniformVec3(PointLightUniform[KFactor ], this->PointLights[i].GetFactors());
			shader.SetUniformVec3(PointLightUniform[Ambient ], this->PointLights[i].GetAmbientColor());
			shader.SetUniformVec3(PointLightUniform[Diffuse ], this->PointLights[i].GetDiffuseColor());
			shader.SetUniformVec3(PointLightUniform[Specular], this->PointLights[i].GetSpecularColor());
		}

		// set spot lights
		shader.SetUniformInt("spotLightCount", this->SpotLights.GetCount());
		for (size_t i = 0; i < this->SpotLights.GetCount(); i++)
		{
			// replace "spotLight[?]" with "spotLight[{i}]"
			for (int j = 0; j < 7; j++) SpotLightUniform[j][SpotPos] = char('0' + i);

			shader.SetUniformVec3(SpotLightUniform[Position   ], this->SpotLights[i].Position);
			shader.SetUniformVec3(SpotLightUniform[Direction  ], this->SpotLights[i].Direction);
			shader.SetUniformVec3(SpotLightUniform[Ambient    ], this->SpotLights[i].GetAmbientColor());
			shader.SetUniformVec3(SpotLightUniform[Diffuse    ], this->SpotLights[i].GetDiffuseColor());
			shader.SetUniformVec3(SpotLightUniform[Specular   ], this->SpotLights[i].GetSpecularColor());
			shader.SetUniformFloat(SpotLightUniform[InnerAngle], this->SpotLights[i].GetInnerCos());
			shader.SetUniformFloat(SpotLightUniform[OuterAngle], this->SpotLights[i].GetOuterCos());
		}

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
					this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetVertexCount(), shader);
				else
					this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetVertexCount(), shader, object.GetInstanceCount());
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RenderController::DrawObjectMesh(const IDrawable& object) const
	{
		// probably nothing to do at all
		if (!this->ViewPort.HasCamera()) return;
		if (!object.IsDrawable()) return;

		size_t iterator = object.GetIterator();
		auto MVP = this->ViewPort.GetCameraMatrix() * object.GetModelMatrix();
		this->MeshShader->SetUniformMat4("MVP", MVP);

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			if (object.GetInstanceCount() == 0)
				this->GetRenderEngine().DrawLines(renderObject.GetVAO(), renderObject.GetMeshIBO(), *this->MeshShader);
			else
				this->GetRenderEngine().DrawLinesInstanced(renderObject.GetVAO(), renderObject.GetMeshIBO(), *this->MeshShader, object.GetInstanceCount());
			iterator = object.GetNext(iterator);
		}
	}
}