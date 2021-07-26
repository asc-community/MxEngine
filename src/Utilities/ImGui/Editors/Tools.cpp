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

#include "Tools.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/ImGui/Editors/ComponentEditor.h"
#include "Core/Application/Rendering.h"
#include "Platform/Window/WindowManager.h"

namespace MxEngine
{
    int LogLevelToLogIndex() 
    {
        VerbosityLevel logVerbosity = Logger::GetVerbosityLevel();
        int newIndex = 0;
        switch (logVerbosity) 
        {
        case VerbosityLevel::ALL:         newIndex = 0; break;
        case VerbosityLevel::NO_DEBUG:    newIndex = 1; break;
        case VerbosityLevel::NO_INFO:     newIndex = 2; break;
        case VerbosityLevel::ONLY_ERRORS: newIndex = 3; break;
        case VerbosityLevel::ONLY_FATAL:  newIndex = 4; break;
        }
        return newIndex;
    }

    void GUI::DrawTools(const char* name, bool* isOpen)
    {
        ImGui::Begin(name, isOpen);
        
        if (ImGui::TreeNode("window settings"))
        {
            static MxString title = WindowManager::GetTitle();;
            static auto size = WindowManager::GetSize();
            auto position = WindowManager::GetPosition();

            if (GUI::InputFloatOnClick("window width ", &size.x))
            {
                WindowManager::SetWidth(size.x);
                size = WindowManager::GetSize();
            }

            if (GUI::InputFloatOnClick("window height", &size.y))
            {
                WindowManager::SetHeight(size.y);
                size = WindowManager::GetSize();
            }

            if (GUI::InputTextOnClick("window title ", title, 48))
            {
                WindowManager::SetTitle(title);
                title = WindowManager::GetTitle();
            }

            ImGui::AlignTextToFramePadding();
            ImGui::Text("window position");
            ImGui::SameLine();
            if (ImGui::InputFloat2("", &position[0]))
                WindowManager::SetPosition(position);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("renderer viewport"))
        {
            auto view = ComponentFactory::GetView<CameraController>();
            auto viewport = Rendering::GetViewport();

            int id = 0;
            ImGui::Text("objects with CameraController attached:");
            ImGui::Indent(10.0f);
            for (auto& component : view)
            {
                auto& object = MxObject::GetByComponent(component);
                auto componentHandle = MxObject::GetComponentHandle(component);
                ImGui::PushID(id++);
                bool isSelected = viewport == componentHandle;
                if (ImGui::Selectable(object.Name.c_str(), &isSelected))
                {
                    if (isSelected) Rendering::SetViewport(componentHandle);
                    else Rendering::SetViewport({ });
                }
                ImGui::PopID();
            }
            ImGui::Unindent(10.0f);

            viewport = Rendering::GetViewport();
            if (viewport.IsValid())
            {
                GUI::DrawImageSaver(viewport->GetRenderTexture(), "take screenshot");
                ImGui::Text("attached viewport:");
                ComponentEditor(*viewport);
            }
            else
            {
                ImGui::Text("- no viewport provided");
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("debug settings"))
        {
            auto drawOverlay = Rendering::IsDebugOverlayed();
            if (ImGui::Checkbox("overlay debug", &drawOverlay))
                Rendering::SetDebugOverlay(drawOverlay);

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Logger"))
        {
            const std::array items = { "ALL", "NO_DEBUG", "NO_INFO", "ONLY_ERRORS", "ONLY_FATAL" };
            int selectedItem = LogLevelToLogIndex();
            if (ImGui::Combo("Messages", &selectedItem, items.data(), items.size()))
            {
                switch (selectedItem) 
                {
                case 0: Logger::SetLogLevel(VerbosityLevel::ALL);         break;
                case 1: Logger::SetLogLevel(VerbosityLevel::NO_DEBUG);    break;
                case 2: Logger::SetLogLevel(VerbosityLevel::NO_INFO);     break;
                case 3: Logger::SetLogLevel(VerbosityLevel::ONLY_ERRORS); break;
                case 4: Logger::SetLogLevel(VerbosityLevel::ONLY_FATAL);  break;
                }
                selectedItem = LogLevelToLogIndex();
            }

            ImGui::TreePop();
        }
        
        ImGui::End();
    }
}