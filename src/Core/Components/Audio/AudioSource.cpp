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

#include "AudioSource.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void AudioSource::OnUpdate(float timeDelta)
    {
        auto position = MxObject::GetByComponent(*this).Transform.GetPosition();
        this->player->SetPosition(position.x, position.y, position.z);
    }

    void AudioSource::Init()
    {
        this->player = AudioFactory::Create<AudioPlayer>();
        this->SetDirection(this->GetDirection());
        if (this->buffer.IsValid())
            this->player->AttachBuffer(*this->buffer);
    }

    AudioSource::AudioSource(const AudioBufferHandle& buffer)
        : buffer(buffer) { }

    void AudioSource::Load(AudioBufferHandle buffer)
    {
        if (buffer.IsValid())
            player->AttachBuffer(*buffer);
        else
            player->DetachBuffer();
        this->buffer = std::move(buffer);
    }

    AudioBufferHandle AudioSource::GetLoadedSource() const
    {
        return this->buffer;
    }
    
    void AudioSource::Play()
    {
        this->isPlaying = true;
    	this->player->Play();
    }
    
    void AudioSource::Stop()
    {
        this->isPlaying = false;
    	this->player->Stop();
    }

    void AudioSource::Pause()
    {
        this->isPlaying = false;
        this->player->Pause();
    }

    void AudioSource::Reset()
    {
        this->isPlaying = false;
        this->player->Reset();
    }

    void AudioSource::Replay()
    {
        this->Reset();
        this->Play();
    }

    void AudioSource::SetPlaying(bool isPlaying)
    {
        isPlaying ? this->Play() : this->Pause();
    }
    
    void AudioSource::SetVolume(float volume)
    {
        this->currentVolume = Clamp(volume, 0.0f, 1.0f);
    	this->player->SetVolume(this->currentVolume);
    }

    void AudioSource::SetLooping(bool value)
    {
        this->isLooping = value;
        this->player->SetLooping(this->isLooping);
    }

    void AudioSource::SetRelative(bool value)
    {
        this->isRelative = value;
        this->player->SetRelative(this->isRelative);
    }

    void AudioSource::SetPlaybackSpeed(float speed)
    {
        this->currentSpeed = speed;
        this->player->SetSpeed(this->currentSpeed);
    }

    void AudioSource::SetVelocity(const Vector3& velocity)
    {
        this->velocity = velocity;
        this->player->SetVelocity(this->velocity.x, this->velocity.y, this->velocity.z);
    }

    void AudioSource::SetDirection(const Vector3& direction)
    {
        if (Length(direction) < 0.01f)
            this->direction = MakeVector3(0.0f, 0.0f, 1.0f);
        else
            this->direction = direction;

        auto normalized = Normalize(this->direction);
        this->player->SetDirection(normalized.x, normalized.y, normalized.z);
    }

    void AudioSource::SetOuterAngle(float angle)
    {
        this->outerAngle = Clamp(angle, 0.0f, 360.0f);
        this->SetInnerAngle(this->GetInnerAngle());
        this->player->SetOuterAngle(this->outerAngle);
    }

    void AudioSource::SetInnerAngle(float angle)
    {
        this->innerAngle = Clamp(angle, 0.0f, this->outerAngle);
        this->player->SetInnerAngle(this->innerAngle);
    }

    void AudioSource::SetOuterAngleVolume(float volume)
    {
        this->outerAngleVolume = Clamp(volume, 0.0f, 1.0f);
        this->player->SetOuterAngleVolume(this->outerAngleVolume);
    }

    void AudioSource::SetRollofFactor(float factor)
    {
        this->rollofFactor = Max(0.0f, factor);
        this->player->SetRollofFactor(this->rollofFactor);
    }

    void AudioSource::SetReferenceDistance(float distance)
    {
        this->referenceDistance = Max(0.0f, distance);
        this->player->SetReferenceDistance(this->referenceDistance);
    }

    bool AudioSource::IsLooping() const
    {
        return this->isLooping;
    }

    bool AudioSource::IsPlaying() const
    {
        return this->isPlaying;
    }

    bool AudioSource::IsRelative() const
    {
        return this->isRelative;
    }

    float AudioSource::GetVolume() const
    {
        return this->currentVolume;
    }

    float AudioSource::GetPlaybackSpeed() const
    {
        return this->currentSpeed;
    }

    float AudioSource::GetOuterAngleVolume() const
    {
        return this->outerAngleVolume;
    }

    float AudioSource::GetOuterAngle() const
    {
        return this->outerAngle;
    }

    float AudioSource::GetInnerAngle() const
    {
        return this->innerAngle;
    }

    void AudioSource::MakeOmnidirectional()
    {
        this->SetOuterAngle(360.0f);
        this->SetInnerAngle(360.0f);
    }

    bool AudioSource::IsOmnidirectional() const
    {
        return this->outerAngle == 360.0f; //-V550
    }

    const Vector3& AudioSource::GetVelocity() const
    {
        return this->velocity;
    }

    const Vector3& AudioSource::GetDirection() const
    {
        return this->direction;
    }

    float AudioSource::GetRollofFactor() const
    {
        return this->rollofFactor;
    }

    float AudioSource::GetReferenceDistance() const
    {
        return this->referenceDistance;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<AudioSource>("AudioSource")
            .constructor<>()
            .method("replay", &AudioSource::Replay)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .method("stop", &AudioSource::Stop)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .method("make omnidirectional", &AudioSource::MakeOmnidirectional)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("is playing", &AudioSource::IsPlaying, &AudioSource::SetPlaying)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is looping", &AudioSource::IsLooping, &AudioSource::SetLooping)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is relative", &AudioSource::IsRelative, &AudioSource::SetRelative)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property_readonly("is omnidirectional", &AudioSource::IsOmnidirectional)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("volume", &AudioSource::GetVolume, &AudioSource::SetVolume)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("outer angle", &AudioSource::GetOuterAngle, &AudioSource::SetOuterAngle)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 360.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("inner angle", &AudioSource::GetInnerAngle, &AudioSource::SetInnerAngle)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 360.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("playback speed", &AudioSource::GetPlaybackSpeed, &AudioSource::SetPlaybackSpeed)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("velocity", &AudioSource::GetVelocity, &AudioSource::SetVelocity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("direction", &AudioSource::GetDirection, &AudioSource::SetDirection)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("source", &AudioSource::GetLoadedSource, &AudioSource::Load)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("outer angle volume", &AudioSource::GetOuterAngleVolume, &AudioSource::SetOuterAngleVolume)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("reference distance", &AudioSource::GetReferenceDistance, &AudioSource::SetReferenceDistance)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("rollof factor", &AudioSource::GetRollofFactor, &AudioSource::SetRollofFactor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );
    }
}