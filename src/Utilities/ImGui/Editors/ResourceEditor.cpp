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
#include "ComponentEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/Image/ImageManager.h"
#include "Library/Primitives/Colors.h"
#include "Library/Primitives/Primitives.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Core/Components/Physics/CompoundCollider.h"
#include "Utilities/ImGui/Editors/ComponentEditors/GenericComponentEditor.h"

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

    template<typename T, typename Factory>
    Resource<T, Factory> GetById(int id, Resource<T, Factory>* = nullptr)
    {
        auto handle = (size_t)Max(id, 0);
        auto& storage = Factory::template Get<T>();
        if (storage.IsAllocated(handle))
        {
            return Factory::GetHandle(storage[handle]);
        }
        else
        {
            return Resource<T, Factory>{ };
        }
    }

    template<typename T>
    auto GetById(int id)
    {
        return GetById(id, (T*)nullptr);
    }

    bool IsInternalEngineTexture(const Texture& texture)
    {
        auto& path = texture.GetFilePath();
        return path.find("[[") != path.npos && path.find("]]") != path.npos;
    }

    void DrawTexturePreview(const Texture& texture, float scale)
    {
        auto nativeHeight = texture.GetHeight();
        auto nativeWidth = texture.GetWidth();
        auto width = ImGui::GetWindowSize().x * 0.9f * scale;
        auto height = width * nativeHeight / nativeWidth;
        if (!texture.IsMultisampled()) // TODO: support multisampled textures
        {
            texture.GenerateMipmaps(); // without mipmaps texture can be not visible in editor if its size is too small
            ImGui::Image((void*)(uintptr_t)texture.GetNativeHandle(), ImVec2(width, height), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        }
    }

    rttr::variant TextureEditorExtra(rttr::instance& object)
    {
        Texture& texture = *object.try_convert<Texture>();
        DrawImageSaver(GraphicFactory::GetHandle(texture));

        ImGui::SameLine();
        if (ImGui::Button("flip image"))
        {
            auto image = texture.GetRawTextureData();
            ImageManager::FlipImage(image);
            texture.Load(image, texture.GetFormat(), texture.GetWrapType());
        }

        static float scale = 1.0f;
        ImGui::DragFloat("texture preview scale", &scale, 0.01f, 0.0f, 1.0f);
        DrawTexturePreview(texture, scale);
        return { };
    }

    rttr::variant TextureHandleEditorExtra(rttr::instance& handle)
    {
        rttr::variant result{ };
        auto& texture = *handle.try_convert<TextureHandle>();
        TextureFormat loadingFormat = texture.IsValid() ? texture->GetFormat() : TextureFormat::RGBA;

        if (texture.IsValid())
        {
            if (ImGui::Button("delete"))
            {
                result = rttr::variant{ TextureHandle{ } };
            }
            ImGui::SameLine();
        }
        
        if (ImGui::Button("load from file"))
        {
            MxString path = FileManager::OpenFileDialog("*.png *.jpg *.jpeg *.bmp *.tga *.hdr", "Image Files");
            if (!path.empty() && File::Exists(path))
            {
                auto newTexture = AssetManager::LoadTexture(path, loadingFormat);
                result = rttr::variant{ newTexture };
            }
        }
        
        static int id = 0;
        ImGui::SameLine();
        if (GUI::InputIntOnClick(&id, "load from id"))
        {
            auto newTexture = GetById<TextureHandle>(id);
            result = rttr::variant{ newTexture };
        }

        return result;
    }

    rttr::variant CubeMapEditorExtra(rttr::instance& object) { /* nothing to do */ return { }; }

    rttr::variant CubeMapHandleEditorExtra(rttr::instance& handle)
    {
        rttr::variant result{ };
        auto& texture = *handle.try_convert<CubeMapHandle>();

        if (texture.IsValid())
        {
            if (ImGui::Button("delete"))
            {
                result = rttr::variant{ CubeMapHandle{ } };
            }
            ImGui::SameLine();
        }

        if (ImGui::Button("load from file"))
        {
            MxString path = FileManager::OpenFileDialog("*.png *.jpg *.jpeg *.bmp *.tga *.hdr", "Image Files");
            if (!path.empty() && File::Exists(path))
            {
                auto newCubeMap = AssetManager::LoadCubeMap(path);
                result = rttr::variant{ newCubeMap };
            }
        }

        static int id = 0;
        ImGui::SameLine();
        if (GUI::InputIntOnClick(&id, "load from id"))
        {
            auto newCubeMap = GetById<CubeMapHandle>(id);
            result = rttr::variant{ newCubeMap };
        }

        return result;
    }

    rttr::variant AudioBufferHandleEditorExtra(rttr::instance& handle)
    {
        rttr::variant result{ };
        auto& texture = *handle.try_convert<AudioBufferHandle>();

        if (texture.IsValid())
        {
            if (ImGui::Button("delete"))
            {
                result = rttr::variant{ AudioBufferHandle{ } };
            }
            ImGui::SameLine();
        }

        if (ImGui::Button("load from file"))
        {
            MxString path = FileManager::OpenFileDialog("*.ogg *.flac *.mp3 *.wav", "Image Files");
            if (!path.empty() && File::Exists(path))
            {
                auto newAudio = AssetManager::LoadAudio(path);
                result = rttr::variant{ newAudio };
            }
        }

        static int id = 0;
        ImGui::SameLine();
        if (GUI::InputIntOnClick(&id, "load from id"))
        {
            auto newAudio = GetById<AudioBufferHandle>(id);
            result = rttr::variant{ newAudio };
        }

        return result;
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

    MeshHandle LoadFromPrimitive(int* subdivisions)
    {
        MeshHandle result{ };
        bool optionPeeked = false;
        ImGui::InputInt("subdivisions", subdivisions);
        *subdivisions = Max(*subdivisions, 1);

        if (ImGui::BeginCombo("load primitive", "click to select"))
        {
            if (ImGui::Selectable("cube", &optionPeeked))
            {
                ImGui::SetItemDefaultFocus();
                result = Primitives::CreateCube((size_t)*subdivisions);
            }
            if (ImGui::Selectable("sphere", &optionPeeked))
            {
                ImGui::SetItemDefaultFocus();
                result = Primitives::CreateSphere((size_t)*subdivisions);
            }
            if (ImGui::Selectable("plane", &optionPeeked))
            {
                ImGui::SetItemDefaultFocus();
                result = Primitives::CreatePlane((size_t)*subdivisions);
            }
            if (ImGui::Selectable("cylinder", &optionPeeked))
            {
                ImGui::SetItemDefaultFocus();
                result = Primitives::CreateCylinder((size_t)*subdivisions);
            }
            if (ImGui::Selectable("pyramid", &optionPeeked))
            {
                ImGui::SetItemDefaultFocus();
                result = Primitives::CreatePyramid();
            }
            ImGui::EndCombo();
        }
        return result;
    }

    rttr::variant MeshHandleEditorExtra(rttr::instance& handle)
    {
        rttr::variant result{ };
        auto& mesh = *handle.try_convert<MeshHandle>();

        if (mesh.IsValid())
        {
            if (ImGui::Button("delete"))
            {
                result = rttr::variant{ MeshHandle{ } };
            }
            ImGui::SameLine();
        }

        if (ImGui::Button("load from file"))
        {
            MxString path = FileManager::OpenFileDialog("", "Object Files");
            if (!path.empty() && File::Exists(path))
            {
                auto newMesh = AssetManager::LoadMesh(path);
                result = rttr::variant{ newMesh };
            }
        }

        static int id = 0;
        ImGui::SameLine();
        if (GUI::InputIntOnClick(&id, "load from id"))
        {
            auto newMesh = GetById<MeshHandle>(id);
            result = rttr::variant{ newMesh };
        }

        ImGui::Indent(9.0f);
        static int subdivisions = 1;
        auto primitive = LoadFromPrimitive(&subdivisions);
        if (primitive.IsValid())
        {
            result = rttr::variant{ primitive };
            subdivisions = 1;
        }
        ImGui::Unindent(9.0f);

        return result;
    }

    rttr::variant VariantShapeEditorExtra(rttr::instance& v)
    {
        rttr::variant result{ };
        auto child = v.try_convert<CompoundCollider::CompoundColliderChild>();
        std::visit([&result, &child](auto&& shape)
            {
                auto bounding = shape->GetNativeBounding();
                using BoundingType = decltype(bounding);
                using ShapeType = std::decay_t<decltype(*shape)>;
                const char* name = rttr::type::get<BoundingType>().get_name().cbegin();
                auto editedValue = ResourceEditor(name, bounding);
                if (editedValue.is_valid())
                {
                    auto newShape = PhysicsFactory::Create<ShapeType>(editedValue.template convert<BoundingType>());
                    result = rttr::variant{ CompoundCollider::CompoundColliderChild{ child->Transform, newShape } };
                }
            }, child->Shape);
        return result;
    }

    template<size_t Index = 0, typename... Args>
    void AddShapeSelectableForEachImpl(CompoundCollider& collider, const std::variant<Args...>& types)
    {
        if constexpr (Index < std::variant_size_v<std::variant<Args...>>)
        {
            using ShapeType = std::decay_t<decltype(*std::get<Index>(types))>;
            using BoundingType = decltype(std::declval<ShapeType>().GetNativeBounding());
            const char* name = rttr::type::get<BoundingType>().get_name().cbegin();

            if (ImGui::Selectable(name))
            {
                auto shape = PhysicsFactory::Create<ShapeType>(BoundingType{ });
                collider.AddShape({}, shape);
                ImGui::SetItemDefaultFocus();
            }

            AddShapeSelectableForEachImpl<Index + 1>(collider, types);
        }
    }

    void AddShapeSelectableForEach(CompoundCollider& collider)
    {
        AddShapeSelectableForEachImpl(collider, CompoundCollider::VariantType{ });
    }

    rttr::variant CompoundColliderAddShapeEditorExtra(rttr::instance& v)
    {
        auto& compoundCollider = *v.try_convert<CompoundCollider>();

        if (ImGui::BeginCombo("add child shape", "click to select"))
        {
            AddShapeSelectableForEach(compoundCollider);
            ImGui::EndCombo();
        }
        return { };
    }
}