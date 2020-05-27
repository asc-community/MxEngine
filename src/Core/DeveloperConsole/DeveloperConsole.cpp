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

#include "DeveloperConsole.h"
#include "Utilities/ImGui/GraphicConsole/GraphicConsole.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Library/Scripting/Python/PythonEngine.h"

namespace MxEngine
{
	DeveloperConsole::~DeveloperConsole()
	{
		Free(this->engine);
		Free(this->console);
	}

	void DeveloperConsole::Log(const MxString& message)
	{
		this->console->PrintLog("%s", message.c_str());
	}

	void DeveloperConsole::ClearLog()
	{
		this->console->ClearLog();
	}

	void DeveloperConsole::PrintHistory()
	{
		this->console->PrintHistory();
	}

	void DeveloperConsole::OnRender()
	{
		if (this->shouldRender)
		{
			ImGui::SetNextWindowPos({ 0, 0 });
			this->console->Draw("Developer Console");

			if (this->debugTools)
			{
				GUI::RightFromConsole();
				ImGui::Begin("Debug Tools");
				GUI_TREE_NODE("Camera Editor",  GUI::DrawCameraEditor());
				GUI_TREE_NODE("Objects Editor", GUI::DrawObjectEditor());
				GUI_TREE_NODE("Profiler",       GUI::DrawProfiler());
				GUI_TREE_NODE("Light Editor",   GUI::DrawLightEditor());
				ImGui::End();
			}
		}
	}

	void DeveloperConsole::SetSize(const Vector2& size)
	{
		this->console->SetSize({ size.x, size.y });
	}

	void DeveloperConsole::Toggle(bool isVisible)
	{
		this->shouldRender = isVisible;
	}

    void DeveloperConsole::UseDebugTools(bool value)
    {
		this->debugTools = value;
    }

	DeveloperConsole::ScriptEngine& DeveloperConsole::GetEngine()
	{
		return *this->engine;
	}

	Vector2 DeveloperConsole::GetSize() const
	{
		return Vector2(this->console->GetSize().x, this->console->GetSize().y);
	}

	bool DeveloperConsole::IsToggled() const
	{
		return this->shouldRender;
	}

#if defined(MXENGINE_USE_PYTHON)
	DeveloperConsole::DeveloperConsole()
	{
		MAKE_SCOPE_PROFILER("DeveloperConsole::Init");
		MAKE_SCOPE_TIMER("MxEngine::DeveloperConsole", "DeveloperConsole::Init");
		this->engine = Alloc<PythonEngine>();
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
#else
	DeveloperConsole::DeveloperConsole()
	{
		MAKE_SCOPE_PROFILER("DeveloperConsole::Init");
		MAKE_SCOPE_TIMER("MxEngine::DeveloperConsole", "DeveloperConsole::Init");
		this->engine = nullptr;
		this->console = Alloc<GraphicConsole>();
	}
#endif
}