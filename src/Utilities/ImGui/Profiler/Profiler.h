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
#include "Core/Event/FpsUpdateEvent.h"

namespace MxEngine::GUI
{
	inline void DrawProfiler(size_t graphRecordSize = 128)
	{
		static std::vector<float> fpsData;
		static size_t curPointer = 0;
		fpsData.resize(graphRecordSize);
		auto context = Application::Get();
		
		INVOKE_ONCE(context->GetEventDispatcher().AddEventListener<FpsUpdateEvent>("FpsGraph", [graphRecordSize](FpsUpdateEvent& e)
			{
				fpsData[curPointer] = static_cast<float>(e.FPS);
				curPointer = (curPointer + 1) % (int)graphRecordSize;
				if (curPointer == 0) std::fill(fpsData.begin(), fpsData.end(), 0.0f);
			}));

		ImGui::PlotLines("", fpsData.data(), (int)graphRecordSize, 0, "FPS profiler", 
			FLT_MAX, FLT_MAX, { ImGui::GetWindowWidth() - 15.0f, graphRecordSize + 15.0f });
	}
}