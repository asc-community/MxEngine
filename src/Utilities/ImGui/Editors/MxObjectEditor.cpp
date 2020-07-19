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

namespace MxEngine::GUI
{
	void DrawMxObjectEditor(
		const char* name,
		MxObject& object,
		const MxVector<const char*>& componentNames,
		MxVector<std::function<void(MxObject&)>>& componentAdderCallbacks,
		MxVector<std::function<void(MxObject&)>>& componentEditorCallbacks
	)
	{
		if (ImGui::CollapsingHeader(name))
		{
			GUI::Indent _(5.0f);

			if (ImGui::Button("Destroy object"))
			{
				MxObject::Destroy(object);
			}
			else
			{
				static MxString objectName;
				if (GUI::InputTextOnClick("object name", objectName, 48))
				{
					if (!objectName.empty()) object.Name = objectName;
					objectName.clear();
				}

				static int currentItem = 0;
				ImGui::Combo("", &currentItem, componentNames.data(), (int)componentNames.size());
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
			}
		}
	}
}