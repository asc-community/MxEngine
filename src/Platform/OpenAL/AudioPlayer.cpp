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

#include "AudioPlayer.h"
#include "ALUtilities.h"

namespace MxEngine
{
    AudioPlayer::AudioPlayer()
    {
        ALCALL(alGenSources(1, &id));
    }

    AudioPlayer::~AudioPlayer()
    {
        if (id != 0)
        {
            ALCALL(alDeleteSources(1, &id));
        }
    }

    AudioPlayer::BindableId AudioPlayer::GetNativeHandle() const
    {
        return id;
    }

    void AudioPlayer::AttachBuffer(const AudioBuffer& buffer)
    {
        ALCALL(alSourcei(id, AL_BUFFER, buffer.GetNativeHandle()));
    }

    void AudioPlayer::Play() const
    {
        ALCALL(alSourcePlay(id));
    }

    void AudioPlayer::Stop() const
    {
        ALCALL(alSourceStop(id));
    }

    void AudioPlayer::Pause() const
    {
        ALCALL(alSourcePause(id));
    }

    void AudioPlayer::Reset() const
    {
        alSourceRewind(id);
    }

    void AudioPlayer::SetLooping(bool value)
    {
        ALCALL(alSourcei(id, AL_LOOPING, value));
    }

    void AudioPlayer::SetVolume(float volume)
    {
        ALCALL(alSourcef(id, AL_GAIN, volume));
    }

    void AudioPlayer::SetOuterAngle(float angle)
    {
        ALCALL(alSourcef(id, AL_CONE_OUTER_ANGLE, angle));
    }

    void AudioPlayer::SetInnerAngle(float angle)
    {
        ALCALL(alSourcef(id, AL_CONE_INNER_ANGLE, angle));
    }

    void AudioPlayer::SetVelocity(float x, float y, float z)
    {
        ALCALL(alSource3f(id, AL_VELOCITY, x, y, z));
    }

    void AudioPlayer::SetPosition(float x, float y, float z)
    {
        ALCALL(alSource3f(id, AL_POSITION, x, y, z));
    }

    void AudioPlayer::SetDirection(float x, float y, float z)
    {
        ALCALL(alSource3f(id, AL_DIRECTION, x, y, z));
    }

    void AudioPlayer::SetSpeed(float speed)
    {
        ALCALL(alSourcef(id, AL_PITCH, speed));
    }

    void AudioPlayer::SetRollofFactor(float factor)
    {
        ALCALL(alSourcef(id, AL_ROLLOFF_FACTOR, factor));
    }

    void AudioPlayer::SetReferenceDistance(float distance)
    {
        ALCALL(alSourcef(id, AL_REFERENCE_DISTANCE, distance));
    }
}