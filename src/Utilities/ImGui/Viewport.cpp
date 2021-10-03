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

#include "Viewport.h"
#include "Core/Application/Rendering.h"
#include "Core/Events/WindowResizeEvent.h"
#include "Core/Application/Event.h"
#include "Utilities/ImGui/ImGuiBase.h"
#include "Utilities/ImGui/Editors/MxObjectEditor.h"
#include "Core/Components/Instancing/Instance.h"

namespace MxEngine::GUI
{
    void DrawViewportWindow(const char* name, Vector2& viewportSize, Vector2& viewportPosition, bool* isOpen)
    {
        ImGui::Begin(name, isOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        auto viewport = Rendering::GetViewport();

        Vector2 newWindowSize = ImGui::GetWindowSize();

        // if (viewport.IsValid() && viewport->GetRenderTexture().IsValid())
        // {
        //     Vector2 newWindowSize = ImGui::GetWindowSize();
        //     if (newWindowSize != viewportSize) // notify application that viewport size has been changed
        //     {
        //         Event::AddEvent(MakeUnique<WindowResizeEvent>(viewportSize, newWindowSize));
        //         viewportSize = newWindowSize;
        //     }
        //     viewportPosition = (newWindowSize - viewportSize) * 0.5f;
        //     ImGui::SetCursorPos(viewportPosition);
        //     ImGui::Image((void*)(uintptr_t)viewport->GetRenderTexture()->GetNativeHandle(), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        // }
        ImGui::End();
    }

    bool IntersectRay(const Vector3& rayOrigin, const Vector3& rayDirection, const BoundingBox& box)
    {
        Vector3 rd = box.Rotation * rayDirection;
        Vector3 ro = box.Rotation * (rayOrigin - box.Center);

        Vector3 m = Vector3(1.0f) / rd;

        Vector3 s = Vector3(
            (rd.x < 0.0f) ? 1.0f : -1.0f,
            (rd.y < 0.0f) ? 1.0f : -1.0f,
            (rd.z < 0.0f) ? 1.0f : -1.0f
        );
        Vector3 t1 = m * (-ro + s * box.Length() * 0.5f);
        Vector3 t2 = m * (-ro - s * box.Length() * 0.5f);

        float tN = Max(Max(t1.x, t1.y), t1.z);
        float tF = Min(Min(t2.x, t2.y), t2.z);

        return (tF > 0.0f && tN < tF);
    }

    MxVector<MxObject::Handle> MousePeekObjects(
        const Vector2& mousePosition, const Vector2& viewportPosition, const Vector2& viewportSize,
        const Vector3& position, const Vector3& direction, const Vector3& up, float aspectRatio, float fov)
    {
        MxVector<MxObject::Handle> result;
        Vector2 screenSpaceCoords = 0.5f - (mousePosition - viewportPosition) / viewportSize;

        if (screenSpaceCoords.x < -0.5f || screenSpaceCoords.y < -0.5f ||
            screenSpaceCoords.x >  0.5f || screenSpaceCoords.y >  0.5f) return result;

        Vector2 angleDiff = screenSpaceCoords * (fov / 30.0f) * Vector2(aspectRatio, 1.0f) * std::tan(Radians(fov) * 0.5f);
        Vector3 rayDirection = Normalize(Vector3(angleDiff, 1.0f));
        Vector3 right = Normalize(Cross(up, direction));
        auto viewToWorld = Matrix3x3(right, up, direction);
        Vector3 worldSpaceDirection = viewToWorld * rayDirection;

        for (const auto& object : MxObject::GetObjects())
        {
            if (IsInstanced(object) || !object.IsDisplayedInEditor) continue; // skip invisible object factories

            BoundingBox box = GetMxObjectBoundingBox(object);
            if (IntersectRay(position, worldSpaceDirection, box))
                result.push_back(MxObject::GetHandle(object));
        }
        std::sort(result.begin(), result.end(), 
            [position](const MxObject::Handle& obj1, const MxObject::Handle& obj2)
            {
                Vector3 d1 = obj1->LocalTransform.GetPosition() - position;
                Vector3 d2 = obj2->LocalTransform.GetPosition() - position;
                return Dot(d1, d1) < Dot(d2, d2);
            });
        return result;
    }
}