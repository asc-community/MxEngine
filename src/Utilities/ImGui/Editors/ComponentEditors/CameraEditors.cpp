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

#include "Core/Components/Camera/CameraController.h"
#include "Core/Components/Camera/CameraEffects.h"
#include "Core/Components/Camera/CameraSSR.h"
#include "Core/Components/Camera/CameraToneMapping.h"
#include "Core/Components/Camera/InputController.h"
#include "Core/Components/Camera/VRCameraController.h"
#include "Core/Components/Camera/FrustrumCamera.h"
#include "Core/Components/Camera/OrthographicCamera.h"
#include "Core/Components/Camera/PerspectiveCamera.h"
#include "Utilities/ImGui/Editors/ComponentEditors/GenericComponentEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Core/Application/Rendering.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	void CameraEffectsEditor(CameraEffects& cameraEffects)
	{
		TREE_NODE_PUSH("CameraEffects");
		REMOVE_COMPONENT_BUTTON(cameraEffects);

		int bloomIterations = (int)cameraEffects.GetBloomIterations();
		float bloomWeight = cameraEffects.GetBloomWeight();

		float vignetteRadius = cameraEffects.GetVignetteRadius();
		float vignetteIntensity = cameraEffects.GetVignetteIntensity();

		float chromaticAbberationMinDistance = cameraEffects.GetChromaticAberrationMinDistance();
		float chromaticAbberationIntensity = cameraEffects.GetChromaticAberrationIntensity();
		float chromaticAbberationDistortion = cameraEffects.GetChromaticAberrationDistortion();

		float ambientOcclusionRadius = cameraEffects.GetAmbientOcclusionRadius();
		float ambientOcclusionIntensity = cameraEffects.GetAmbientOcclusionIntensity();
		int ambientOcclusionSamples = (int)cameraEffects.GetAmbientOcclusionSamples();

		bool isFXAAEnabled = cameraEffects.IsFXAAEnabled();

		Vector3 fogColor = cameraEffects.GetFogColor();
		float fogDensity = cameraEffects.GetFogDensity();
		float fogDistance = cameraEffects.GetFogDistance();

		if (ImGui::TreeNode("fog"))
		{
			if (ImGui::ColorEdit3("fog color", &fogColor[0]))
				cameraEffects.SetFogColor(fogColor);

			if (ImGui::DragFloat("fog density", &fogDensity, 0.001f))
				cameraEffects.SetFogDensity(fogDensity);

			if (ImGui::DragFloat("fog distance", &fogDistance, 0.01f))
				cameraEffects.SetFogDistance(fogDistance);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("bloom"))
		{
			if (ImGui::DragFloat("weight", &bloomWeight, 0.1f))
				cameraEffects.SetBloomWeight(bloomWeight);
			if (ImGui::DragInt("iterations", &bloomIterations))
				cameraEffects.SetBloomIterations((size_t)Max(0, bloomIterations));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("vignette"))
		{
			if (ImGui::DragFloat("radius", &vignetteRadius, 0.01f))
				cameraEffects.SetVignetteRadius(vignetteRadius);
			if (ImGui::DragFloat("intensity", &vignetteIntensity, 0.1f))
				cameraEffects.SetVignetteIntensity(vignetteIntensity);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("chromatic abberation"))
		{
			if (ImGui::DragFloat("min distance", &chromaticAbberationMinDistance, 0.01f))
				cameraEffects.SetChromaticAberrationMinDistance(chromaticAbberationMinDistance);
			if (ImGui::DragFloat("intensity", &chromaticAbberationIntensity, 0.01f))
				cameraEffects.SetChromaticAberrationIntensity(chromaticAbberationIntensity);
			if (ImGui::DragFloat("distortion", &chromaticAbberationDistortion, 0.01f))
				cameraEffects.SetChromaticAberrationDistortion(chromaticAbberationDistortion);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ambient occlusion"))
		{
			if (ImGui::DragFloat("radius", &ambientOcclusionRadius, 0.01f))
				cameraEffects.SetAmbientOcclusionRadius(ambientOcclusionRadius);
			if (ImGui::DragFloat("intensity", &ambientOcclusionIntensity, 0.01f))
				cameraEffects.SetAmbientOcclusionIntensity(ambientOcclusionIntensity);
			if (ImGui::DragInt("samples", &ambientOcclusionSamples))
				cameraEffects.SetAmbientOcclusionSamples((size_t)Max(ambientOcclusionSamples, 0));

			ImGui::TreePop();
		}
		
		if (ImGui::Checkbox("uses FXAA", &isFXAAEnabled))
			cameraEffects.ToggleFXAA(isFXAAEnabled);
	}

	void CameraToneMappingEditor(CameraToneMapping& cameraToneMapping)
	{
		TREE_NODE_PUSH("CameraToneMapping");
		REMOVE_COMPONENT_BUTTON(cameraToneMapping);

		float exposure		  = cameraToneMapping.GetExposure();
		float colorScale	  = cameraToneMapping.GetColorScale();
		float whitePoint	  = cameraToneMapping.GetWhitePoint();
		float minLuminance	  = cameraToneMapping.GetMinLuminance();
		float maxLuminance	  = cameraToneMapping.GetMaxLuminance();
		auto channels		  = cameraToneMapping.GetColorGrading();
		ACES acesCoefficients = cameraToneMapping.GetACESCoefficients();

		float gamma               = cameraToneMapping.GetGamma();
		float adaptationSpeed     = cameraToneMapping.GetEyeAdaptationSpeed();
		float adaptationThreshold = cameraToneMapping.GetEyeAdaptationThreshold();

		if (ImGui::TreeNode("luminance"))
		{
			if (ImGui::DragFloat("white point", &whitePoint, 0.01f))
				cameraToneMapping.SetWhitePoint(whitePoint);
			if (ImGui::DragFloat("min luminance", &minLuminance, 0.1f))
				cameraToneMapping.SetMinLuminance(minLuminance);
			if (ImGui::DragFloat("max luminance", &maxLuminance, 0.1f))
				cameraToneMapping.SetMaxLuminance(maxLuminance);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ACES curve"))
		{
			ImGui::DragFloat("A", &acesCoefficients.A, 0.01f, 0.0f, FLT_MAX);
			ImGui::DragFloat("B", &acesCoefficients.B, 0.01f, 0.0f, FLT_MAX);
			ImGui::DragFloat("C", &acesCoefficients.C, 0.01f, 0.0f, FLT_MAX);
			ImGui::DragFloat("D", &acesCoefficients.D, 0.01f, 0.0f, FLT_MAX);
			ImGui::DragFloat("E", &acesCoefficients.E, 0.01f, 0.0f, FLT_MAX);
			ImGui::DragFloat("F", &acesCoefficients.F, 0.01f, 0.0f, FLT_MAX);

			cameraToneMapping.SetACESCoefficients(acesCoefficients);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("exposure"))
		{
			if (ImGui::DragFloat("exposure", &exposure, 0.01f))
				cameraToneMapping.SetExposure(exposure);
			if (ImGui::DragFloat("color scale", &colorScale, 0.01f))
				cameraToneMapping.SetColorScale(colorScale);
			if (ImGui::DragFloat("gamma", &gamma, 0.01f, 0.0f, 5.0f))
				cameraToneMapping.SetGamma(gamma);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("eye adaptation"))
		{
			if (ImGui::DragFloat("speed", &adaptationSpeed, 0.01f))
				cameraToneMapping.SetEyeAdaptationSpeed(adaptationSpeed);
			if (ImGui::DragFloat("threshold", &adaptationThreshold, 0.01f))
				cameraToneMapping.SetEyeAdaptationThreshold(adaptationThreshold);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("color grading"))
		{
			if (ImGui::ColorEdit3("channel R", &channels.R[0]))
				cameraToneMapping.SetColorGrading(channels);
			if (ImGui::ColorEdit3("channel G", &channels.G[0]))
				cameraToneMapping.SetColorGrading(channels);
			if (ImGui::ColorEdit3("channel B", &channels.B[0]))
				cameraToneMapping.SetColorGrading(channels);

			ImGui::TreePop();
		}
	}

	void CameraSSREditor(CameraSSR& cameraSSR)
	{
		TREE_NODE_PUSH("CameraSSR");
		REMOVE_COMPONENT_BUTTON(cameraSSR);

		float ssrThickness = cameraSSR.GetThickness();
		float ssrMaxCosAngle = cameraSSR.GetMaxCosAngle();
		int ssrSteps = (int)cameraSSR.GetSteps();
		float ssrMaxDistance = cameraSSR.GetMaxDistance();
		float startDistance = cameraSSR.GetStartDistance();
		float fading = cameraSSR.GetFading();

		if (ImGui::DragFloat("thickness", &ssrThickness, 0.1f))
			cameraSSR.SetThickness(ssrThickness);
		if (ImGui::DragFloat("max angle cos", &ssrMaxCosAngle, 0.01f))
			cameraSSR.SetMaxCosAngle(ssrMaxCosAngle);
		if (ImGui::DragInt("steps", &ssrSteps, 0.1f))
			cameraSSR.SetSteps((size_t)Clamp(ssrSteps, 0, 1000));
		if (ImGui::DragFloat("max distance", &ssrMaxDistance, 0.1f))
			cameraSSR.SetMaxDistance(ssrMaxDistance);
		if (ImGui::DragFloat("start distance", &startDistance, 0.1f))
			cameraSSR.SetStartDistance(startDistance);
		if (ImGui::DragFloat("fading", &fading, 0.01f))
			cameraSSR.SetFading(fading);
	}

	void CameraControllerEditor(CameraController& cameraController)
	{
		ComponentEditor(cameraController);
	}

	void VRCameraControllerEditor(VRCameraController& vrCameraController)
	{
		TREE_NODE_PUSH("VRCameraController");
		REMOVE_COMPONENT_BUTTON(vrCameraController);

		ImGui::DragFloat("eye distance", &vrCameraController.EyeDistance, 0.001f, 0.0f, 10.0f);
		ImGui::DragFloat("eye focus distance", &vrCameraController.FocusDistance, 0.01f, 0.1f, 100.0f);

		if (ImGui::TreeNode("left eye"))
		{
			if (vrCameraController.LeftEye.IsValid())
				CameraControllerEditor(*vrCameraController.LeftEye);
			else
				ImGui::Text("- no camera attached");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("right eye"))
		{
			if (vrCameraController.RightEye.IsValid())
				CameraControllerEditor(*vrCameraController.RightEye);
			else
				ImGui::Text("- no camera attached");
			ImGui::TreePop();
		}
	}

	void InputControllerEditor(InputController& inputControl)
	{
		TREE_NODE_PUSH("InputController");
		REMOVE_COMPONENT_BUTTON(inputControl);

		// TODO: custom keys input
		if (ImGui::Button("bind movement WASD"))
			inputControl.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D);

		if (ImGui::Button("bind movement ESDF"))
			inputControl.BindMovement(KeyCode::E, KeyCode::S, KeyCode::D, KeyCode::F);

		if (ImGui::Button("bind movement QWES"))
			inputControl.BindMovement(KeyCode::Q, KeyCode::W, KeyCode::E, KeyCode::S);

		if (ImGui::Button("bind rotation"))
			inputControl.BindRotation();

		if (ImGui::Button("bind horizontal rotation"))
			inputControl.BindHorizontalRotation();

		if (ImGui::Button("bind vertical rotation"))
			inputControl.BindVerticalRotation();

		if (ImGui::Button("bind movement WASD SPACE/LSHIFT"))
			inputControl.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);

		if (ImGui::Button("bind movement ESDF SPACE/LSHIFT"))
			inputControl.BindMovement(KeyCode::E, KeyCode::S, KeyCode::D, KeyCode::F, KeyCode::SPACE, KeyCode::LEFT_SHIFT);

		if (ImGui::Button("bind movement QWES SPACE/LSHIFT"))
			inputControl.BindMovement(KeyCode::Q, KeyCode::W, KeyCode::E, KeyCode::S, KeyCode::SPACE, KeyCode::LEFT_SHIFT);

		if (ImGui::Button("unbind all"))
		{
			auto& object = MxObject::GetByComponent(inputControl);
			object.RemoveComponent<InputController>();
			object.AddComponent<InputController>();
		}
	}
}