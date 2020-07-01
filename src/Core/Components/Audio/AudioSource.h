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
#include "Platform/AudioAPI.h"
#include "Utilities/ECS/Component.h"

namespace MxEngine
{
	class AudioSource
	{
		MAKE_COMPONENT(AudioSource);

		AudioPlayerHandle player;
		AudioBufferHandle buffer;
		float currentVolume = 1.0f;
		float currentSpeed = 1.0f;
		Vector3 velocity = MakeVector3(0.0f);
		Vector3 direction = MakeVector3(0.0f, 0.0f, 1.0f);
		float outerAngleVolume = 0.0f;
		float outerAngle = 360.0f;
		float innerAngle = 360.0f;
		float rollofFactor = 1.0f;
		float referenceDistance = 1.0f;
		bool isLooping = false;
		bool isPlaying = false;
		bool isRelative = false;
	public:
		void OnUpdate();
		void Init();
		~AudioSource();
		AudioSource() = default;
		AudioSource(const AudioBufferHandle& buffer);

		void Load(const AudioBufferHandle& buffer);
		AudioBufferHandle GetLoadedSource() const;

		void Play();
		void Stop();
		void Pause();
		void Reset();
		void Replay();
		void SetVolume(float volume);
		void SetLooping(bool value);
		void SetRelative(bool value);
		void SetPlaybackSpeed(float speed);
		void SetVelocity(const Vector3& velocity);
		void SetDirection(const Vector3& direction);
		void SetOuterAngle(float angle);
		void SetInnerAngle(float angle);
		void SetOuterAngleVolume(float volume);
		void SetRollofFactor(float factor);
		void SetReferenceDistance(float distance);
		void MakeOmnidirectional();
		bool IsOmnidirectional() const;
		bool IsLooping() const;
		bool IsPlaying() const;
		bool IsRelative() const;
		float GetVolume() const;
		float GetSpeed() const;
		float GetOuterAngleVolume() const;
		float GetOuterAngle() const;
		float GetInnerAngle() const;
		const Vector3& GetVelocity() const;
		const Vector3& GetDirection() const;
		float GetRollofFactor() const;
		float GetReferenceDistance() const;
	};
}