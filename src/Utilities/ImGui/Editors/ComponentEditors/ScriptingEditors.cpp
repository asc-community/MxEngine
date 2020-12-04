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

#include "Utilities/ImGui/Editors/ComponentEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/FileSystem/FileManager.h"

#include "Core/Components/Scripting/Script.h"
#include "Core/Application/Runtime.h"

#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

namespace MxEngine::GUI
{
    void CreateNewScript(const MxString& scriptName, const MxString& filepath)
    {
        File script(filepath, File::WRITE);

        script << "#include <MxEngine.h>\n\n";
        script << "using namespace MxEngine;\n\n";
        script << "class " << scriptName << " : public MxEngine::Scriptable\n";
        script << "{\n";
        script << "public:\n";
        script << "    virtual void OnCreate(MxEngine::MxObject& self) override\n";
        script << "    {\n";
        script << "        \n";
        script << "    }\n\n";
        script << "    virtual void OnReload(MxEngine::MxObject& self) override\n";
        script << "    {\n";
        script << "        \n";
        script << "    }\n\n";
        script << "    virtual void OnUpdate(MxEngine::MxObject& self) override\n";
        script << "    {\n";
        script << "        \n";
        script << "    }\n";
        script << "};\n\n";
        script << "MXENGINE_RUNTIME_EDITOR(" << scriptName << ");\n";
    }

    void ScriptEditor(Script& script)
    {
        TREE_NODE_PUSH("Script");
        REMOVE_COMPONENT_BUTTON(script);

        if (RuntimeCompiler::HasCompilationTaskInProcess())
        {
            ImGui::Text("[note]: recompile task is started...");
        }

        auto& scripts = RuntimeCompiler::GetRegisteredScripts();

        if (ImGui::BeginCombo("select script", "click to expand"))
        {
            if (ImGui::Selectable("none"))
            {
                script.RemoveScriptableObject();
                ImGui::SetItemDefaultFocus();
            }

            for (const auto& scriptInfo : scripts)
            {
                const auto& name = scriptInfo.second.Name;
                if (ImGui::Selectable(name.c_str()))
                {
                    script.SetScriptableObject(name);
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        bool addScript = ImGui::Button("add existing script file");
        ImGui::SameLine();
        bool newScript = ImGui::Button("create new script file");

        if (addScript || newScript)
        {
            auto path = newScript ?
                FileManager::SaveFileDialog("*.cpp", "MxEngine script files") :
                FileManager::OpenFileDialog("*.cpp", "MxEngine script files");
            if (!path.empty())
            {
                auto scriptFileName = ToFilePath(path).stem();
                if (!scriptFileName.empty())
                {
                    auto scriptName = ToMxString(scriptFileName);
                    if(newScript) CreateNewScript(scriptName, path);
                    Runtime::AddScriptFile(scriptName, path);
                    Runtime::StartCompilationTask();
                }
                else
                {
                    MXLOG_ERROR("MxEngine::FileManager", "cannot create script file with empty name");
                }
            }
        }

        if (!script.HasScriptableObject())
        {
            ImGui::Text("no script attached");
        }
        else
        {
            ImGui::Text("current script: %s", script.GetScriptName().c_str());
            ImGui::Text("script filename: %s", script.GetScriptFileName().c_str());
        }
    }
}