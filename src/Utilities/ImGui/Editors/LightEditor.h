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

#pragma once

#include "Core/Application/Application.h"
#include "Utilities/ImGui/Layout/Layout.h"

namespace MxEngine::GUI
{
	inline void DrawLightEditor()
	{
		auto context = Application::Get();

		auto& scene = context->GetCurrentScene();
		auto& globalLight = scene.GlobalLight;

		if (ImGui::CollapsingHeader("global light", ImGuiTreeNodeFlags_None))
		{
			ImGui::PushID(0xFFFF);

			static int bufferSize = (int)context->GetRenderer().GetDepthBufferSize<DirectionalLight>();
			if (GUI::InputIntOnClick("depth buffer size", &bufferSize))
			{
				bufferSize = (int)ToNearestPowTwo((size_t)Clamp(bufferSize, 1, 1 << 16));
				context->GetRenderer().SetDepthBufferSize<DirectionalLight>(bufferSize);
				bufferSize = (int)context->GetRenderer().GetDepthBufferSize<DirectionalLight>();
			}
			ImGui::InputInt("PCF distance", &context->GetRenderer().PCFdistance);

			ImGui::InputFloat3("direction", &globalLight.Direction[0]);
			ImGui::DragFloat("projection size", &globalLight.ProjectionSize, 10.0f, 50.0f, 5000.0f);

			ImGui::ColorEdit3("ambient color", &globalLight.AmbientColor[0],   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("diffuse color", &globalLight.DiffuseColor[0],   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("specular color", &globalLight.SpecularColor[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);

			globalLight.AmbientColor  = VectorMax(globalLight.AmbientColor,  MakeVector3(0.0f));
			globalLight.DiffuseColor  = VectorMax(globalLight.DiffuseColor,  MakeVector3(0.0f));
			globalLight.SpecularColor = VectorMax(globalLight.SpecularColor, MakeVector3(0.0f));

			ImGui::PopID();
		}

		for (size_t i = 0; i < scene.PointLights.GetCount(); i++)
		{
			static std::string strLight;
			strLight = "point light #" + std::to_string(i);
			if (ImGui::CollapsingHeader(strLight.c_str(), ImGuiTreeNodeFlags_None))
			{
				ImGui::PushID((int)i);

				static int bufferSize = (int)context->GetRenderer().GetDepthBufferSize<PointLight>();
				if (GUI::InputIntOnClick("depth buffer size", &bufferSize))
				{
					bufferSize = (int)ToNearestPowTwo((size_t)Clamp(bufferSize, 1, 1 << 16));
					context->GetRenderer().SetDepthBufferSize<PointLight>(bufferSize);
					int bufferSize = (int)context->GetRenderer().GetDepthBufferSize<PointLight>();
				}

				auto& pointLight = scene.PointLights[i];
				auto factors = pointLight.GetFactors();

				ImGui::DragFloat3("position", &pointLight.Position[0], 0.5f);
				ImGui::ColorEdit3("ambient color", &pointLight.AmbientColor[0],   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("diffuse color", &pointLight.DiffuseColor[0],   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("specular color", &pointLight.SpecularColor[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);

				pointLight.AmbientColor  = VectorMax(pointLight.AmbientColor,  MakeVector3(0.0f));
				pointLight.DiffuseColor  = VectorMax(pointLight.DiffuseColor,  MakeVector3(0.0f));
				pointLight.SpecularColor = VectorMax(pointLight.SpecularColor, MakeVector3(0.0f));

				if (ImGui::InputFloat3("K factors", &factors[0]))
					pointLight.UseFactors(factors);

				ImGui::PopID();
			}
		}

		for (size_t i = 0; i < scene.SpotLights.GetCount(); i++)
		{
			static std::string strLight;
			strLight = "spot light #" + std::to_string(i);
			if (ImGui::CollapsingHeader(strLight.c_str(), ImGuiTreeNodeFlags_None))
			{
				ImGui::PushID(int(i + scene.PointLights.GetCount()));

				static int bufferSize = (int)context->GetRenderer().GetDepthBufferSize<SpotLight>();
				if (GUI::InputIntOnClick("depth buffer size", &bufferSize))
				{
					bufferSize = (int)ToNearestPowTwo((size_t)Clamp(bufferSize, 1, 1 << 16));
					context->GetRenderer().SetDepthBufferSize<SpotLight>(bufferSize);
					bufferSize = (int)context->GetRenderer().GetDepthBufferSize<SpotLight>();
				}

				auto& spotLight = scene.SpotLights[i];
				auto innerAngle = spotLight.GetInnerAngle();
				auto outerAngle = spotLight.GetOuterAngle();

				ImGui::DragFloat3("position", &spotLight.Position[0], 0.5f);
				ImGui::InputFloat3("direction", &spotLight.Direction[0]);

				if (ImGui::DragFloat("outer angle", &outerAngle, 1.0f, 0.0f, 90.0f))
					spotLight.UseOuterAngle(outerAngle);
				if (ImGui::DragFloat("inner angle", &innerAngle, 1.0f, 0.0f, 90.0f))
					spotLight.UseInnerAngle(innerAngle);

				ImGui::ColorEdit3("ambient color", &spotLight.AmbientColor[0],   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("diffuse color", &spotLight.DiffuseColor[0],   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("specular color", &spotLight.SpecularColor[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);

				spotLight.AmbientColor  = VectorMax(spotLight.AmbientColor,  MakeVector3(0.0f));
				spotLight.DiffuseColor  = VectorMax(spotLight.DiffuseColor,  MakeVector3(0.0f));
				spotLight.SpecularColor = VectorMax(spotLight.SpecularColor, MakeVector3(0.0f));

				ImGui::PopID();
			}
		}
	}
}