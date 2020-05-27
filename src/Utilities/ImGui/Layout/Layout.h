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

#include "Utilities/ImGui/ImGuiBase.h"
#include "Core/Application/Application.h"

namespace MxEngine::GUI
{
    /*!
    Indent is a special object which is created on stack to push extra element offset
    on its destruction offset is popped, resulting in a tree-like structure of GUI elements
    */
    struct Indent
    {
        /*!
        offset to be popped on destruction
        */
        float indent;

        /*!
        construct Indent object, adding indent to GUI elements
        */
        inline Indent(float indent) 
            : indent(indent)
        {
            ImGui::Indent(indent);
        }

        /*!
        destroys Indent object, removing indent from GUI elements
        */
        inline ~Indent()
        {
            ImGui::Unindent(indent);
        }
    };

    // GUI_TREE_NODE is a special macro to create tree nodes with offset all code of node must be places after its name as argument, i.e. GUI_TREE_NODE(NAME, CODE);
    #define GUI_TREE_NODE(name, ...) if(ImGui::CollapsingHeader(name)) { GUI::Indent _(5.0f); __VA_ARGS__; }

    /*!
    Setting next drawn window position to be in the right from graphic console of Application (see GraphicConsole for more info)
    */
    inline void RightFromConsole()
    {
        auto context = Application::Get();
        // Set next window to be right from console
        ImGui::SetNextWindowPos({ context->GetConsole().GetSize().x, 0.0f });
        // Also limit its size according to environment
        ImGui::SetNextWindowSize({ context->GetWindow().GetWidth() - context->GetConsole().GetSize().x - 150.0f, 0.0f });
    }

    /*!
    Setting next drawn window position to be in the under graphic console of Application (see GraphicConsole for more info)
    */
    inline void UnderConsole()
    {
        auto context = Application::Get();
        // Set next window to be under console
        ImGui::SetNextWindowPos({ 0.0f, context->GetConsole().GetSize().y });
        // Also limit its size according to environment
        ImGui::SetNextWindowSize({ context->GetConsole().GetSize().x, 0.0f });
    }

    /*!
    creates input field for an int and "apply" button next to it
    \param title input field title
    \param v value in which input will be stored
    \returns true if "apply" button was pressed, false either
    */
    inline bool InputIntOnClick(const char* title, int* v)
    {
        ImGui::PushID(title);
        ImGui::InputInt(title, v); 
        ImGui::SameLine();
        bool result = ImGui::Button("apply");
        ImGui::PopID();
        return result;
    }

    /*!
    creates input field for a float and "apply" button next to it
    \param title input field title
    \param v value in which input will be stored
    \returns true if "apply" button was pressed, false either
    */
    inline bool InputFloatOnClick(const char* title, float* v)
    {
        ImGui::PushID(title);
        ImGui::InputFloat(title, v);
        ImGui::SameLine();
        bool result = ImGui::Button("apply");
        ImGui::PopID();
        return result;
    }

    /*!
    creates input field for a char string and "apply" button next to it
    \param title input field title
    \param v value in which input will be stored
    \param size maximal size of v to limit user input
    \returns true if "apply" button was pressed, false either
    */
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