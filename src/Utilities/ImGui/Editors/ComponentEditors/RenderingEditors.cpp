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

#include "Core/Components/Rendering/DebugDraw.h"
#include "Core/Components/Rendering/MeshLOD.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Utilities/FileSystem/FileManager.h"

#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	void SkyboxEditor(Skybox& skybox)
	{
		TREE_NODE_PUSH("Skybox");
		REMOVE_COMPONENT_BUTTON(skybox);

		auto rotation = DegreesVec(skybox.GetEulerRotation());
		auto newRotation = rotation;
		if (ImGui::DragFloat("rotate x", &newRotation.x))
			skybox.RotateX(newRotation.x - rotation.x);
		if (ImGui::DragFloat("rotate y", &newRotation.y))
			skybox.RotateY(newRotation.y - rotation.y);
		if (ImGui::DragFloat("rotate z", &newRotation.z))
			skybox.RotateZ(newRotation.z - rotation.z);

		DrawCubeMapEditor("cubemap", skybox.Texture);
	}

	void DebugDrawEditor(DebugDraw& debugDraw)
	{
		TREE_NODE_PUSH("DebugDraw");
		REMOVE_COMPONENT_BUTTON(debugDraw);
		ImGui::Checkbox("draw physics collider", &debugDraw.RenderPhysicsCollider);
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

		if (ImGui::Button("load from file"))
		{
			MxString path = FileManager::OpenFileDialog();
			if (!path.empty() && File::Exists(path))
				meshRenderer = AssetManager::LoadMaterials(path);


		}

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
			auto name = "LOD level #" + ToMxString(i + 1);
			ImGui::DragFloat(name.c_str(), &config.Factors[i], 0.01f, 0.0f, 1.0f);
		}

		if (ImGui::Button("generate LODs"))
			meshLOD.Generate(config);

		ImGui::SameLine();
		ImGui::Checkbox("auto LOD selection", &meshLOD.AutoLODSelection);

		int currentLOD = (int)meshLOD.CurrentLOD;
		if (ImGui::DragInt("current LOD", &currentLOD, 0.1f, 0, (int)meshLOD.LODs.size()))
			meshLOD.CurrentLOD = (MeshLOD::LODIndex)Min(currentLOD, meshLOD.LODs.size());

		for (auto& lod : meshLOD.LODs)
		{
			ImGui::PushID(id++);
			auto name = "lod #" + ToMxString(id);
			DrawMeshEditor(name.c_str(), lod);
			ImGui::PopID();
		}
	}
}