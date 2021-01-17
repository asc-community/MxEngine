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
#include "Utilities/ImGui/Editors/ComponentEditors/GenericComponentEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	void SkyboxEditor(Skybox& skybox)
	{
		ComponentEditor(skybox);
	}

	void DebugDrawEditor(DebugDraw& debugDraw)
	{
		ComponentEditor(debugDraw);
	}

	void LoadMaterialsFileDialog(const rttr::instance& parent)
	{
		if (ImGui::Button("load from file"))
		{
			auto& meshRenderer = *parent.try_convert<MeshRenderer>();
			MxString path = FileManager::OpenFileDialog();
			if (!path.empty() && File::Exists(path))
				meshRenderer = AssetManager::LoadMaterials(path);
		}
	}

	void MeshRendererEditor(MeshRenderer& meshRenderer)
	{
		ComponentEditor(meshRenderer);
	}

	void MeshSourceEditor(MeshSource& meshSource)
	{
		TREE_NODE_PUSH("MeshSource");
		REMOVE_COMPONENT_BUTTON(meshSource);

		ImGui::Checkbox("is drawn", &meshSource.IsDrawn);
		ImGui::SameLine();
		ImGui::Checkbox("casts shadow", &meshSource.CastsShadow);
		ImGui::SameLine();
		if (ImGui::Button("load from file"))
		{
			MxString path = FileManager::OpenFileDialog();
			if (!path.empty() && File::Exists(path))
				meshSource.Mesh = AssetManager::LoadMesh(path);
		}
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