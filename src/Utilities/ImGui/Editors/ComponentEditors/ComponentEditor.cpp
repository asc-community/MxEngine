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

#include "Core/Components/Behaviour.h"
#include "Core/Components/Transform.h"

#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

    void TransformEditor(TransformComponent& transform)
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

		TimerMode timer = behaviour.GetTimerMode();
		bool eachFrame = timer == TimerMode::UPDATE_EACH_FRAME;
		bool eachDelta = timer == TimerMode::UPDATE_EACH_DELTA;
		bool onceDelta = timer == TimerMode::UPDATE_AFTER_DELTA;
		bool nSecDelta = timer == TimerMode::UPDATE_FOR_N_SECONDS;
		static auto timeDelta = 0.0f;
		static MxString tag;

		if (GUI::InputTextOnClick("tag", tag, 128))
			behaviour.Tag = tag;

		ImGui::InputFloat("time delta", &timeDelta, 0.01f);

		if (ImGui::BeginCombo("timer mode", eachFrame ? "per frame" : (eachDelta ? "per delta" : (onceDelta ? "once" : "per n seconds"))))
		{
			if (ImGui::Selectable("per frame", &eachFrame))
			{
				behaviour.Schedule(TimerMode::UPDATE_EACH_FRAME);
			}
			if (ImGui::Selectable("per delta", &eachDelta))
			{
				behaviour.Schedule(TimerMode::UPDATE_EACH_DELTA, timeDelta);
			}
			if (ImGui::Selectable("once", &onceDelta))
			{
				behaviour.Schedule(TimerMode::UPDATE_AFTER_DELTA, timeDelta);
			}
			if (ImGui::Selectable("for n seconds", &nSecDelta))
			{
				behaviour.Schedule(TimerMode::UPDATE_FOR_N_SECONDS, timeDelta);
			}
			ImGui::EndCombo();
		}

		ImGui::Text("tag: %s", behaviour.Tag.c_str());
		ImGui::AlignTextToFramePadding();
		ImGui::Text("has update callback: %s", BOOL_STRING(behaviour.HasBehaviour()));
		ImGui::SameLine();
		if (ImGui::Button("remove")) 
			behaviour.RemoveBehaviour();
	}
}