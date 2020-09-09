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

#include "RuntimeEditor.h"
#include "Utilities/ImGui/GraphicConsole.h"
#include "Utilities/ImGui/EventLogger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/Format/Format.h"
#include "Core/Events/WindowResizeEvent.h"
#include "Core/Events/UpdateEvent.h"
#include "Core/Application/Event.h"
#include "Core/Application/Rendering.h"
#include "Platform/Window/WindowManager.h"
#include "Platform/Window/Input.h"
#include "Library/Scripting/Python/PythonEngine.h"

namespace MxEngine
{
	RuntimeEditor::~RuntimeEditor()
	{
		Free(this->engine);
		Free(this->console);
		Free(this->logger);
	}

	void RuntimeEditor::Log(const MxString& message)
	{
		this->console->PrintLog(message.c_str()); //-V111
	}

	void RuntimeEditor::ClearLog()
	{
		this->console->ClearLog();
	}

	void RuntimeEditor::PrintHistory()
	{
		this->console->PrintHistory();
	}

	void InitDockspace(ImGuiID dockspaceId)
	{
		static bool inited = false;
		auto node = ImGui::DockBuilderGetNode(dockspaceId);

		if (inited || (node != nullptr && node->IsSplitNode()))
			return;

		inited = true;
		const float viewportRatio = 0.7f;
		const float editorRatio = 0.1f;

		ImGuiID leftDockspace = 0; 
		ImGuiID rightDockspace = 0;
		ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, viewportRatio, &leftDockspace, &rightDockspace);

		ImGuiID viewportId = 0;
		ImGuiID profilerId = 0;
		ImGui::DockBuilderSplitNode(leftDockspace, ImGuiDir_Up, viewportRatio, &viewportId, &profilerId);

		ImGuiID rightUpDockspace = 0;
		ImGuiID rightDownDockspace = 0;
		ImGui::DockBuilderSplitNode(rightDockspace, ImGuiDir_Up, editorRatio, &rightUpDockspace, &rightDownDockspace);

		ImGui::DockBuilderDockWindow("Viewport", viewportId);
		ImGui::DockBuilderDockWindow("Profiling Tools", profilerId);
		ImGui::DockBuilderDockWindow("Application Editor", rightUpDockspace);
		ImGui::DockBuilderDockWindow("Object Editor", rightDownDockspace);
		ImGui::DockBuilderDockWindow("Developer Console", rightDownDockspace);
		ImGui::DockBuilderDockWindow("Object Editor", rightDownDockspace);
		ImGui::DockBuilderDockWindow("Render Editor", rightDownDockspace);
		ImGui::DockBuilderDockWindow("Texture Viewer", rightDownDockspace);

