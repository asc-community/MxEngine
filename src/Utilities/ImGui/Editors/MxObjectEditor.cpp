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

#include "MxObjectEditor.h"
#include "ComponentEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Core/BoundingObjects/BoundingBox.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Application/Rendering.h"
#include "Utilities/FileSystem/FileManager.h"

namespace MxEngine::GUI
{
	void CreateMxObjectFromModelFile()
	{
		MxString filepath = FileManager::OpenFileDialog();
		if (!filepath.empty() && File::Exists(filepath))
		{
			auto object = MxObject::Create();
			object->Name = ToMxString(ToFilePath(filepath).stem());
			auto meshSource = object->AddComponent<MeshSource>(AssetManager::LoadMesh(filepath));
			auto meshRenderer = object->AddComponent<MeshRenderer>(AssetManager::LoadMaterials(meshSource->Mesh->GetFilePath()));
		}
	}

	void DrawMxObjectEditor(
		const char* name,
		MxObject& object,
		bool withBoundingBox,
		const MxVector<const char*>& componentNames,
		MxVector<std::function<void(MxObject&)>>& componentAdderCallbacks,
		MxVector<std::function<void(MxObject&)>>& componentEditorCallbacks
	)
	{
		static MxString objectName;
		if (GUI::InputTextOnClick("object name", objectName, 48))
		{
			if (!objectName.empty()) object.Name = objectName;
			objectName.clear();
		}

		static int currentItem = 0;
		ImGui::PushID(0xFFAA);
		ImGui::Combo("", &currentItem, componentNames.data(), (int)componentNames.size());
		ImGui::PopID();
		ImGui::SameLine();
		if (ImGui::Button("add component"))
		{
			componentAdderCallbacks[(size_t)currentItem](object);
		}

		GUI::TransformEditor(object.Transform);

		for (size_t i = 0; i < componentEditorCallbacks.size(); i++)
		{
			componentEditorCallbacks[i](object);
		}

		if (withBoundingBox)
		{
			AABB aabb;
			auto meshSource = object.GetComponent<MeshSource>();
			if (meshSource.IsValid() && meshSource->Mesh.IsValid())
				aabb = meshSource->Mesh->MeshAABB;
			else
				aabb = { MakeVector3(-0.5f), MakeVector3(0.5f) };

			// add a bit of offset to scale to make boundings visible for cubic objects
			BoundingBox box = BoundingBox(aabb.GetCenter(), aabb.Length() * 0.6f);
			box.Rotation = object.Transform.GetRotation();
			box.Center = object.Transform.GetPosition();
			box.Max *= object.Transform.GetScale();
			box.Min *= object.Transform.GetScale();

			Rendering::Draw(box, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
}