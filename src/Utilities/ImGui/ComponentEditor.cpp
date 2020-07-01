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
#include "Core/Components/Camera/FrustrumCamera.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }
	
	struct TreeNodeAutoPop { ~TreeNodeAutoPop() { ImGui::TreePop(); } };
	#define TREE_NODE_PUSH(name) if(!ImGui::TreeNode(name)) return; TreeNodeAutoPop _pop_

    void TransformEditor(Transform& transform)
    {
		TREE_NODE_PUSH("Transform");
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

		static auto lookPoint = MakeVector3(0.0f);
		ImGui::DragFloat3("look at", &lookPoint[0], 0.01f);

		if (ImGui::Button("look at xyz"))
			transform.LookAt(lookPoint);
		ImGui::SameLine();
		if (ImGui::Button("look at xy"))
			transform.LookAtXY(lookPoint);
		ImGui::SameLine();
		if (ImGui::Button("look at xz"))
			transform.LookAtXZ(lookPoint);
		ImGui::SameLine();
		if (ImGui::Button("look at yz"))
			transform.LookAtYZ(lookPoint);
    }

	void BehaviourEditor(Behaviour& behaviour)
	{
		TREE_NODE_PUSH("Behaviour");
		REMOVE_COMPONENT_BUTTON(behaviour);
		ImGui::Text("has update callback: %s", BOOL_STRING(behaviour.UpdateCallback));
	}

	// TODO: runtime editing of script file
	// TODO: last time of file editing
	void ScriptEditor(Script& script)
	{
		TREE_NODE_PUSH("Script");
		REMOVE_COMPONENT_BUTTON(script);
		ImGui::Text("path: %s", script.GetFilePath().c_str());
		ImGui::LabelText("contents", script.GetContent().c_str());
	}

	void InstanceFactoryEditor(InstanceFactory& instanceFactory)
	{
		TREE_NODE_PUSH("InstanceFactory");
		REMOVE_COMPONENT_BUTTON(instanceFactory);
		
		ImGui::Text("instance count: %d", (int)instanceFactory.GetCount());
		
		ImGui::SameLine();
		if (ImGui::Button("instanciate"))
			instanceFactory.MakeInstance().MakeStatic();
		ImGui::SameLine();
		if (ImGui::Button("destroy all"))
			instanceFactory.GetInstancePool() = { };

		int id = 0;
		auto begin = instanceFactory.GetInstancePool().begin();
		auto end = instanceFactory.GetInstancePool().end();
		for (auto it = begin; it != end; it++)
		{
			ImGui::PushID(id);
			if (ImGui::Button("-"))
			{
				instanceFactory.GetInstancePool().Deallocate(it);
			}
			else
			{
				MxString nodeName = MxFormat("instance #{0}", id++);
				ImGui::SameLine();
				if (ImGui::CollapsingHeader(nodeName.c_str()))
				{
					GUI::Indent _(30.0f);

					auto& instance = it->value;
					auto color = instance.GetColor();

					TransformEditor(instance.Transform);
					if (ImGui::ColorEdit3("base color", &color[0], ImGuiColorEditFlags_HDR))
						instance.SetColor(color);
				}
			}
			ImGui::PopID();
		}
	}

	void SkyboxEditor(Skybox& skybox)
	{
		TREE_NODE_PUSH("Skybox");
		REMOVE_COMPONENT_BUTTON(skybox);
		DrawCubeMapEditor("cubemap", skybox.Texture);
	}

	void DebugDrawEditor(DebugDraw& debugDraw)
	{
		TREE_NODE_PUSH("DebugDraw");
		REMOVE_COMPONENT_BUTTON(debugDraw);
		ImGui::Checkbox("draw bounding box (AABB)", &debugDraw.RenderBoundingBox);
		ImGui::Checkbox("draw bounding sphere", &debugDraw.RenderBoundingSphere);
		ImGui::Checkbox("draw light bounds", &debugDraw.RenderLightingBounds);
		ImGui::Checkbox("draw sound bounds", &debugDraw.RenderSoundBounds);
		ImGui::Checkbox("draw frustrum bounds", &debugDraw.RenderFrustrumBounds);
		ImGui::ColorEdit4("bounding box color", &debugDraw.BoundingBoxColor[0], ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("bounding sphere color", &debugDraw.BoundingSphereColor[0], ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("light source color", &debugDraw.LightSourceColor[0], ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("sound source color", &debugDraw.SoundSourceColor[0], ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("frustrum color", &debugDraw.FrustrumColor[0], ImGuiColorEditFlags_AlphaBar);
	}

    void MeshRendererEditor(MeshRenderer& meshRenderer)
    {
		TREE_NODE_PUSH("MeshRenderer");
		REMOVE_COMPONENT_BUTTON(meshRenderer);

		static MxString path;
		if (GUI::InputTextOnClick("", path, 128, "load materials"))
			meshRenderer.Materials = AssetManager::LoadMaterials(path);

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
		TREE_NODE_PUSH("MeshSource");
		REMOVE_COMPONENT_BUTTON(meshSource);

		ImGui::Checkbox("is drawn", &meshSource.IsDrawn);
		DrawMeshEditor("mesh", meshSource.Mesh);
	}

	void MeshLODEditor(MeshLOD& meshLOD)
	{
		TREE_NODE_PUSH("MeshLOD");
		REMOVE_COMPONENT_BUTTON(meshLOD);
		int id = 0;
		static LODConfig config;
		
		for (size_t i = 0; i < config.Factors.size(); i++)
		{
			auto name = MxFormat("LOD level #{0}", i + 1);
			ImGui::DragFloat(name.c_str(), &config.Factors[i], 0.01f, 0.0f, 1.0f);
		}

		if(ImGui::Button("generate LODs"))
			meshLOD.Generate(config);

		ImGui::SameLine();
		ImGui::Checkbox("auto LOD selection", &meshLOD.AutoLODSelection);

		int currentLOD = (int)meshLOD.CurrentLOD;
		if (ImGui::DragInt("current LOD", &currentLOD, 0.1f, 0, (int)meshLOD.LODs.size()))
			meshLOD.CurrentLOD = (MeshLOD::LODIndex)Min(currentLOD, meshLOD.LODs.size());

		for (auto& lod : meshLOD.LODs)
		{
			ImGui::PushID(id++);
			auto name = MxFormat("lod #{0}", id);
			DrawMeshEditor(name.c_str(), lod);
			ImGui::PopID();
		}
	}

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

		DrawTextureEditor("depth map", texture);
	}

	void PointLightEditor(PointLight& pointLight)
	{
		TREE_NODE_PUSH("PointLight");
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

		DrawTextureEditor("depth map", texture);
	}

	void CameraControllerEditor(CameraController& cameraController)
	{
		TREE_NODE_PUSH("CameraController");
		REMOVE_COMPONENT_BUTTON(cameraController);
			
		if (ImGui::BeginCombo("camera type", "perspective"))
		{
			bool perspective = cameraController.GetCameraType() == CameraType::PERSPECTIVE;
			bool orthographic = cameraController.GetCameraType() == CameraType::ORTHOGRAPHIC;
			bool frustrum = cameraController.GetCameraType() == CameraType::FRUSTRUM;
			if (ImGui::Selectable("perspective", &perspective))
			{
				cameraController.SetCameraType(CameraType::PERSPECTIVE);
				ImGui::SetItemDefaultFocus();
			}
			if (ImGui::Selectable("orthographic", &orthographic))
			{
				cameraController.SetCameraType(CameraType::ORTHOGRAPHIC);
				ImGui::SetItemDefaultFocus();
			}
			if (ImGui::Selectable("frustrum", &frustrum))
			{
				cameraController.SetCameraType(CameraType::FRUSTRUM);
				ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (cameraController.GetCameraType() == CameraType::PERSPECTIVE)
		{
			float fov = cameraController.GetCamera<PerspectiveCamera>().GetFOV();
			if (ImGui::DragFloat("fov", &fov, 0.3f, 1.0f, 179.0f))
				cameraController.GetCamera<PerspectiveCamera>().SetFOV(fov);
		}
		else if (cameraController.GetCameraType() == CameraType::ORTHOGRAPHIC)
		{
			auto size = cameraController.GetCamera<OrthographicCamera>().GetSize();
			if (ImGui::DragFloat("size", &size, 0.3f, 0.01f, 10000.0f))
				cameraController.GetCamera<OrthographicCamera>().SetSize(size);
		}
		else if (cameraController.GetCameraType() == CameraType::FRUSTRUM)
		{
			auto bounds = cameraController.GetCamera<FrustrumCamera>().GetBounds();
			if (ImGui::DragFloat2("center", &bounds[0], 0.01f))
				cameraController.GetCamera<FrustrumCamera>().SetBounds(bounds.x, bounds.y, bounds.z);
			if (ImGui::DragFloat("size", &bounds.z, 0.01f, 0.0f, 10000.0f))
				cameraController.GetCamera<FrustrumCamera>().SetBounds(bounds.x, bounds.y, bounds.z);
		}

		int bloomIterations = (int)cameraController.GetBloomIterations();
		float exposure = cameraController.GetExposure();
		float bloomWeight = cameraController.GetBloomWeight();
		auto direction = cameraController.GetDirection();
		float moveSpeed = cameraController.GetMoveSpeed();
		float rotateSpeed = cameraController.GetRotateSpeed();

		if (ImGui::DragFloat("move speed", &moveSpeed))
			cameraController.SetMoveSpeed(moveSpeed);
		if(ImGui::DragFloat("rotate speed", &rotateSpeed, 0.01f))
			cameraController.SetRotateSpeed(rotateSpeed);

		if (ImGui::DragFloat3("direction", &direction[0], 0.01f))
			cameraController.SetDirection(direction);
		if (ImGui::DragFloat("exposure", &exposure, 0.1f))
			cameraController.SetExposure(exposure);
		if (ImGui::DragFloat("bloom weight", &bloomWeight, 0.1f))
			cameraController.SetBloomWeight(bloomWeight);
		if (ImGui::DragInt("bloom iterations", &bloomIterations))
			cameraController.SetBloomIterations((size_t)Max(0, bloomIterations));
		
		bool isRendered = cameraController.IsRendered();
		if (ImGui::Checkbox("enable rendering", &isRendered))
			cameraController.ToggleRendering(isRendered);

		ImGui::SameLine();
		if (ImGui::Button("listen window resize"))
		{
			cameraController.ListenWindowResizeEvent();
		}
		ImGui::SameLine();
		if (ImGui::Button("set as main viewport"))
		{
			auto cameraComponent = MxObject::GetComponentHandle(cameraController);
			Application::Get()->GetRenderAdaptor().Viewport = cameraComponent;
		}

		auto texture = cameraController.GetRenderTexture();
		DrawTextureEditor("output texture", texture);

		{
			SCOPE_TREE_NODE("attached camera");
			ImGui::PushID(0xFFFF);

			auto forward = cameraController.GetForwardVector();
			auto right = cameraController.GetRightVector();
			auto up = cameraController.GetUpVector();

			if (ImGui::DragFloat3("forward vec", &forward[0], 0.01f))
				cameraController.SetForwardVector(forward);
			if (ImGui::DragFloat3("right vec", &right[0], 0.01f))
				cameraController.SetRightVector(right);
			if (ImGui::DragFloat3("up vec", &up[0], 0.01f))
				cameraController.SetUpVector(up);

			float aspect = cameraController.Camera.GetAspectRatio();
			float zfar = cameraController.Camera.GetZFar();
			float znear = cameraController.Camera.GetZNear();
			float zoom = cameraController.Camera.GetZoom();

			ImGui::Separator();
			if (ImGui::DragFloat("aspect ratio", &aspect, 0.01f, 0.01f, 10.0f))
				cameraController.Camera.SetAspectRatio(aspect);
			if (ImGui::DragFloat("Z far", &zfar, 1.0f, 0.01f, std::numeric_limits<float>::max()))
				cameraController.Camera.SetZFar(zfar);
			if (ImGui::DragFloat("Z near", &znear, 0.001f, 0.001f, 10000.0f))
				cameraController.Camera.SetZNear(znear);
			if (ImGui::DragFloat("zoom", &zoom, 0.01f, 0.01f, 10000.0f))
				cameraController.Camera.SetZoom(zoom);


			ImGui::PopID();
		}
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

	void InputControlEditor(InputControl& inputControl)
	{
		TREE_NODE_PUSH("InputControl");
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
			object.RemoveComponent<InputControl>();
			object.AddComponent<InputControl>();
		}
	}

	void AudioSourceEditor(AudioSource& audioSource)
	{
		TREE_NODE_PUSH("AudioSource");
		REMOVE_COMPONENT_BUTTON(audioSource);

		if (ImGui::TreeNode("source"))
		{
			auto source = audioSource.GetLoadedSource();
			static MxString path;
			if (GUI::InputTextOnClick(nullptr, path, 128, "load audio"))
				audioSource.Load(AssetManager::LoadAudio(path));


			if (!source.IsValid())
			{
				ImGui::Text("no audio source loaded");
			}
			else
			{
				ImGui::Text("native handle: %d", (int)source->GetNativeHandle());
				ImGui::Text("native format: %d", (int)source->GetNativeFormat());
				ImGui::Text("audio format: %s", EnumToString(source->GetAudioType()));
				ImGui::Text("channel count: %d", (int)source->GetChannelCount());
				ImGui::Text("length (in seconds): %d", int(source->GetSampleCount()) / (source->GetFrequency()));
				ImGui::Text("sample count: %d", (int)source->GetSampleCount());
				ImGui::Text("sampling frequency: %d", (int)source->GetFrequency());
				ImGui::Text("path to file: %s", source->GetFilePath().c_str());
			}							  
			ImGui::TreePop();
		}

		auto isLooping = audioSource.IsLooping();
		auto isPlaying = audioSource.IsPlaying();
		auto isRelative = audioSource.IsRelative();
		auto omnidirectional = audioSource.IsOmnidirectional();
		auto volume = audioSource.GetVolume();
		auto speed = audioSource.GetSpeed();
		auto coneVolume = audioSource.GetOuterAngleVolume();
		auto velocity = audioSource.GetVelocity();
		auto direction = audioSource.GetDirection();
		auto outerAngle = audioSource.GetOuterAngle();
		auto innerAngle = audioSource.GetInnerAngle();
		auto rollofFactor = audioSource.GetRollofFactor();
		auto referenceDistance = audioSource.GetReferenceDistance();
		

		if (ImGui::Checkbox("is looping", &isLooping))
			audioSource.SetLooping(isLooping);
		ImGui::SameLine();
		if (ImGui::Checkbox("is relative", &isRelative))
			audioSource.SetRelative(isRelative);

		ImGui::AlignTextToFramePadding();
		ImGui::Text("is omnidirectional: %s", BOOL_STRING(omnidirectional));
		ImGui::SameLine();
		if (ImGui::Button("make omnidirectional"))
			audioSource.MakeOmnidirectional();

		if (ImGui::Button("play"))
			audioSource.Play();
		ImGui::SameLine();
		if (ImGui::Button("reset"))
			audioSource.Reset();
		ImGui::SameLine();
		if (ImGui::Button("replay"))
			audioSource.Replay();
		ImGui::SameLine();
		if (ImGui::Button("stop"))
			audioSource.Stop();
		ImGui::SameLine();
		if (ImGui::Button("pause"))
			audioSource.Stop();

		if (ImGui::DragFloat3("direction", &direction[0], 0.01f))
			audioSource.SetDirection(direction);

		if (ImGui::DragFloat3("velocity", &velocity[0], 0.01f))
			audioSource.SetVelocity(velocity);

		if (ImGui::DragFloat("volume", &volume, 0.001f))
			audioSource.SetVolume(volume);

		if (ImGui::DragFloat("playback speed", &speed, 0.001f))
			audioSource.SetPlaybackSpeed(speed);

		if (ImGui::DragFloat("outer angle", &outerAngle))
			audioSource.SetOuterAngle(outerAngle);

		if (ImGui::DragFloat("inner angle", &innerAngle))
			audioSource.SetInnerAngle(innerAngle);

		if (ImGui::TreeNode("other settings"))
		{
			if (ImGui::DragFloat("outer angle volume", &coneVolume))
				audioSource.SetOuterAngleVolume(coneVolume);

			if (ImGui::DragFloat("rollof factor", &rollofFactor))
				audioSource.SetRollofFactor(rollofFactor);

			if (ImGui::DragFloat("reference distance", &referenceDistance))
				audioSource.SetReferenceDistance(referenceDistance);
			ImGui::TreePop();
		}
	}

	void AudioListenerEditor(AudioListener& audioListener)
	{
		TREE_NODE_PUSH("AudioListener");
		REMOVE_COMPONENT_BUTTON(audioListener);

		auto volume = audioListener.GetVolume();
		auto velocity = audioListener.GetVelocity();
		auto soundSpeed = audioListener.GetSoundSpeed();
		auto dopplerFactor = audioListener.GetDopplerFactor();

		if (ImGui::DragFloat("volume", &volume, 0.001f))
			audioListener.SetVolume(volume);

		if (ImGui::DragFloat3("velocity", &velocity[0]))
			audioListener.SetVelocity(velocity);

		if (ImGui::DragFloat("speed of sound", &soundSpeed))
			audioListener.SetSoundSpeed(soundSpeed);

		if (ImGui::DragFloat("doppler factor", &dopplerFactor))
			audioListener.SetDopplerFactor(dopplerFactor);
	}
}