		ImGui::DockBuilderFinish(dockspaceId);
	}

	void RuntimeEditor::OnUpdate()
	{
		if (this->shouldRender)
		{
			auto dockspaceID = ImGui::DockSpaceOverViewport();
			InitDockspace(dockspaceID);

			static bool isRenderEditorOpened = false;
			static bool isObjectEditorOpened = false;
			static bool isApplicationEditorOpened = true;
			static bool isTextureListOpened = true;
			static bool isDeveloperConsoleOpened = true;
			static bool isProfilerOpened = true;
			static bool isViewportOpened = true;

			this->console->Draw("Developer Console", &isDeveloperConsoleOpened);

			GUI::DrawRenderEditor("Render Editor", &isRenderEditorOpened);
			GUI::DrawApplicationEditor("Application Editor", &isApplicationEditorOpened);

			{
				ImGui::Begin("Object Editor", &isObjectEditorOpened);

				if (ImGui::Button("create new MxObject"))
					MxObject::Create();

				auto objects = MxObject::GetObjects();
				int id = 0;
				for (auto& object : objects)
				{
					if (object.IsDisplayedInRuntimeEditor())
					{
						ImGui::PushID(id++);
						this->DrawMxObject(object.Name, object);
						ImGui::PopID();
					}
				}
			}

			// should be rendered after object editor, as user can delete drawn textures in this editor
			GUI::DrawTextureList("Texture Viewer", &isTextureListOpened);
			GUI::DrawViewportWindow("Viewport", this->cachedWindowSize, &isViewportOpened);

			{
				ImGui::Begin("Profiling Tools", &isProfilerOpened);
				
				GUI_TREE_NODE("Profiler", GUI::DrawProfiler("fps profiler"));
				this->logger->Draw("Event Logger", 20);

				ImGui::End();
			}
			
			ImGui::End();
		}
	}

	void RuntimeEditor::AddEventEntry(const MxString& entry)
	{
		this->logger->AddEventEntry(entry);
	}

	void RuntimeEditor::SetSize(const Vector2& size)
	{
		this->console->SetSize({ size.x, size.y });
	}

	void RuntimeEditor::Toggle(bool isVisible)
	{
		this->shouldRender = isVisible;

		if (!this->shouldRender) // if developer environment was turned off, we should notify application that viewport returned to normal
		{
			Rendering::SetRenderToDefaultFrameBuffer(this->useDefaultFrameBufferCached);
			auto windowSize = WindowManager::GetSize();
			Event::AddEvent(MakeUnique<WindowResizeEvent>(this->cachedWindowSize, windowSize));
			this->cachedWindowSize = windowSize;
		}
		else
		{
			this->useDefaultFrameBufferCached = Rendering::IsRenderedToDefaultFrameBuffer();
			Rendering::SetRenderToDefaultFrameBuffer(false);
		}
	}

	void RuntimeEditor::AddKeyBinding(KeyCode openKey)
	{
		MXLOG_INFO("MxEngine::ConsoleBinding", MxFormat("bound console to keycode: {0}", EnumToString(openKey)));
		Event::AddEventListener<UpdateEvent>("RuntimeEditor", 
		[cursorPos = Vector2(), cursorModeCached = CursorMode::DISABLED, openKey, savedStateKeyHeld = false](auto& event) mutable
		{
			bool isHeld = Application::Get()->GetWindow().IsKeyHeldUnchecked(openKey);
			if (isHeld != savedStateKeyHeld) savedStateKeyHeld = false;


			if (isHeld && !savedStateKeyHeld)
			{
				savedStateKeyHeld = true;
				if (Application::Get()->GetRuntimeEditor().IsActive())
				{
					Input::SetCursorMode(cursorModeCached);
					Application::Get()->ToggleRuntimeEditor(false);
					Input::SetCursorPosition(cursorPos);
				}
				else
				{
					cursorPos = Input::GetCursorPosition();
					cursorModeCached = Input::GetCursorMode();
					Input::SetCursorMode(CursorMode::NORMAL);
					Application::Get()->ToggleRuntimeEditor(true);
					Input::SetCursorPosition(WindowManager::GetSize() * 0.5f);
				}
			}
		});
	}

    void RuntimeEditor::DrawMxObject(const MxString& treeName, MxObject& object)
    {
		GUI::DrawMxObjectEditor(treeName.c_str(), object, this->componentNames, this->componentAdderCallbacks, this->componentEditorCallbacks);
    }

	RuntimeEditor::ScriptEngine& RuntimeEditor::GetEngine()
	{
		return *this->engine;
	}

	Vector2 RuntimeEditor::GetSize() const
	{
		return Vector2(this->console->GetSize().x, this->console->GetSize().y);
	}

	bool RuntimeEditor::IsActive() const
	{
		return this->shouldRender;
	}

	void RuntimeEditor::ExecuteScript(const MxString& code)
	{
		#if defined(MXENGINE_USE_PYTHON)
		MAKE_SCOPE_PROFILER("RuntimeEditor::ExecuteScript");
		this->GetEngine().Execute(code.c_str());
		#endif
	}

	const MxString& RuntimeEditor::GetLastErrorMessage() const
	{
		#if defined(MXENGINE_USE_PYTHON)
		return this->engine->GetErrorMessage();
		#else
		static MxString empty;
		return empty;
		#endif
	}

	bool RuntimeEditor::HasErrorsInExecution() const
	{
		#if defined(MXENGINE_USE_PYTHON)
		return this->engine->HasErrors();
		#else
		return false;
		#endif
	}

	RuntimeEditor::RuntimeEditor()
	{
		MAKE_SCOPE_PROFILER("DeveloperConsole::Init");
		MAKE_SCOPE_TIMER("MxEngine::DeveloperConsole", "DeveloperConsole::Init");
		this->console = Alloc<GraphicConsole>();
		this->logger = Alloc<EventLogger>();

		#if defined(MXENGINE_USE_PYTHON)
		this->engine = Alloc<PythonEngine>();

		this->engine->Execute("from mx_engine import *");
		this->engine->Execute("dt = lambda: mx.dt()");

		this->console->SetEventCallback([this](const char* text)
		{
			this->engine->Execute(text);
			if (this->engine->HasErrors())
			{
				this->Log("[error]: " + this->engine->GetErrorMessage());
			}
			else if (!this->engine->GetOutput().empty())
			{
				this->Log(this->engine->GetOutput());
			}
		});
		#endif
	}
}