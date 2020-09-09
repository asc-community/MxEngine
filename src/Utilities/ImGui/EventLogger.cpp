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

#include "EventLogger.h"
#include "Core/Application/Event.h"
#include "Core/Events/Events.h"
#include "Utilities/Format/Format.h"
#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine
{
    void EventLogger::EndFrameCapture()
    {
        this->isFrameAlreadyCaptured = true;
    }

    EventLogger::EventLogger()
    {
        this->Init();
    }

    EventLogger::~EventLogger()
    {
        Event::RemoveEventListener("EventLogger");
    }

    void EventLogger::Init()
    {
        Event::AddEventListener<UpdateEvent>("EventLogger", [this](auto& e)
        {
            if(this->UpdateEvents)
                this->AddEventEntry("update event: " + ToMxString(e.TimeDelta) + "s");
        });

        Event::AddEventListener<FpsUpdateEvent>("EventLogger", [this](auto& e)
        {
            if(this->FpsUpdateEvents)
                this->AddEventEntry("fps update: " + ToMxString(e.FPS));
        });

        Event::AddEventListener<KeyEvent>("EventLogger", [this](auto& e)
        {
            for (size_t i = 0; i < 350; i++)
            {
                auto key = (KeyCode)i;
                if (e.IsPressed(key))
                    this->AddEventEntry("key pressed: " + (MxString)EnumToString(key));
                if (e.IsReleased(key))
                    this->AddEventEntry("key released: " + (MxString)EnumToString(key));
            }
        });

        Event::AddEventListener<MouseMoveEvent>("EventLogger", [this](auto& e)
        {
            if(this->MouseMoveEvents)
                this->AddEventEntry(MxFormat("mouse position: ({0}, {1})", e.position.x, e.position.y));
        });

        Event::AddEventListener<MousePressEvent>("EventLogger", [this](auto& e)
        {
            for (size_t i = 0; i < 8; i++)
            {
                auto button = (MouseButton)i;
                if(e.IsPressed(button))
                    this->AddEventEntry("mouse pressed: " + (MxString)EnumToString(button));
                if (e.IsReleased(button))
                    this->AddEventEntry("mouse released: " + (MxString)EnumToString(button));
            }
        });

        Event::AddEventListener<RenderEvent>("EventLogger", [this](auto& e)
        {
            if(this->RenderEvents)
                this->AddEventEntry("render event");
        });

        Event::AddEventListener<WindowResizeEvent>("EventLogger", [this](auto& e)
        {
            this->AddEventEntry(MxFormat("window resize: ({0}, {1})", e.New.x, e.New.y));
        });
    }

    void EventLogger::AddEventEntry(const MxString& entry)
    {
        if (this->isCurrentlyCapturing && this->isFrameAlreadyCaptured) return;

        this->entries.push_back(entry);
        if (this->entries.size() > 1024)
        {   // do not store too many event entries in list
            this->entries.erase(this->entries.begin(), this->entries.begin() + 1024 - 128);
        }
    }

    void EventLogger::Draw(const char* name, size_t lineCount)
    {
        SCOPE_TREE_NODE(name);

        this->entries.resize(Max(lineCount, this->entries.size()));
        auto first = this->entries.end() - lineCount;
        this->entries.erase(this->entries.begin(), first);

        ImGui::Checkbox("update events", &this->UpdateEvents);
        ImGui::SameLine();
        ImGui::Checkbox("render events", &this->RenderEvents);
        ImGui::SameLine();
        ImGui::Checkbox("fps update events", &this->FpsUpdateEvents);
        ImGui::SameLine();
        ImGui::Checkbox("mouse move events", &this->MouseMoveEvents);

        ImGui::Checkbox("auto-scroll", &this->autoScroll);
        ImGui::SameLine();

        // start/end capture logic between two game frames
        this->EndFrameCapture();
        if (ImGui::Button("capture frame"))
        {
            this->entries.clear();
            this->StartFrameCapture();
        }
        ImGui::SameLine();

        if (ImGui::Button("clear"))
        {
            this->entries.clear();
        }
        // if frame is captured, add logic to drop it and continue tracking events
        if (this->isCurrentlyCapturing)
        {
            ImGui::SameLine();
            if (ImGui::Button("drop captured frame"))
            {
                this->isCurrentlyCapturing = false;
            }
        }

        // create scrollable region for log output
        ImGui::BeginChild("ScrollingRegion", Vector2(0, 0), true);
        // if frame is captured, move cursor to the first line in log
        if (this->autoScroll && this->isCurrentlyCapturing)
        {
            ImGui::SetScrollHereY();
        }
        // display event log entries
        for (auto& entry : this->entries)
        {
            ImGui::Text("%s", entry.c_str());
        }
        // if frame is not captured, move cursor to the last log entry
        if (this->autoScroll && !this->isCurrentlyCapturing)
        {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    }

    void EventLogger::StartFrameCapture()
    {
        this->isCurrentlyCapturing = true;
        this->isFrameAlreadyCaptured = false;
    }
}