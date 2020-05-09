// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "Utilities/ImGui/ImGuiBase.h"
#include "Core/Application/Application.h"

namespace MxEngine::GUI
{
    struct Indent
    {
        float indent;

        inline Indent(float indent) 
            : indent(indent)
        {
            ImGui::Indent(indent);
        }

        inline ~Indent()
        {
            ImGui::Unindent(indent);
        }
    };

    #define GUI_TREE_NODE(name, ...) if(ImGui::CollapsingHeader(name)) { GUI::Indent _(5.0f); __VA_ARGS__; }

    inline void RightFromConsole()
    {
        auto context = Application::Get();
        // Set next window to be right from console
        ImGui::SetNextWindowPos({ context->GetConsole().GetSize().x, 0.0f });
        // Also limit its size according to environment
        ImGui::SetNextWindowSize({ context->GetWindow().GetWidth() - context->GetConsole().GetSize().x - 150.0f, 0.0f });
    }

    inline void UnderConsole()
    {
        auto context = Application::Get();
        // Set next window to be under console
        ImGui::SetNextWindowPos({ 0.0f, context->GetConsole().GetSize().y });
        // Also limit its size according to environment
        ImGui::SetNextWindowSize({ context->GetConsole().GetSize().x, 0.0f });
    }

    inline bool InputIntOnClick(const char* title, int* v)
    {
        ImGui::PushID(title);
        ImGui::InputInt(title, v); 
        ImGui::SameLine();
        bool result = ImGui::Button("apply");
        ImGui::PopID();
        return result;
    }

    inline bool InputFloatOnClick(const char* title, float* v)
    {
        ImGui::PushID(title);
        ImGui::InputFloat(title, v);
        ImGui::SameLine();
        bool result = ImGui::Button("apply");
        ImGui::PopID();
        return result;
    }

    inline bool InputTextOnClick(const char* title, char* v, size_t size)
    {
        ImGui::PushID(title);
        ImGui::InputText(title, v, size);
        ImGui::SameLine();
        bool result = ImGui::Button("apply");
        ImGui::PopID();
        return result;
    }
}