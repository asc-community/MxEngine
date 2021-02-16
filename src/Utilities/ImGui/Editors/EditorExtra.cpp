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

#include "Core/Components/Instancing/InstanceFactory.h"
#include "Core/Components/Scripting/Script.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Physics/CompoundCollider.h"
#include "Core/Application/Runtime.h"
#include "Platform/GraphicAPI.h"
#include "Utilities/ImGui/Editors/ComponentEditor.h"
#include "Utilities/Format/Format.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/Image/ImageManager.h"
#include "Utilities/STL/MxMap.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Library/Primitives/Primitives.h"

namespace MxEngine::GUI
{
    template<>
    rttr::variant EditorExtra<InstanceFactory>(rttr::instance& val)
    {
        auto& instanceFactory = *val.try_convert<InstanceFactory>();
        auto view = instanceFactory.GetInstances();

        int id = 0;
        for (const auto& object : view)
        {
            auto name = MxFormat("instance #{}", id++);
            if (ImGui::TreeNode(name.c_str()))
            {
                if (ImGui::Button("destroy instance"))
                    MxObject::Destroy(object);
                else
                    Application::GetImpl()->GetRuntimeEditor().DrawMxObject(name, object);
                ImGui::TreePop();
            }
        }
        return { };
    }

    template<>
    rttr::variant EditorExtra<MeshRenderer>(rttr::instance& parent)
    {
        if (ImGui::Button("load from file"))
        {
            auto& meshRenderer = *parent.try_convert<MeshRenderer>();
            MxString path = FileManager::OpenFileDialog();
            if (!path.empty() && File::Exists(path))
                meshRenderer = AssetManager::LoadMaterials(path);
        }
        return { };
    }

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

    template<>
    rttr::variant EditorExtra<Script>(rttr::instance& val)
    {
        auto& script = *val.try_convert<Script>();

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
                    if (newScript) CreateNewScript(scriptName, path);
                    Runtime::AddScriptFile(scriptName, path);
                    Runtime::StartCompilationTask();
                }
                else
                {
                    MXLOG_ERROR("MxEngine::FileManager", "cannot create script file with empty name");
                }
            }
        }

        return { };
    }

    template<typename Handle>
    Handle GetById(int id)
    {
        using Type = typename Handle::Type;
        using Factory = typename Handle::Factory;

        auto handle = (size_t)Max(id, 0);
        auto& storage = Factory::template Get<Type>();
        if (storage.IsAllocated(handle))
        {
            return Factory::GetHandle(storage[handle]);
        }
        else
        {
            return Handle{ };
        }
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

    template<>
    rttr::variant EditorExtra<Texture>(rttr::instance& object)
    {
        Texture& texture = *object.try_convert<Texture>();
        DrawImageSaver(GraphicFactory::GetHandle(texture));

        ImGui::SameLine();
        if (ImGui::Button("flip image"))
        {
            auto image = texture.GetRawTextureData();
            ImageManager::FlipImage(image);
            texture.Load(image, texture.GetFormat());
        }

        static float scale = 1.0f;
        ImGui::DragFloat("texture preview scale", &scale, 0.01f, 0.0f, 1.0f);
        DrawTexturePreview(texture, scale);
        return { };
    }

    template<>
    rttr::variant HandleEditorExtra<Texture>(rttr::instance& handle)
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

    template<>
    rttr::variant EditorExtra<CubeMap>(rttr::instance& object) { /* nothing to do */ return { }; }

    template<>
    rttr::variant HandleEditorExtra<CubeMap>(rttr::instance& handle)
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

    template<>
    rttr::variant HandleEditorExtra<AudioBuffer>(rttr::instance& handle)
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

    template<>
    rttr::variant HandleEditorExtra<Mesh>(rttr::instance& handle)
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

    template<>
    rttr::variant EditorExtra<CompoundCollider::CompoundColliderChild>(rttr::instance& v)
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

    template<>
    rttr::variant EditorExtra<CompoundCollider>(rttr::instance& v)
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