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

#pragma once

#include "Core/Components/Behaviour.h"
#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
    class Timer
    {
    public:
        template<typename F>
        static MxObject::Handle Schedule(F&& func, TimerMode mode = TimerMode::UPDATE_EACH_FRAME, float timeInSeconds = 0.0f)
        {
            auto object = MxObject::Create();
            object->IsSerialized = false;
            object->SetDisplayInRuntimeEditor(false);
            object->Name = "__Timer";
            auto behaviour = object->AddComponent<Behaviour>(
            [callback = std::forward<F>(func)](MxObject& self, float dt) mutable
            {
                callback();
                // destroy object if timer was set to single update mode
                auto& behaviour = *self.GetComponent<Behaviour>();
                bool shouldDestroy = (behaviour.GetTimerMode() == TimerMode::UPDATE_AFTER_DELTA) &&
                                     (behaviour.GetTimeLeft() <= 0.0f);

                if(shouldDestroy) MxObject::Destroy(self);
            });
            behaviour->Schedule(mode, timeInSeconds);
            return object;
        }

        template<typename F>
        static MxObject::Handle CallEachFrame(F&& func)
        {
            return Timer::Schedule(std::forward<F>(func), TimerMode::UPDATE_EACH_FRAME);
        }

        template<typename F>
        static MxObject::Handle CallEachDelta(F&& func, float delta)
        {
            return Timer::Schedule(std::forward<F>(func), TimerMode::UPDATE_EACH_DELTA, delta);
        }

        template<typename F>
        static MxObject::Handle CallAfterDelta(F&& func, float delta)
        {
            return Timer::Schedule(std::forward<F>(func), TimerMode::UPDATE_AFTER_DELTA, delta);
        }

        template<typename F>
        static MxObject::Handle Repeat(F&& func, float duration)
        {
            return Timer::Schedule(std::forward<F>(func), TimerMode::UPDATE_FOR_N_SECONDS, duration);
        }
        
        template<typename F>
        static MxObject::Handle RepeatAfterDelta(F&& func, float delta, float duration)
        {
            return Timer::CallAfterDelta([f = std::forward<F>(func), duration]() { Timer::Repeat(std::move(f), duration); }, delta);
        }
    };
}