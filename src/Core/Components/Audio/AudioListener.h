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

#include "Utilities/Math/Math.h"
#include "Utilities/ECS/Component.h"

namespace MxEngine
{
    enum class SoundModel
    {
        NONE,
        INVERSE_DISTANCE,
        INVERSE_DISTANCE_CLAMPED,
        LINEAR_DISTANCE,
        LINEAR_DISTANCE_CLAMPED,
        EXPONENT_DISTANCE,
        EXPONENT_DISTANCE_CLAMPED,
    };

    class AudioListener
    {
        MAKE_COMPONENT(AudioListener);

        float volume = 1.0f;
        Vector3 velocity{ 0.0f, 0.0f, 0.0f };
        float soundSpeed = 343.3f;
        float dopplerFactor = 1.0f;
        SoundModel model = SoundModel::INVERSE_DISTANCE_CLAMPED;
    public:
        AudioListener() = default;
        void OnUpdate(float timeDelta);

        void SetPosition(const Vector3& position);
        void SetOrientation(const Vector3& direction, const Vector3& up);
        void SetVolume(float speed);
        void SetVelocity(const Vector3& velocity);
        void SetSoundSpeed(float value);
        void SetDopplerFactor(float factor);
        void SetSoundModel(SoundModel model);

        const Vector3& GetPosition() const;
        float GetVolume() const; 
        const Vector3& GetVelocity() const;
        float GetSoundSpeed() const;
        float GetDopplerFactor() const;
        SoundModel GetSoundModel() const;
    };
}