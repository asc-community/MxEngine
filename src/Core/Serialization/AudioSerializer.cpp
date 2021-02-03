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

#include "Core/Serialization/SceneSerializer.h"
#include "Core/Components/Audio/AudioListener.h"
#include "Core/Components/Audio/AudioSource.h"
#include "Core/Serialization/DeserializerMappings.h"

namespace MxEngine
{
    void Serialize(JsonFile& json, const AudioListener& listener)
    {
        json["doppler"] = listener.GetDopplerFactor();
        json["sound-model"] = listener.GetSoundModel();
        json["sound-speed"] = listener.GetSoundSpeed();
        json["velocity"] = listener.GetVelocity();
        json["volume"] = listener.GetVolume();
    }

    void Serialize(JsonFile& json, const AudioSource& source)
    {
        auto buffer = source.GetLoadedSource();
        json["source-id"] = buffer.IsValid() ? buffer.GetHandle() : size_t(-1);
        json["is-relative"] = source.IsRelative();
        json["is-looping"] = source.IsLooping();
        json["is-playing"] = source.IsPlaying();
        json["direction"] = source.GetDirection();
        json["inner-angle"] = source.GetInnerAngle();
        json["outer-angle"] = source.GetOuterAngle();
        json["outer-angle-volume"] = source.GetOuterAngleVolume();
        json["reference-distance"] = source.GetReferenceDistance();
        json["rollof-factor"] = source.GetRollofFactor();
        json["speed"] = source.GetPlaybackSpeed();
        json["velocity"] = source.GetVelocity();
        json["volume"] = source.GetVolume();
    }

    void Deserialize(const JsonFile& json, DeserializerMappings& mappings, AudioListener& listener)
    {
        listener.SetDopplerFactor(json["doppler"]);
        listener.SetSoundModel(json["sound-model"]);
        listener.SetSoundSpeed(json["sound-speed"]);
        listener.SetVelocity(json["velocity"]);
        listener.SetVolume(json["volume"]);
    }

    void Deserialize(const JsonFile& json, DeserializerMappings& mappings, AudioSource& source)
    {
        auto buffer = mappings.AudioBuffers[json["source-id"]];
        source.Load(buffer);

        bool(json["is-playing"]) ? source.Play() : source.Pause();

        source.SetRelative(json["is-relative"]);
        source.SetLooping(json["is-looping"]);
        source.SetDirection(json["direction"]);
        source.SetInnerAngle(json["inner-angle"]);
        source.SetOuterAngle(json["outer-angle"]);
        source.SetOuterAngleVolume(json["outer-angle-volume"]);
        source.SetReferenceDistance(json["reference-distance"]);
        source.SetRollofFactor(json["rollof-factor"]);
        source.SetPlaybackSpeed(json["speed"]);
        source.SetVelocity(json["velocity"]);
        source.SetVolume(json["volume"]);
    }
}