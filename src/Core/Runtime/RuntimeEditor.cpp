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
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Library/Scripting/Python/PythonEngine.h"
#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
	RuntimeEditor::~RuntimeEditor()
	{
		Free(this->engine);
		Free(this->console);
	}

	void RuntimeEditor::Log(const MxString& message)
	{
		this->console->PrintLog("%s", message.c_str());
	}

	void RuntimeEditor::ClearLog()
	{
		this->console->ClearLog();
	}

	void RuntimeEditor::PrintHistory()
	{
		this->console->PrintHistory();
	}

	void RuntimeEditor::OnRender()
	{
		if (this->shouldRender)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			ImGui::Begin("DockSpace Demo", nullptr, window_flags);
			ImGui::PopStyleVar(3);
			ImGui::DockSpace(0xFF);
			ImGui::End();

			this->console->Draw("Developer Console");

			GUI::DrawViewportWindow(cachedWindowSize);

			if (this->debugTools)
			{
				ImGui::Begin("Debug Tools");
				GUI_TREE_NODE("Profiler", GUI::DrawProfiler());
				ImGui::End();
			}

			ImGui::Begin("Object Editor");
			auto objects = MxObject::GetObjects();
			int id = 0;
			for (auto& object : objects)
			{
				ImGui::PushID(object.Name.c_str());
				if (ImGui::TreeNode(object.Name.c_str()))
				{
					GUI::Indent _(5.0f);
					ImGui::PushID(id++);

					if (ImGui::Button("Destroy"))
					{
						MxObject::Destroy(object);
					}
					else
					{
						for (const auto& callback : this->componentEditorCallbacks)
							callback(object);
					}

					ImGui::PopID();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::End();
		}
	}

	void RuntimeEditor::SetSize(const Vector2& size)
	{
		this->console->SetSize({ size.x, size.y });
	}

	void RuntimeEditor::Toggle(bool isVisible)
	{
		this->shouldRender = isVisible;
		// turn off rendering to default framebuffer while in developer environment
		Application::Get()->GetRenderAdaptor().SetRenderToDefaultFrameBuffer(!this->shouldRender);

		if (!this->shouldRender) // if developer environment was turned off, we should notify application that viewport returned to normal
		{
			auto windowSize = MakeVector2((float)Application::Get()->GetWindow().GetWidth(), (float)Application::Get()->GetWindow().GetHeight());
			Application::Get()->GetEventDispatcher().AddEvent(MakeUnique<WindowResizeEvent>(this->cachedWindowSize, windowSize));
			this->cachedWindowSize = windowSize;
		}
	}

    void RuntimeEditor::UseDebugTools(bool value)
    {
		this->debugTools = value;
    }

	RuntimeEditor::ScriptEngine& RuntimeEditor::GetEngine()
	{
		return *this->engine;
	}

	Vector2 RuntimeEditor::GetSize() const
	{
		return Vector2(this->console->GetSize().x, this->console->GetSize().y);
	}

	bool RuntimeEditor::IsToggled() const
	{
		return this->shouldRender;
	}

	void RuntimeEditor::Execute(const MxString& code)
	{
		this->GetEngine().Execute(code.c_str());
	}

	const MxString& RuntimeEditor::GetLastErrorMessage() const
	{
		return this->engine->GetErrorMessage();
	}

	bool RuntimeEditor::HasErrorsInExecution() const
	{
		return this->engine->HasErrors();
	}

	RuntimeEditor::RuntimeEditor()
	{
		MAKE_SCOPE_PROFILER("DeveloperConsole::Init");
		MAKE_SCOPE_TIMER("MxEngine::DeveloperConsole", "DeveloperConsole::Init");
		#if defined(MXENGINE_USE_PYTHON)
		this->engine = Alloc<PythonEngine>();
		#else
		this->engine = nullptr;
		#endif
		this->console = Alloc<GraphicConsole>();

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
	}
}