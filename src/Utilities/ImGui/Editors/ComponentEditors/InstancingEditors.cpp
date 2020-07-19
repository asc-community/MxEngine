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

#include "Core/Components/Instancing/InstanceFactory.h"

#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	void InstanceFactoryEditor(InstanceFactory& instanceFactory)
	{
		TREE_NODE_PUSH("InstanceFactory");
		REMOVE_COMPONENT_BUTTON(instanceFactory);

		ImGui::Text("instance count: %d", (int)instanceFactory.GetCount());

		ImGui::SameLine();
		if (ImGui::Button("instanciate"))
			instanceFactory.MakeInstance();
		ImGui::SameLine();
		if (ImGui::Button("destroy all"))
			instanceFactory.DestroyInstances();
		ImGui::SameLine();
		ImGui::Checkbox("is static", &instanceFactory.IsStatic);

		int id = 0;
		auto self = MxObject::GetComponentHandle(instanceFactory);
		for (size_t i = 0; i < self->GetInstancePool().Capacity(); i++)
		{
			if (!self->GetInstancePool().IsAllocated(i)) continue;

			GUI::Indent _(5.0f);
			ImGui::PushID(id);
			MxString nodeName = "instance #" + ToMxString(id++); //-V127
			Application::Get()->GetRuntimeEditor().DrawMxObject(nodeName, *self->GetInstancePool()[i]);
			ImGui::PopID();
		}
	}

	void InstanceEditor(Instance& instance)
	{
		TREE_NODE_PUSH("Instance");
		auto color = instance.GetColor();
		if (ImGui::ColorEdit3("base color", &color[0], ImGuiColorEditFlags_HDR))
			instance.SetColor(color);
	}
}