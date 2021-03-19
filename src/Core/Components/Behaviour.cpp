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

#include "Behaviour.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void Behaviour::InvokeUserBehaviour(TimeDelta dt)
    {
        if(this->userBehaviour)
            this->userBehaviour(MxObject::GetByComponent(*this), dt);
    }

    Behaviour::~Behaviour()
    {
        this->RemoveBehaviour();
    }

    void Behaviour::OnUpdate(TimeDelta dt)
    {
        this->timeLeft -= dt;

        switch (this->timerMode)
        {
        case TimerMode::UPDATE_EACH_FRAME:
            this->InvokeUserBehaviour(dt);
            break;
        case TimerMode::UPDATE_EACH_DELTA:
            if (this->timeLeft <= 0.0f)
            {
                this->timeLeft = this->timeRequested;
                this->InvokeUserBehaviour(dt);
            }
            break;
        case TimerMode::UPDATE_AFTER_DELTA:
            if (this->timeLeft <= 0.0f)
            {
                this->timeLeft = std::numeric_limits<TimeDelta>::infinity();
                this->InvokeUserBehaviour(dt);
            }
            break;
        case TimerMode::UPDATE_FOR_N_SECONDS:
            if (this->timeLeft > 0.0f)
                this->InvokeUserBehaviour(dt);
            break;
        default:
            // do nothing
            break;
        }
    }

    void Behaviour::RemoveBehaviour()
    {
        this->userBehaviour = { };
    }

    bool Behaviour::HasBehaviour() const
    {
        return this->userBehaviour != nullptr;
    }

    void Behaviour::Schedule(TimerMode timer, TimeDelta seconds)
    {
        this->timerMode = timer;
        this->timeRequested = seconds;
        this->timeLeft = this->timeRequested;
    }

    void Behaviour::Pause()
    {
        this->Schedule(TimerMode::UPDATE_AFTER_DELTA, std::numeric_limits<TimeDelta>::infinity());
    }

    Behaviour::TimeDelta Behaviour::GetTimeLeft() const
    {
        return Max(this->timeLeft, 0.0f);
    }

    Behaviour::TimeDelta Behaviour::GetTimeRequest() const
    {
        return this->timeRequested;
    }

    TimerMode Behaviour::GetTimerMode() const
    {
        return this->timerMode;
    }

    void Behaviour::SetTimerModeInternal(TimerMode mode)
    {
        this->timerMode = mode;
    }

    void Behaviour::SetTimeLeftInternal(TimeDelta time)
    {
        this->timeLeft = Max(time, 0.0f);
    }

    void Behaviour::SetTimeRequestInternal(TimeDelta time)
    {
        this->timeRequested = Max(time, 0.0f);
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::enumeration<TimerMode>("TimerMode")
            (
                rttr::value("update after delta",   TimerMode::UPDATE_AFTER_DELTA),
                rttr::value("update each delta",    TimerMode::UPDATE_EACH_DELTA),
                rttr::value("update each frame",    TimerMode::UPDATE_EACH_FRAME),
                rttr::value("update for n seconds", TimerMode::UPDATE_FOR_N_SECONDS)
            );

        rttr::registration::class_<Behaviour>("Behaviour")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("tag", &Behaviour::Tag)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property_readonly("has update callback", &Behaviour::HasBehaviour)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("time left", &Behaviour::GetTimeLeft, &Behaviour::SetTimeLeftInternal)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("time request", &Behaviour::GetTimeRequest, &Behaviour::SetTimeRequestInternal)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("timer mode", &Behaviour::GetTimerMode, &Behaviour::SetTimerModeInternal)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .method("pause", &Behaviour::Pause)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .method("remove callback", &Behaviour::RemoveBehaviour)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            );
    }
}