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
#include "Core/Runtime/RuntimeEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Core/Runtime/Reflection.h"
#include "Utilities/Format/Format.h"
#include "Utilities/ImGui/Editors/ComponentEditors/GenericComponentEditor.h"

namespace MxEngine::GUI
{
	rttr::variant DisplayPoolExtra(rttr::instance& val)
	{
		auto& instanceFactory = *val.try_convert<InstanceFactory>();
		auto view = instanceFactory.GetInstances();
		auto& runtimeEditor = Application::GetImpl()->GetRuntimeEditor();

		int id = 0;
		for (const auto& object : view)
		{
			auto name = MxFormat("instance #{}", id++);
			if (ImGui::TreeNode(name.c_str()))
			{
				if (ImGui::Button("destroy instance"))
					MxObject::Destroy(object);
				else
					runtimeEditor.DrawMxObject(name, object);
				ImGui::TreePop();
			}
		}
		return { };
	}

	void InstanceFactoryEditor(InstanceFactory& instanceFactory)
	{
		ComponentEditor(instanceFactory);
	}

	void InstanceEditor(Instance& instance)
	{
		ComponentEditor(instance);
	}
}