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

#include "ResourceEditor.h"
#include "Utilities/ImGui/Editors/ComponentEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/Image/ImageManager.h"
#include "Library/Primitives/Colors.h"
#include "Core/Resources/AssetManager.h"

namespace MxEngine::GUI
{
    void DrawTextureList(const char* name, bool* isOpen)
    {
        ImGui::Begin(name, isOpen);

        static char filter[128] = { '\0' };
        ImGui::InputText("search filter", filter, std::size(filter));

        if (ImGui::CollapsingHeader("create new texture"))
        {
            if (ImGui::Button("load from file"))
            {
                MxString path = FileManager::OpenFileDialog("*.png *.jpg *.jpeg *.bmp *.tga *.hdr", "Image Files");
                if (!path.empty() && File::Exists(path))
                {
                    auto newTexture = AssetManager::LoadTexture(path, TextureFormat::RGBA);
                    newTexture.MakeStatic();
                }
            }

            static Vector3 color{ 0.0f };
            ImGui::ColorEdit3("", &color[0]);
            ImGui::SameLine();
            if (ImGui::Button("create texture"))
            {
                auto colorTexture = Colors::MakeTexture(color);
                colorTexture->SetInternalEngineTag("color.runtime");
                colorTexture.MakeStatic();
            }
        }

        ImGui::SetCursorPosX(0.5f * ImGui::GetWindowWidth() - 20.0f);
        ImGui::Text("%s", "texture list");

        auto& factory = GraphicFactory::Get<Texture>();
        for (auto& object : factory)
        {
            auto id = (int)factory.IndexOf(object);
            ImGui::PushID(id);

            auto texture = GraphicFactory::GetHandle(object);
            auto& texturePath = texture->GetFilePath();

            if (filter[0] == '\0' || texturePath.find(filter) != texturePath.npos)
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("id: %-3d", id);
                ImGui::SameLine();
                GUI::ResourceEditor(texture->GetFilePath().c_str(), texture);
            }

            ImGui::PopID();
        }

        ImGui::End();
    }

    void DrawImageSaver(const TextureHandle& texture, const char* name)
    {
        if (ImGui::Button("save image to disk"))
        {
            MxString path = FileManager::OpenFileDialog("*.png *.jpg *.jpeg *.bmp *.tga *.hdr", "Image Files");
            if (!path.empty())
            {
                ImageManager::SaveTexture(ToFilePath(path), texture);
            }
        }
    }
}