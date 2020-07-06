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

#include "Utilities/Time/Time.h"
#include "Core/Event/IEvent.h"
#include "Core/Runtime/RuntimeEditor.h"
#include "Core/Rendering/RenderAdaptor.h"
#include "Core/MxObject/MxObject.h"
#include "Utilities/FileSystem/File.h"
#include "Core/Components/Script.h"

#include "Platform/Window/Window.h"

GENERATE_METHOD_CHECK(OnUpdate, OnUpdate(float()));

namespace MxEngine
{
	class Application
	{
		struct ModuleManager
		{
			ModuleManager(Application* app);
			~ModuleManager();
		} manager;

		using CallbackList = MxVector<std::function<void(TimeStep)>>;
	private:
		static inline Application* Current = nullptr;
		UniqueRef<Window> window;
		RenderAdaptor renderAdaptor;
		AppEventDispatcher dispatcher;
		RuntimeEditor console;
		CallbackList updateCallbacks;
		TimeStep timeDelta = 0.0f;
		size_t counterFPS = 0;
		bool shouldClose = false;
		bool isRunning = false;

		void InitializeRuntime(RuntimeEditor& console);
		void InitializeRenderAdaptor(RenderAdaptor& adaptor);
		void DrawObjects();
		void InvokeUpdate();
		bool VerifyApplicationState();
	protected:

		Application();

		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDestroy();
	public:
		template<typename T>
		void RegisterComponentUpdate();
		void ToggleRuntimeEditor(bool isVisible);
		void CloseOnKeyPress(KeyCode key);

		AppEventDispatcher& GetEventDispatcher();
		RenderAdaptor& GetRenderAdaptor();
		RuntimeEditor& GetRuntimeEditor();
		Window& GetWindow();
		float GetTimeDelta() const;
		size_t GetCurrentFPS() const;
		void Run();
		bool IsRunning() const;
		void CloseApplication();
		void CreateContext();
		virtual ~Application();

		static Application* Get();
		static void Set(Application* application);
	};
	
	template<typename T>
	inline void Application::RegisterComponentUpdate()
	{
		static_assert(has_method_OnUpdate<T>::value, "object must contain OnUpdate(TimeDelta) method");
		this->updateCallbacks.push_back([](TimeStep dt)
		{
			auto view = ComponentFactory::GetView<T>();
			for (auto& component : view)
			{
				component.OnUpdate(dt);
			}
		});
	}
}