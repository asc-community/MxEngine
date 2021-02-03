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

#pragma once

#include "Utilities/STL/MxString.h"
#include "Utilities/Memory/Memory.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Events/KeyEvent.h"
#include "Utilities/ImGui/Editors/Components/ComponentEditor.h"

namespace MxEngine
{
	class GraphicConsole;
	class EventLogger;

	class RuntimeEditor
	{
		GraphicConsole* console = nullptr;
		EventLogger* logger = nullptr;
		Vector2 cachedViewportSize{ 0.0f };
		Vector2 cachedViewportPosition{ 0.0f };
		bool shouldRender = false;
		bool cachedUseDefaultFrameBufferVariable = false;

		MxVector<const char*> componentAdderComponentNames;
		MxVector<void(*)(MxObject&)> componentEditorCallbacks;
		MxVector<void(*)(MxObject&)> componentAdderCallbacks;
		MxObject::Handle currentlySelectedObject{ };

		void DrawMxObjectList(bool* isOpen = nullptr);
		void DrawMxObjectEditorWindow(bool* isOpen = nullptr);
		void DrawTransformManipulator(TransformComponent& transform);
  	public:
		RuntimeEditor();
		RuntimeEditor(const RuntimeEditor&) = delete;
		RuntimeEditor& operator=(const RuntimeEditor&) = delete;
		RuntimeEditor(RuntimeEditor&&) = default;
		RuntimeEditor& operator=(RuntimeEditor&&) = default;
		~RuntimeEditor();

		void LogToConsole(const MxString& message);
		void ClearConsoleLog();
		void PrintCommandHistory();
		void OnUpdate();
		void AddEventEntry(const MxString& entry);
		void Toggle(bool isVisible = true);
		void AddKeyBinding(KeyCode openKey);
		template<typename ShaderHandle>
		void AddShaderUpdateListener(ShaderHandle shader);
		template<typename ShaderHandle, typename FilePath>
		void AddShaderUpdateListener(ShaderHandle shader, const FilePath& lookupDirectory);
		Vector2 GetViewportSize() const;
		Vector2 GetViewportPosition() const;
		bool IsActive() const;
		bool IsKeyHeld(KeyCode key);

		void DrawMxObject(const MxString& name, MxObject::Handle object);

		template<typename T>
		void RegisterComponentEditor()
		{
			this->componentEditorCallbacks.push_back([](MxObject& object)
			{
				auto component = object.GetComponent<T>();
				if (component.IsValid())
					GUI::ComponentEditor(*component);
			});
			// Note: if component has no default constructor, it cannot be added to component list in runtime editor
			if constexpr (std::is_default_constructible_v<T>)
			{
				this->componentAdderComponentNames.push_back(rttr::type::get<T>().get_name().cbegin());
				this->componentAdderCallbacks.push_back([](MxObject& object)
				{
					if(!object.HasComponent<T>())
						object.AddComponent<T>();
				});
			}
		}
	};
}