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

#include "Utilities/ImGui/ImGuiBase.h"
#include "Core/Application/Application.h"

namespace MxEngine::GUI
{
	inline void DrawObjectEditor()
	{
		auto context = Application::Get();
		for (const auto& pair : context->GetObjectList())
		{
			if (ImGui::CollapsingHeader(pair.first.c_str()))
			{
				auto& object = *pair.second;

				// toggle object visibility
				bool isDrawn = object.IsDrawable();
				static bool dirVecs = false;
				if (ImGui::Checkbox("drawn", &isDrawn))
					isDrawn ? object.Show() : object.Hide();

				// toggle dir vec input (see below)
				ImGui::SameLine(); ImGui::Checkbox("dir. vecs", &dirVecs);

				// current texture path
				ImGui::Text((std::string("texture: ") + (object.ObjectTexture ? object.GetTexture().GetPath() : std::string("none"))).c_str());

				// object translation
				const auto& oldTranslation = object.ObjectTransform.GetTranslation();
				Vector3 newTranslation = oldTranslation;
				if (ImGui::InputFloat3("translation", &newTranslation[0]))
					pair.second->ObjectTransform.SetTranslation(newTranslation);

				// object rotation (euler)
				const auto& oldRotation = object.ObjectTransform.GetEulerRotation();
				Vector3 newRotation = oldRotation;
				if (ImGui::InputFloat3("rotation", &newRotation[0]))
					pair.second->ObjectTransform.SetRotation(1.0f, newRotation);

				// object scale
				const auto& oldScale = object.ObjectTransform.GetScale();
				Vector3 newScale = oldScale;
				if (ImGui::InputFloat3("scale", &newScale[0]))
					pair.second->ObjectTransform.SetScale(newScale);

				// object texture (loads from file)
				static std::string texturePath(64, '\0');
				ImGui::InputText("texture path", texturePath.data(), texturePath.size());
				ImGui::SameLine();
				if (ImGui::Button("update"))
					object.ObjectTexture = context->CreateTexture(texturePath);

				if (dirVecs)
				{
					Vector3 forward = object.GetForwardVector();
					Vector3 up = object.GetUpVector();
					Vector3 right = object.GetRightVector();
					if (ImGui::InputFloat3("forward vec", &forward[0]))
						object.SetForwardVector(forward);
					if (ImGui::InputFloat3("up vec", &up[0]))
						object.SetUpVector(up);
					if (ImGui::InputFloat3("right vec", &right[0]))
						object.SetRightVector(right);
				}
			}
		}
	}
}