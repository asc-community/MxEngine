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
#include "Core/BoundingObjects/Cylinder.h"
#include "Core/BoundingObjects/Capsule.h"
#include "Core/BoundingObjects/BoundingBox.h"
#include "Utilities/Image/ImageManager.h"
#include "Library/Primitives/Colors.h"
#include "Library/Primitives/Primitives.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Core/Runtime/Reflection.h"
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

    void TextureEditorExtra(rttr::instance& object)
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

    void CubeMapEditorExtra(rttr::instance& object) { /* nothing to do */ }

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

    void DrawAABBEditor(const char* name, AABB& aabb)
    {
        SCOPE_TREE_NODE(name);
        ImGui::DragFloat3("min", &aabb.Min[0], 0.01f);
        ImGui::DragFloat3("max", &aabb.Max[0], 0.01f);
        aabb.Min = VectorMin(aabb.Min, aabb.Max);
        aabb.Max = VectorMax(aabb.Min, aabb.Max);
    }

    void DrawBoxEditor(const char* name, BoundingBox& box)
    {
        SCOPE_TREE_NODE(name);

        ImGui::DragFloat3("center", &box.Center[0]);
        ImGui::DragFloat3("min", &box.Min[0], 0.01f);
        ImGui::DragFloat3("max", &box.Max[0], 0.01f);
        
        auto rotation = DegreesVec(MakeEulerAngles(box.Rotation));
        auto newRotation = rotation;
        if (ImGui::DragFloat("rotate x", &newRotation.x))
            box.Rotation *= MakeQuaternion(Radians(newRotation.x - rotation.x), MakeVector3(1.0f, 0.0f, 0.0f));
        if (ImGui::DragFloat("rotate y", &newRotation.y))
            box.Rotation *= MakeQuaternion(Radians(newRotation.y - rotation.y), MakeVector3(0.0f, 1.0f, 0.0f));
        if (ImGui::DragFloat("rotate z", &newRotation.z))
            box.Rotation *= MakeQuaternion(Radians(newRotation.z - rotation.z), MakeVector3(0.0f, 0.0f, 1.0f));

        box.Min = VectorMin(box.Min, box.Max);
        box.Max = VectorMax(box.Min, box.Max);
    }

    void DrawSphereEditor(const char* name, BoundingSphere& sphere)
    {
        SCOPE_TREE_NODE(name);
        ImGui::DragFloat3("center", &sphere.Center[0], 0.01f);
        ImGui::DragFloat("radius", &sphere.Radius, 0.01f);
        sphere.Radius = Max(sphere.Radius, 0.0f);
    }

    void DrawCylinderEditor(const char* name, Cylinder& cylinder)
    {
        SCOPE_TREE_NODE(name);
        ImGui::DragFloat("height",   &cylinder.Height, 0.01f);
        ImGui::DragFloat("x radius", &cylinder.RadiusX, 0.01f);
        ImGui::DragFloat("z radius", &cylinder.RadiusZ, 0.01f);
        cylinder.Height  = Max(cylinder.Height, 0.0f);
        cylinder.RadiusX = Max(cylinder.RadiusX, 0.0f);
        cylinder.RadiusZ = Max(cylinder.RadiusZ, 0.0f);

        bool axisX = cylinder.Orientation == Cylinder::Axis::X;
        bool axisY = cylinder.Orientation == Cylinder::Axis::Y;
        bool axisZ = cylinder.Orientation == Cylinder::Axis::Z;

        if (ImGui::BeginCombo("orientation", axisX ? "x axis" : (axisY ? "y axis" : "z axis")))
        {
            if (ImGui::Selectable("x axis", &axisX))
            {
                cylinder.Orientation = Cylinder::Axis::X;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable("y axis", &axisY))
            {
                cylinder.Orientation = Cylinder::Axis::Y;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable("z axis", &axisZ))
            {
                cylinder.Orientation = Cylinder::Axis::Z;
                ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    void DrawCapsuleEditor(const char* name, Capsule& capsule)
    {
        SCOPE_TREE_NODE(name);
        ImGui::DragFloat("height", &capsule.Height, 0.01f);
        ImGui::DragFloat("radius", &capsule.Radius, 0.01f);
        capsule.Height = Max(capsule.Height, 0.0f);
        capsule.Radius = Max(capsule.Radius, 0.0f);

        bool axisX = capsule.Orientation == Capsule::Axis::X;
        bool axisY = capsule.Orientation == Capsule::Axis::Y;
        bool axisZ = capsule.Orientation == Capsule::Axis::Z;

        if (ImGui::BeginCombo("orientation", axisX ? "x axis" : (axisY ? "y axis" : "z axis")))
        {
            if (ImGui::Selectable("x axis", &axisX))
            {
                capsule.Orientation = Capsule::Axis::X;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable("y axis", &axisY))
            {
                capsule.Orientation = Capsule::Axis::Y;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable("z axis", &axisZ))
            {
                capsule.Orientation = Capsule::Axis::Z;
                ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    void DrawVertexEditor(Vertex& vertex)
    {
        ImGui::InputFloat3("position", &vertex.Position[0]);
        ImGui::InputFloat2("texture", &vertex.TexCoord[0]);
        ImGui::InputFloat3("normal", &vertex.Normal[0]);
        ImGui::InputFloat3("tangent", &vertex.Tangent[0]);
        ImGui::InputFloat3("bitangent", &vertex.Bitangent[0]);
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
}