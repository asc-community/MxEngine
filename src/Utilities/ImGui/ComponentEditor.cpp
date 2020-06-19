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

#include "ComponentEditor.h"
#include "ImGuiUtils.h"
#include "Core/Components/Camera/PerspectiveCamera.h"
#include "Core/Components/Camera/OrthographicCamera.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

    void TransformEditor(Transform& transform)
    {
		SCOPE_TREE_NODE("Transform");
		// note that Transform component cannot be removed

		// position
		auto position = transform.GetPosition();
		if (ImGui::DragFloat3("position", &position[0], 0.5f))
			transform.SetPosition(position);

		// rotation (euler)
		auto rotation = DegreesVec(transform.GetEulerRotation());
		auto newRotation = rotation;
		if (ImGui::DragFloat("rotate x", &newRotation.x))
			transform.RotateX(newRotation.x - rotation.x);
		if (ImGui::DragFloat("rotate y", &newRotation.y))
			transform.RotateY(newRotation.y - rotation.y);
		if (ImGui::DragFloat("rotate z", &newRotation.z))
			transform.RotateZ(newRotation.z - rotation.z);

		// scale
		auto scale = transform.GetScale();
		if (ImGui::DragFloat3("scale", &scale[0], 0.01f, 0.01f, 1000.0f))
			transform.SetScale(scale);
    }

	void BehaviourEditor(Behaviour& behaviour)
	{
		SCOPE_TREE_NODE("Behaviour");
		REMOVE_COMPONENT_BUTTON(behaviour);
		ImGui::Text("has update callback: %s", BOOL_STRING(behaviour.UpdateCallback));
	}

	// TODO: runtime editing of script file
	// TODO: last time of file editing
	void ScriptEditor(Script& script)
	{
		SCOPE_TREE_NODE("Script");
		REMOVE_COMPONENT_BUTTON(script);
		ImGui::Text("path: %s", script.GetFilePath().c_str());
		ImGui::LabelText("contents", script.GetContent().c_str());
	}

	void SkyboxEditor(Skybox& skybox)
	{
		SCOPE_TREE_NODE("Skybox");
		REMOVE_COMPONENT_BUTTON(skybox);
		DrawCubeMapEditor("cubemap", skybox.Texture);
	}

    void MeshRendererEditor(MeshRenderer& meshRenderer)
    {
		SCOPE_TREE_NODE("MeshRenderer");
		// note that MeshRenderer component cannot be removed

		int id = 0;
		for (auto& material : meshRenderer.Materials)
		{
			ImGui::PushID(id++);
			DrawMaterialEditor(material);
			ImGui::PopID();
		}
    }

	void MeshSourceEditor(MeshSource& meshSource)
	{
		SCOPE_TREE_NODE("MeshSource");
		REMOVE_COMPONENT_BUTTON(meshSource);
		auto mesh = meshSource.GetMesh();
		DrawMeshEditor("mesh", mesh);
	}

	void MeshLODEditor(MeshLOD& meshLOD)
	{
		SCOPE_TREE_NODE("MeshLOD");
		REMOVE_COMPONENT_BUTTON(meshLOD);
		int id = 0;
		static LODConfig config;
		
		for (size_t i = 0; i < config.Factors.size(); i++)
		{
			auto name = MxFormat("LOD level #{0}", i + 1);
			ImGui::DragFloat(name.c_str(), &config.Factors[i], 0.01f, 0.0f, 1.0f);
		}

		if(ImGui::Button("generate LODs"))
			meshLOD.Generate();

		for (auto& lod : meshLOD.LODs)
		{
			auto name = MxFormat("lod #{0}", id);
			ImGui::PushID(id++);
			DrawMeshEditor(name.c_str(), lod);
			ImGui::PopID();
		}
	}

	void DirectionalLightEditor(DirectionalLight& dirLight)
	{
		SCOPE_TREE_NODE("Directional Light");
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

		DrawTextureEditor("depth map", texture);
	}

	void PointLightEditor(PointLight& pointLight)
	{
		SCOPE_TREE_NODE("Point Ligth");
		REMOVE_COMPONENT_BUTTON(pointLight);

		DrawLightBaseEditor(pointLight);
		auto factors = pointLight.GetFactors();
		if(ImGui::DragFloat3("factors", &factors[0], 0.01f, 0.0f, 10000.0f))
			pointLight.UseFactors(factors);

		auto cubemap = pointLight.GetDepthCubeMap();
		static int depthMapSize = (int)cubemap->GetWidth();
		if (GUI::InputIntOnClick("depth map size", &depthMapSize))
			cubemap->LoadDepth(depthMapSize, depthMapSize);

		DrawCubeMapEditor("depth map", cubemap);
	}

	void SpotLightEditor(SpotLight& spotLight)
	{
		SCOPE_TREE_NODE("Spot Ligth");
		REMOVE_COMPONENT_BUTTON(spotLight);

		DrawLightBaseEditor(spotLight);

		auto innerAngle = spotLight.GetInnerAngle();
		auto outerAngle = spotLight.GetOuterAngle();

		ImGui::DragFloat3("direction", &spotLight.Direction[0], 0.01f);

		if (ImGui::DragFloat("outer angle", &outerAngle, 1.0f, 0.0f, 90.0f))
			spotLight.UseOuterAngle(outerAngle);
		if (ImGui::DragFloat("inner angle", &innerAngle, 1.0f, 0.0f, 90.0f))
			spotLight.UseInnerAngle(innerAngle);

		auto texture = spotLight.GetDepthTexture();
		static int depthMapSize = (int)texture->GetWidth();
		if (GUI::InputIntOnClick("depth map size", &depthMapSize))
			texture->LoadDepth(depthMapSize, depthMapSize);

		DrawTextureEditor("depth map", texture);
	}

	void CameraControllerEditor(CameraController& camera)
	{
		SCOPE_TREE_NODE("Camera Controller");
		REMOVE_COMPONENT_BUTTON(camera);

		if (!camera.HasCamera())
		{
			ImGui::Text("no camera attached");
			if (ImGui::Button("attach perspective camera"))
				camera.SetCamera(MakeUnique<PerspectiveCamera>());

			ImGui::SameLine();

			if (ImGui::Button("attach orthographic camera"))
				camera.SetCamera(MakeUnique<OrthographicCamera>());

			return;
		}

		int bloomIterations = (int)camera.GetBloomIterations();
		float exposure = camera.GetExposure();
		float bloomWeight = camera.GetBloomWeight();
		auto direction = camera.GetDirection();
		float moveSpeed = camera.GetMoveSpeed();
		float rotateSpeed = camera.GetRotateSpeed();
		float zoom = camera.GetZoom();

		if (ImGui::DragFloat("move speed", &moveSpeed))
			camera.SetMoveSpeed(moveSpeed);
		if(ImGui::DragFloat("rotate speed", &rotateSpeed, 0.01f))
			camera.SetRotateSpeed(rotateSpeed);
		if (ImGui::DragFloat("zoom", &zoom, 0.01f))
			camera.SetZoom(zoom);
		if (ImGui::DragFloat3("direction", &direction[0], 0.01f))
			camera.SetDirection(direction);
		if (ImGui::DragFloat("exposure", &exposure, 0.1f))
			camera.SetExposure(exposure);
		if (ImGui::DragFloat("bloom weight", &bloomWeight, 0.1f))
			camera.SetBloomWeight(bloomWeight);
		if (ImGui::DragInt("bloom iterations", &bloomIterations))
			camera.SetBloomIterations((size_t)Max(0, bloomIterations));

		if (ImGui::Button("auto-resize viewport"))
			camera.FitScreenViewport();

		ImGui::SameLine();
		if (ImGui::Button("detach camera"))
		{
			camera.SetCamera(nullptr);
			return;
		}

		auto texture = camera.GetTexture();
		DrawTextureEditor("output texture", texture);

		{
			SCOPE_TREE_NODE("attached camera");
			ImGui::PushID(0xFFFF);

			auto forward = camera.GetForwardVector();
			auto right = camera.GetRightVector();
			auto up = camera.GetUpVector();

			if (ImGui::DragFloat3("forward vec", &forward[0], 0.01f))
				camera.SetForwardVector(forward);
			if (ImGui::DragFloat3("right vec", &right[0], 0.01f))
				camera.SetRightVector(right);
			if (ImGui::DragFloat3("up vec", &up[0], 0.01f))
				camera.SetUpVector(up);

			auto& attachedCamera = camera.GetCamera();
			float aspect = attachedCamera.GetAspectRatio();
			float zfar = attachedCamera.GetZFar();
			float znear = attachedCamera.GetZNear();
			float zoom = attachedCamera.GetZoom();

			ImGui::Text((attachedCamera.IsPerspective() ? "camera type: perspective" : "camera type: orthographic"));

			if (ImGui::DragFloat("aspect ratio", &aspect, 0.01f))
				attachedCamera.SetAspectRatio(aspect);
			if (ImGui::DragFloat("Z far", &zfar))
				attachedCamera.SetZFar(zfar);
			if (ImGui::DragFloat("Z near", &znear, 0.001f))
				attachedCamera.SetZNear(znear);
			if (ImGui::DragFloat("zoom", &zoom, 0.01f))
				attachedCamera.SetZoom(zoom);

			ImGui::PopID();
		}
	}
}