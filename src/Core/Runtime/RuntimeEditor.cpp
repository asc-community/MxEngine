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
#include "Utilities/FileSystem/FileManager.h"
#include "Core/Events/WindowResizeEvent.h"
#include "Core/Events/UpdateEvent.h"
#include "Core/Application/Event.h"
#include "Core/Application/Rendering.h"
#include "Platform/Window/WindowManager.h"
#include "Platform/Window/Input.h"
#include "Core/Events/FpsUpdateEvent.h"

namespace MxEngine
{
	RuntimeEditor::~RuntimeEditor()
	{
		Free(this->console);
		Free(this->logger);
	}

	void RuntimeEditor::LogToConsole(const MxString& message)
	{
		this->console->PrintLog(message.c_str()); //-V111
	}

	void RuntimeEditor::ClearConsoleLog()
	{
		this->console->ClearLog();
	}

	void RuntimeEditor::PrintCommandHistory()
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
		const float editorRatio = 0.15f;
		const float objectListRatio = 0.5f;

		ImGuiID viewportDockspace = 0; 
		ImGuiID editorDockspace = 0;
		ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, viewportRatio, &viewportDockspace, &editorDockspace);

		ImGuiID viewportId = 0;
		ImGuiID profilerId = 0;
		ImGui::DockBuilderSplitNode(viewportDockspace, ImGuiDir_Up, viewportRatio, &viewportId, &profilerId);

		ImGuiID applicationId = 0;
		ImGuiID objectEditorDockspace = 0;
		ImGui::DockBuilderSplitNode(editorDockspace, ImGuiDir_Up, editorRatio, &applicationId, &objectEditorDockspace);

		ImGuiID objectListId = 0;
		ImGuiID objectEditorId = 0;
		ImGui::DockBuilderSplitNode(objectEditorDockspace, ImGuiDir_Up, objectListRatio, &objectListId, &objectEditorId);

		ImGui::DockBuilderDockWindow("Viewport", viewportId);
		ImGui::DockBuilderDockWindow("Profiling Tools", profilerId);
		ImGui::DockBuilderDockWindow("Application Editor", applicationId);
		ImGui::DockBuilderDockWindow("Object Editor", objectEditorId);
		ImGui::DockBuilderDockWindow("Object List", objectListId);
		ImGui::DockBuilderDockWindow("Developer Console", objectListId);
		ImGui::DockBuilderDockWindow("Render Editor", objectListId);
		ImGui::DockBuilderDockWindow("Texture Viewer", objectListId);

		ImGui::DockBuilderFinish(dockspaceId);
	}

	void RuntimeEditor::OnUpdate()
	{
		if (this->shouldRender)
		{
			MAKE_SCOPE_PROFILER("RuntimeEditor::OnUpdate()");
			auto dockspaceID = ImGui::DockSpaceOverViewport();
			InitDockspace(dockspaceID);

			static bool isRenderEditorOpened = false;
			static bool isObjectListOpened = false;
			static bool isObjectEditorOpened = false;
			static bool isApplicationEditorOpened = true;
			static bool isTextureListOpened = true;
			static bool isDeveloperConsoleOpened = true;
			static bool isProfilerOpened = true;
			static bool isViewportOpened = true;

			this->console->Draw("Developer Console", &isDeveloperConsoleOpened);

			GUI::DrawRenderEditor("Render Editor", &isRenderEditorOpened);
			GUI::DrawApplicationEditor("Application Editor", &isApplicationEditorOpened);
			GUI::DrawTextureList("Texture Viewer", &isTextureListOpened);

			this->DrawMxObjectList(&isObjectListOpened);
			this->DrawMxObjectEditorWindow(&isObjectEditorOpened);

			GUI::DrawViewportWindow("Viewport", this->cachedViewportSize, this->cachedViewportPosition, &isViewportOpened);

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

	void RuntimeEditor::Toggle(bool isVisible)
	{
		this->shouldRender = isVisible;

		if (!this->shouldRender) // if developer environment was turned off, we should notify application that viewport returned to normal
		{
			Rendering::SetRenderToDefaultFrameBuffer(this->cachedUseDefaultFrameBufferVariable);
			auto windowSize = WindowManager::GetSize();
			Event::AddEvent(MakeUnique<WindowResizeEvent>(this->cachedViewportSize, windowSize));
			this->cachedViewportSize = windowSize;
		}
		else
		{
			this->cachedUseDefaultFrameBufferVariable = Rendering::IsRenderedToDefaultFrameBuffer();
			Rendering::SetRenderToDefaultFrameBuffer(false);
		}
	}

	void RuntimeEditor::AddKeyBinding(KeyCode openKey)
	{
		MXLOG_INFO("MxEngine::ConsoleBinding", MxFormat("bound console to keycode: {0}", EnumToString(openKey)));
		Event::AddEventListener<UpdateEvent>("RuntimeEditor", 
		[cursorPos = Vector2(), cursorModeCached = CursorMode::DISABLED, openKey, savedStateKeyHeld = false](auto& event) mutable
		{
			bool isHeld = Application::GetImpl()->GetWindow().IsKeyHeldUnchecked(openKey);
			if (isHeld != savedStateKeyHeld) savedStateKeyHeld = false;

			if (isHeld && !savedStateKeyHeld)
			{
				savedStateKeyHeld = true;
				if (Application::GetImpl()->GetRuntimeEditor().IsActive())
				{
					Input::SetCursorMode(cursorModeCached);
					Application::GetImpl()->ToggleRuntimeEditor(false);
					Input::SetCursorPosition(cursorPos);
				}
				else
				{
					cursorPos = Input::GetCursorPosition();
					cursorModeCached = Input::GetCursorMode();
					Input::SetCursorMode(CursorMode::NORMAL);
					Application::GetImpl()->ToggleRuntimeEditor(true);
					Input::SetCursorPosition(WindowManager::GetSize() * 0.5f);
				}
			}
		});
	}

	template<>
	void RuntimeEditor::AddShaderUpdateListener(ShaderHandle shader, const FilePath& lookupDirectory)
	{
		// we need shader stages to reload shader correctly
		enum ShaderStages : uint8_t
		{
			NONE = 0,
			HAS_VERTEX_STAGE = 1 << 0,
			HAS_GEOMETRY_STAGE = 1 << 1,
			HAS_FRAGMENT_STAGE = 1 << 2,
		};

		#if !defined(MXENGINE_DEBUG)
		MXLOG_WARNING("RuntimeEditor::AddShaderUpdateListener", "cannot add listener in non-debug mode");
		#else
		// list of all files shader depend on
		MxVector<std::pair<FilePath, FileSystemTime>> dependencies;

		auto& vertex = shader->GetVertexShaderDebugFilePath();
		auto& geometry = shader->GetGeometryShaderDebugFilePath();
		auto& fragment = shader->GetFragmentShaderDebugFilePath();
		auto& includes = shader->GetIncludedFilePaths();
		
		// add all filenames to list. File paths and modified time will be resolved later
		if (!vertex.empty()) dependencies.emplace_back(ToFilePath(vertex).filename(), FileSystemTime());
		if (!geometry.empty()) dependencies.emplace_back(ToFilePath(geometry).filename(), FileSystemTime());
		if (!fragment.empty()) dependencies.emplace_back(ToFilePath(fragment).filename(), FileSystemTime());
		for (const auto& include : includes) dependencies.emplace_back(ToFilePath(include).filename(), FileSystemTime());

		// resolve file paths, if file was not found - skip whole shader to avoid crashing in listener
		for (auto& [filepath, modifiedTime] : dependencies)
		{
			auto resolvedFilePath = FileManager::SearchInDirectory(lookupDirectory, filepath);
			if (resolvedFilePath.empty())
			{
				MXLOG_WARNING("MxEngine::Runtime", "cannot find shader file for debug: " + ToMxString(filepath));
				return;
			}
			filepath = resolvedFilePath.lexically_normal();
			modifiedTime = File::LastModifiedTime(filepath);
		}

		// check for all shader stages
		uint8_t stages = ShaderStages::NONE;
		if (!vertex.empty())   stages |= ShaderStages::HAS_VERTEX_STAGE;
		if (!geometry.empty()) stages |= ShaderStages::HAS_GEOMETRY_STAGE;
		if (!fragment.empty()) stages |= ShaderStages::HAS_FRAGMENT_STAGE;
		
		Event::AddEventListener<FpsUpdateEvent>("ShaderDebugEvent", 
			[shader, dependencies = std::move(dependencies), stages](FpsUpdateEvent&) mutable
			{
				bool alreadyModified = false;
				for (auto& [filepath, modifiedTime] : dependencies)
				{
					auto lastModified = File::LastModifiedTime(filepath);
					if(!alreadyModified && modifiedTime < lastModified)
					{
						alreadyModified = true;
						// check for shader stages combinations: vertex & fragment are required,
						// other stages are optional
						constexpr uint8_t VF = ShaderStages::HAS_VERTEX_STAGE | ShaderStages::HAS_FRAGMENT_STAGE;
						constexpr uint8_t VGF = VF | ShaderStages::HAS_GEOMETRY_STAGE;

						switch(stages)
						{
						case VF:
							shader->Load(
								dependencies[0].first, dependencies[1].first
							);
							break;
						case VGF:
							shader->Load(
								dependencies[0].first, dependencies[1].first, dependencies[2].first
							);
							break;
						}
					}
					modifiedTime = lastModified;
				}
			});
		#endif
	}

	template<>
	void RuntimeEditor::AddShaderUpdateListener(ShaderHandle shader)
	{
		#if !defined(MXENGINE_DEBUG)
		MXLOG_WARNING("RuntimeEditor::AddShaderUpdateListener", "cannot add listener in non-debug mode");
		#else
		auto lookupDirectory = ToFilePath(shader->GetFragmentShaderDebugFilePath()).parent_path();
		RuntimeEditor::AddShaderUpdateListener<ShaderHandle, FilePath>(std::move(shader), lookupDirectory);
		#endif
	}

    void RuntimeEditor::DrawMxObject(const MxString& treeName, MxObject& object)
    {
		GUI::DrawMxObjectEditor(treeName.c_str(), object, true, this->componentNames, this->componentAdderCallbacks, this->componentEditorCallbacks);
    }

	Vector2 RuntimeEditor::GetViewportSize() const
	{
		return this->IsActive() ? this->cachedViewportSize : (Vector2)Rendering::GetViewportSize();
	}

	Vector2 RuntimeEditor::GetViewportPosition() const
	{
		return this->IsActive() ? this->cachedViewportPosition : MakeVector2(0.0f);
	}

	bool RuntimeEditor::IsActive() const
	{
		return this->shouldRender;
	}

	void RuntimeEditor::DrawMxObjectList(bool* isOpen)
	{
		ImGui::Begin("Object List", isOpen);

		static char filter[128] = { '\0' };
		ImGui::InputText("search filter", filter, std::size(filter));

		if (ImGui::Button("create new MxObject"))
			MxObject::Create();
		ImGui::SameLine();
		if (ImGui::Button("delete currently selected"))
			MxObject::Destroy(this->currentlySelectedObject);

		auto objects = MxObject::GetObjects();
		int id = 0;
		for (auto& object : objects)
		{
			bool shouldDisplay = object.IsDisplayedInRuntimeEditor() && object.Name.find(filter) != object.Name.npos;
			if (shouldDisplay && id < 10000) // do not display too much objects
			{
				ImGui::PushID(id++);
				bool isSelected = this->currentlySelectedObject == MxObject::GetHandle(object);
				if (ImGui::Selectable(object.Name.c_str(), &isSelected))
				{
					if (isSelected) this->currentlySelectedObject = MxObject::GetHandle(object);
					else this->currentlySelectedObject = { };
				}
				ImGui::PopID();
			}
		}
	}

	void RuntimeEditor::DrawMxObjectEditorWindow(bool* isOpen)
	{
		ImGui::Begin("Object Editor", isOpen);
		if (!this->currentlySelectedObject.IsValid())
			ImGui::Text("no object selected");
		else
			this->DrawMxObject(this->currentlySelectedObject->Name, *this->currentlySelectedObject);
		ImGui::End();
	}

	RuntimeEditor::RuntimeEditor()
	{
		MAKE_SCOPE_PROFILER("DeveloperConsole::Init");
		MAKE_SCOPE_TIMER("MxEngine::DeveloperConsole", "DeveloperConsole::Init");
		this->console = Alloc<GraphicConsole>();
		this->logger = Alloc<EventLogger>();
	}
}