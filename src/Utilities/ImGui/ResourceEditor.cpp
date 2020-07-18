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
#include "ImGuiUtils.h"
#include "ComponentEditor.h"
#include "Core/BoundingObjects/Cylinder.h"
#include "Core/BoundingObjects/Capsule.h"
#include "Core/BoundingObjects/BoundingBox.h"
#include "Utilities/Image/ImageManager.h"

namespace MxEngine::GUI
{
    void DrawTextureEditor(const char* name, TextureHandle& texture)
    {
        SCOPE_TREE_NODE(name);

        if (texture.IsValid())
        {
            ImGui::Text("path: %s", texture->GetPath().c_str());
            ImGui::Text("width: %d", (int)texture->GetWidth());
            ImGui::Text("height: %d", (int)texture->GetHeight());
            ImGui::Text("channels: %d", (int)texture->GetChannelCount());
            ImGui::Text("samples: %d", (int)texture->GetSampleCount());
            ImGui::Text("format: %s", EnumToString(texture->GetFormat())); //-V111
            ImGui::Text("wrap type: %s", EnumToString(texture->GetWrapType())); //-V111
            ImGui::Text("native render type: %d", (int)texture->GetTextureType());
            ImGui::Text("native handle: %d", (int)texture->GetNativeHandle());
            ImGui::Text("is depth-only: %s", BOOL_STRING(texture->IsDepthOnly()));
            ImGui::Text("is multisampled: %s", BOOL_STRING(texture->IsMultisampled()));
        }
        else
        {
            ImGui::Text("empty resource");
        }

        static MxString path;
        if (GUI::InputTextOnClick(nullptr, path, 128, "load texture"))
            texture = AssetManager::LoadTexture(path);
        // TODO: support textures from Colors class

        if (texture.IsValid())
        {   
            DrawImageSaver(texture);

            auto nativeHeight = texture->GetHeight();
            auto nativeWidth = texture->GetWidth();

            static Vector3 color{ 1.0f };
            if (ImGui::ColorEdit3("border color", &color[0]))
                texture->SetBorderColor(color);

            static float scale = 1.0f;
            ImGui::DragFloat("texture preview scale", &scale, 0.01f, 0.0f, 1.0f);
            auto width = ImGui::GetWindowSize().x * 0.9f * scale;
            auto height = width * nativeHeight / nativeWidth;
            ImGui::Image((void*)(uintptr_t)texture->GetNativeHandle(), ImVec2(width, height), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        }
    }

    void DrawCubeMapEditor(const char* name, CubeMapHandle& cubemap)
    {
        SCOPE_TREE_NODE(name);

        if (cubemap.IsValid())
        {
            ImGui::Text("path: %s", cubemap->GetPath().c_str());
            ImGui::Text("width: %d", (int)cubemap->GetWidth());
            ImGui::Text("height: %d", (int)cubemap->GetHeight());
            ImGui::Text("channels: %d", (int)cubemap->GetChannelCount());
            ImGui::Text("native handle: %d", (int)cubemap->GetNativeHandle());
        }
        else
        {
            ImGui::Text("empty resource");
        }
        
        static MxString path;
        if (GUI::InputTextOnClick(nullptr, path, 128, "load cubemap"))
            cubemap = AssetManager::LoadCubeMap(path);
        // TODO: support cubemap preview
    }

    void DrawMaterialEditor(MaterialHandle& material)
    {
        if (!material.IsValid())
        {
            GUI::Indent _(5.0f);
            ImGui::Text("empty material");
            return;
        }
        SCOPE_TREE_NODE(material->Name.c_str());

        DrawTextureEditor("albedo map", material->AlbedoMap);
        DrawTextureEditor("specular map", material->SpecularMap);
        DrawTextureEditor("emmisive map", material->EmmisiveMap);
        DrawTextureEditor("normal map", material->NormalMap);
        DrawTextureEditor("height map", material->HeightMap);

        ImGui::DragFloat("specular exponent", &material->SpecularExponent, 1.0f, 1.0f, 10000.0f);
        ImGui::DragFloat("transparency", &material->Transparency, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("displacement", &material->Displacement, 0.01f);
        ImGui::DragFloat("reflection", &material->Reflection, 0.01f, 0.0f, 1.0f);
        ImGui::ColorEdit3("ambient color", &material->AmbientColor[0], ImGuiColorEditFlags_HDR);
        ImGui::ColorEdit3("diffuse color", &material->DiffuseColor[0], ImGuiColorEditFlags_HDR);
        ImGui::ColorEdit3("specular color", &material->SpecularColor[0], ImGuiColorEditFlags_HDR);
        ImGui::ColorEdit3("emmisive color", &material->EmmisiveColor[0], ImGuiColorEditFlags_HDR);
        ImGui::ColorEdit3("base color", &material->BaseColor[0], ImGuiColorEditFlags_HDR);
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
        ImGui::DragFloat3("min", &box.Min[0], 0.01f);
        ImGui::DragFloat3("max", &box.Max[0], 0.01f);
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
            if (ImGui::Selectable("y axis", &axisX))
            {
                cylinder.Orientation = Cylinder::Axis::Y;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable("z axis", &axisX))
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
            if (ImGui::Selectable("y axis", &axisX))
            {
                capsule.Orientation = Capsule::Axis::Y;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable("z axis", &axisX))
            {
                capsule.Orientation = Capsule::Axis::Z;
                ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    void DrawLightBaseEditor(LightBase& base)
    {
        ImGui::ColorEdit3("ambient color",  &base.AmbientColor[0],  ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("diffuse color",  &base.DiffuseColor[0],  ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("specular color", &base.SpecularColor[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);

        base.AmbientColor  = VectorMax(MakeVector3(0.0f), base.AmbientColor );
        base.DiffuseColor  = VectorMax(MakeVector3(0.0f), base.DiffuseColor );
        base.SpecularColor = VectorMax(MakeVector3(0.0f), base.SpecularColor);
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
        static MxString path(128, '\0');
        ImGui::InputText("save path", path.data(), path.size());

        const char* imageTypes[] = { "PNG", "BMP", "TGA", "JPG", "HDR", };
        static int currentImageType = 0;
        if (ImGui::Button("save image to disk"))
            ImageManager::SaveTexture(path.c_str(), texture, (ImageType)currentImageType);
        ImGui::SameLine();
        ImGui::PushItemWidth(50.0f);
        ImGui::Combo(" ", &currentImageType, imageTypes, (int)std::size(imageTypes));
        ImGui::PopItemWidth();
    }

    void DrawMeshEditor(const char* name, MeshHandle& mesh)
    {
        SCOPE_TREE_NODE(name);

        auto aabb = mesh->GetBoundingBox();
        auto sphere = mesh->GetBoundingSphere();
        DrawAABBEditor("bounding box", aabb);
        DrawSphereEditor("bounding sphere", sphere);
        mesh->SetBoundingBox(aabb);
        mesh->SetBoundingSphere(sphere);

        if (ImGui::Button("update mesh boundings"))
            mesh->UpdateBoundingGeometry();

        static MxString path;
        if (GUI::InputTextOnClick("", path, 128, "load mesh"))
            mesh = AssetManager::LoadMesh(path);
        // TODO: support meshes from Primitives class

        static MxString submeshName;
        
        size_t vertexCount = 0, indexCount = 0;
        for (auto& submesh : mesh->GetSubmeshes())
        {
            vertexCount += submesh.MeshData.GetVertecies().size();
            indexCount += submesh.MeshData.GetIndicies().size();
        }
        ImGui::Text("total vertex count: %d, total index count: %d", (int)vertexCount, (int)indexCount);

        int id = 0;
        for (auto& submesh : mesh->GetSubmeshes())
        {
            if (!ImGui::CollapsingHeader(submesh.Name.c_str())) continue;
            GUI::Indent _(5.0f);
            ImGui::PushID(id++);

            ImGui::Text("vertex count: %d", (int)submesh.MeshData.GetVertecies().size());
            ImGui::Text("index count: %d", (int)submesh.MeshData.GetIndicies().size());
            ImGui::Text("material id: %d", (int)submesh.GetMaterialId());

            TransformEditor(*submesh.GetTransform());

            if (ImGui::Button("update submesh boundings"))
                submesh.MeshData.UpdateBoundingGeometry();
            ImGui::SameLine();
            if (ImGui::Button("buffer vertecies"))
                submesh.MeshData.BufferVertecies();
            ImGui::SameLine();
            if (ImGui::Button("free mesh data copy"))
                submesh.MeshData.FreeMeshDataCopy();

            if (ImGui::Button("regenerate normals"))
                submesh.MeshData.RegenerateNormals();
            ImGui::SameLine();
            if (ImGui::Button("regenerate tangents"))
                submesh.MeshData.RegenerateTangentSpace();

            // TODO: maybe add indicies editor?
            {
                if (ImGui::CollapsingHeader("vertecies"))
                {
                    GUI::Indent _(5.0f);
                    int id = 0;
                    for (auto& vertex : submesh.MeshData.GetVertecies())
                    {
                        ImGui::PushID(id++);
                        DrawVertexEditor(vertex);
                        ImGui::Separator();
                        ImGui::PopID();
                    }
                }
            }
            ImGui::PopID();
        }
    }
}