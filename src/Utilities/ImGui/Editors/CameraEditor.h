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

#pragma once

#include "Utilities/ImGui/ImGuiBase.h"
#include "Core/Application/Application.h"
#include "Utilities/ImGui/Layout/Layout.h"

namespace MxEngine::GUI
{
	/*!
	gets application debug settings and draws them in GUI window
	includes debug AABB and bounding spheres, settings for overlays
	*/
	inline void DrawDebugMeshes()
	{
		auto context = Application::Get();

		static bool boundingBoxes = false;
		static bool boundingSpheres = false;
		static bool debugOverlay = false;
		static bool skyboxDraw = true;
		static Vector4 debugColor = MakeVector4(1.0f, 0.0f, 0.0f, 1.0f);

		ImGui::Checkbox("display AABB", &boundingBoxes);
		ImGui::SameLine(); ImGui::Checkbox("display spheres", &boundingSpheres);
		ImGui::SameLine(); ImGui::Checkbox("overlay debug meshes", &debugOverlay);
		ImGui::SameLine(); ImGui::Checkbox("draw skybox", &skyboxDraw);
		ImGui::ColorEdit4("debug mesh color", &debugColor[0]);

		context->ToggleDebugDraw(boundingBoxes, boundingSpheres, debugColor, debugOverlay);
		context->ToggleSkybox(skyboxDraw);
	}

	inline void DrawPostEffects()
	{
		auto& renderer = Application::Get()->GetRenderer();

		float exposure = renderer.GetHDRExposure();
		float bloomWeight = renderer.GetBloomWeight();
		int bloomIters = renderer.GetBloomIterations();

		GUI_TREE_NODE("post effects",
			if (ImGui::DragFloat("HDR exposure", &exposure, 0.01f, 0.0f, std::numeric_limits<float>::max()))
				renderer.SetHDRExposure(exposure);
			if (ImGui::DragFloat("bloom weight", &bloomWeight, 1.0f, 0.0f, std::numeric_limits<float>::max()))
				renderer.SetBloomWeight(bloomWeight);
			if (ImGui::DragInt("bloom iterations", &bloomIters, 0.1f, 0, 100))
				renderer.SetBloomIterations(bloomIters);

			// fog editor
			ImGui::ColorEdit3("fog color", &renderer.Fog.Color[0]);
			ImGui::DragFloat("fog density", &renderer.Fog.Density, 0.001f, 0.0f, std::numeric_limits<float>::max());
			ImGui::DragFloat("fog distance", &renderer.Fog.Distance, 0.01f, 0.0f, 1.0f);
		);
	}

	/*!
	gets camera information from currenly active scene and draws it in GUI window
	- camera position, aspect, speed and fov
	- HDR settings: bloom effect, exposure, bloom iterations
	- debug draw utilities
	- skybox settings
	*/
	inline void DrawCameraEditor()
	{
		auto context = Application::Get();
		auto& scene = context->GetCurrentScene();

		auto& camera = scene.Viewport;
		float speed = camera.GetMoveSpeed();
		float sensitivity = camera.GetRotateSpeed();
		float zoom = camera.GetZoom();
		Vector3 pos = camera.GetPosition();

		DrawDebugMeshes();

		DrawPostEffects();

		ImGui::Text("position: (%f, %f, %f)", pos.x, pos.y, pos.z);

		if (ImGui::InputFloat("speed", &speed))
			camera.SetMoveSpeed(speed);

		if (ImGui::InputFloat("sensitivity", &sensitivity))
			camera.SetRotateSpeed(sensitivity);

		if (ImGui::DragFloat3("position", &pos[0], 0.5f))
			camera.SetPosition(pos);

		ImGui::Text("zoom / fov: %f", zoom);
		if (ImGui::DragFloat("zoom", &zoom, 0.1f, 0.1f, 20.0f))
			camera.SetZoom(zoom);

		GUI_TREE_NODE("skybox",
			auto& skybox = context->GetCurrentScene().SceneSkybox;
			if (skybox != nullptr)
			{
				auto rotation = DegreesVec(skybox->GetRotation());
				auto newRotation = rotation;
				if (ImGui::DragFloat("skybox rotate x", &newRotation.x))
					skybox->RotateX(newRotation.x - rotation.x);
				if (ImGui::DragFloat("skybox rotate y", &newRotation.y))
					skybox->RotateY(newRotation.y - rotation.y);
				if (ImGui::DragFloat("skybox rotate z", &newRotation.z))
					skybox->RotateZ(newRotation.z - rotation.z);

				static MxString buf(128, '\0');
				if (GUI::InputTextOnClick("skybox texture", buf.data(), 128))
				{
					skybox->SkyboxTexture = GraphicFactory::Create<CubeMap>(buf);
				}
			}
		);
	}
}