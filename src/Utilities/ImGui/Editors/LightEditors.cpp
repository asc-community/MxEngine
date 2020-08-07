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

#include "Core/Components/Lighting/DirectionalLight.h"
#include "Core/Components/Lighting/PointLight.h"
#include "Core/Components/Lighting/SpotLight.h"

#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	void DirectionalLightEditor(DirectionalLight& dirLight)
	{
		TREE_NODE_PUSH("DirectionalLight");
		REMOVE_COMPONENT_BUTTON(dirLight);

		DrawLightBaseEditor(dirLight);
		ImGui::DragFloat("projection size", &dirLight.ProjectionSize, 1.0f, 0.0f, 10000.0f);
		ImGui::DragFloat3("direction", &dirLight.Direction[0], 0.01f);
		if (ImGui::Button("follow viewport"))
			dirLight.FollowViewport();

		auto texture = dirLight.GetDepthTexture();
		static int depthMapSize = (int)texture->GetWidth();
		if (GUI::InputIntOnClick("depth map size", &depthMapSize))
			texture->LoadDepth(depthMapSize, depthMapSize);

		DrawTextureEditor("depth map", texture, false);
	}

	void PointLightEditor(PointLight& pointLight)
	{
		TREE_NODE_PUSH("PointLight");
		REMOVE_COMPONENT_BUTTON(pointLight);

		DrawLightBaseEditor(pointLight);
		auto factors = pointLight.GetFactors();
		if (ImGui::DragFloat3("factors", &factors[0], 0.01f, 0.0f, 10000.0f))
			pointLight.UseFactors(factors);

		auto cubemap = pointLight.GetDepthCubeMap();
		static int depthMapSize = (int)cubemap->GetWidth();
		if (GUI::InputIntOnClick("depth map size", &depthMapSize))
			cubemap->LoadDepth(depthMapSize, depthMapSize);

		DrawCubeMapEditor("depth map", cubemap);
	}

	void SpotLightEditor(SpotLight& spotLight)
	{
		TREE_NODE_PUSH("SpotLight");
		REMOVE_COMPONENT_BUTTON(spotLight);

		DrawLightBaseEditor(spotLight);

		auto innerAngle = spotLight.GetInnerAngle();
		auto outerAngle = spotLight.GetOuterAngle();

		ImGui::DragFloat3("direction", &spotLight.Direction[0], 0.01f);

		if (ImGui::DragFloat("outer angle", &outerAngle))
			spotLight.UseOuterAngle(outerAngle);
		if (ImGui::DragFloat("inner angle", &innerAngle))
			spotLight.UseInnerAngle(innerAngle);

		auto texture = spotLight.GetDepthTexture();
		static int depthMapSize = (int)texture->GetWidth();
		if (GUI::InputIntOnClick("depth map size", &depthMapSize))
			texture->LoadDepth(depthMapSize, depthMapSize);

		DrawTextureEditor("depth map", texture, false);
	}
}