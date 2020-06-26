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
#include "Utilities/ImGui/ImGuiBase.h"
#include "Core/Application/RenderManager.h"
#include "Core/Event/Events/WindowResizeEvent.h"
#include "Core/Application/EventManager.h"

namespace MxEngine::GUI
{
	void DrawViewportWindow(Vector2& viewportSize)
	{
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		auto viewport = RenderManager::GetViewport();

		if (viewport.IsValid() && viewport->GetRenderTexture().IsValid())
		{
			Vector2 newWindowSize = ImGui::GetWindowSize();
			if (newWindowSize != viewportSize) // notify application that viewport size has been changed
			{
				EventManager::AddEvent(MakeUnique<WindowResizeEvent>(viewportSize, newWindowSize));
				viewportSize = newWindowSize;
			}
			ImGui::SetCursorPos((newWindowSize - viewportSize) * 0.5f);
			ImGui::Image((void*)(uintptr_t)viewport->GetRenderTexture()->GetNativeHandle(), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		}
		ImGui::End();
	}
}