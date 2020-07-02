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

#include "Utilities/ECS/Component.h"
#include "Utilities/STL/MxFunction.h"

namespace MxEngine
{
    class MxObject;

    class Behaviour
    {
        MAKE_COMPONENT(Behaviour);

        using TimeDelta = float;
        using UpdateCallbackType = MxFunction<void(void*, MxObject&, TimeDelta)>::type;
        using DeleteCallbackType = MxFunction<void(void*)>::type;

        UpdateCallbackType updateCallback;
        DeleteCallbackType deleteCallback;
        void* userBehaviour = nullptr;
    public:

        Behaviour() = default;
        ~Behaviour();

        void Init();
        void InvokeUpdate(TimeDelta dt);
        void RemoveBehaviour();
        bool HasBehaviour() const;

        template<typename T>
        Behaviour(T&& customBehaviour)
        {
            this->SetBehaviour(std::forward<T>(customBehaviour));
        }

        template<typename T>
        void SetBehaviour(T&& customBehaviour)
        {
            static_assert(!std::is_reference_v<T>, "passing reference to object as behaviour is prohibited");

            this->RemoveBehaviour();

            this->userBehaviour = std::malloc(sizeof(T));
            auto* _ = new(this->userBehaviour) T(std::move(customBehaviour)); //-V799

            this->updateCallback = [](void* behaviour, MxObject& self, TimeDelta dt) mutable { ((T*)behaviour)->OnUpdate(self, dt); };
            this->deleteCallback = [](void* behaviour) { reinterpret_cast<T*>(behaviour)->~T(); };
        }

        template<typename T>
        T& GetBehaviour()
        {
            MX_ASSERT(this->HasBehaviour());
            return *reinterpret_cast<T*>(this->userBehaviour);
        }

        template<typename T>
        const T& GetBehaviour() const
        {
            MX_ASSERT(this->HasBehaviour());
            return *reinterpret_cast<const T*>(this->userBehaviour);
        }
    };
}