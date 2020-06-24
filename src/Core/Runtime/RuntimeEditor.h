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
#include "Core/Event/Events/KeyEvent.h"
#include <functional>

namespace MxEngine
{
	class GraphicConsole;

	#if defined(MXENGINE_USE_PYTHON)
	class PythonEngine;
	#endif


	class RuntimeEditor
	{
		#if defined(MXENGINE_USE_PYTHON)
		using ScriptEngine = PythonEngine;
		#else
		using ScriptEngine = int; // stub
		#endif

		ScriptEngine* engine;
		GraphicConsole* console;
		Vector2 cachedWindowSize{ 0.0f };
		bool shouldRender = false;
		bool useDefaultFrameBufferCached = false;

		MxVector<std::function<void(MxObject&)>> componentEditorCallbacks;
		MxVector<std::function<void(MxObject&)>> componentAdderCallbacks;
		MxVector<const char*> componentNames;
	public:
		RuntimeEditor();
		RuntimeEditor(const RuntimeEditor&) = delete;
		RuntimeEditor(RuntimeEditor&&) = default;
		~RuntimeEditor();

		void Log(const MxString& message);
		void ClearLog();
		void PrintHistory();
		void OnRender();
		void SetSize(const Vector2& size);
		void Toggle(bool isVisible = true);
		void AddKeyBinding(KeyCode openKey);

		ScriptEngine& GetEngine();
		Vector2 GetSize() const;
		bool IsToggled() const;

		void ExecuteScript(const MxString& code);
		bool HasErrorsInExecution() const;
		const MxString& GetLastErrorMessage() const;

		template<typename T>
		void RegisterComponentEditor(const char* name, std::function<void(T&)> callback)
		{
			this->componentEditorCallbacks.push_back([func = std::move(callback)](MxObject& object)
			{
				auto component = object.GetComponent<T>();
				if (component.IsValid())
					func(*component);
			});
			if constexpr (std::is_default_constructible_v<T>)
			{
				this->componentNames.push_back(name);
				this->componentAdderCallbacks.push_back([](MxObject& object)
				{
					if(!object.HasComponent<T>())
						object.AddComponent<T>();
				});
			}
		}

		template<typename Func>
		void RegisterComponentEditor(const char* name, Func&& callback)
		{
			this->RegisterComponentEditor(name, std::function{ std::forward<Func>(callback) });
		}
	};
}