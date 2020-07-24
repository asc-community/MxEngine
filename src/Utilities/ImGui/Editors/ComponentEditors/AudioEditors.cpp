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

#include "Core/Components/Audio/AudioListener.h"
#include "Core/Components/Audio/AudioSource.h"

#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	void AudioSourceEditor(AudioSource& audioSource)
	{
		TREE_NODE_PUSH("AudioSource");
		REMOVE_COMPONENT_BUTTON(audioSource);

		if (ImGui::TreeNode("source"))
		{
			auto source = audioSource.GetLoadedSource();
			static MxString path;
			if (GUI::InputTextOnClick(nullptr, path, 128, "load audio"))
				audioSource.Load(AssetManager::LoadAudio(path));


			if (!source.IsValid())
			{
				ImGui::Text("no audio source loaded");
			}
			else
			{
				ImGui::Text("native handle: %d", (int)source->GetNativeHandle());
				ImGui::Text("native format: %d", (int)source->GetNativeFormat());
				ImGui::Text("audio format: %s", EnumToString(source->GetAudioType())); //-V111
				ImGui::Text("channel count: %d", (int)source->GetChannelCount());
				ImGui::Text("length (in seconds): %d", int(source->GetSampleCount() / source->GetFrequency()));
				ImGui::Text("sample count: %d", (int)source->GetSampleCount());
				ImGui::Text("sampling frequency: %d", (int)source->GetFrequency());
				ImGui::Text("path to file: %s", source->GetFilePath().c_str());
			}
			ImGui::TreePop();
		}

		auto isLooping = audioSource.IsLooping();
		auto isPlaying = audioSource.IsPlaying();
		auto isRelative = audioSource.IsRelative();
		auto omnidirectional = audioSource.IsOmnidirectional();
		auto volume = audioSource.GetVolume();
		auto speed = audioSource.GetSpeed();
		auto coneVolume = audioSource.GetOuterAngleVolume();
		auto velocity = audioSource.GetVelocity();
		auto direction = audioSource.GetDirection();
		auto outerAngle = audioSource.GetOuterAngle();
		auto innerAngle = audioSource.GetInnerAngle();
		auto rollofFactor = audioSource.GetRollofFactor();
		auto referenceDistance = audioSource.GetReferenceDistance();


		if (ImGui::Checkbox("is looping", &isLooping))
			audioSource.SetLooping(isLooping);
		ImGui::SameLine();
		if (ImGui::Checkbox("is relative", &isRelative))
			audioSource.SetRelative(isRelative);

		ImGui::AlignTextToFramePadding();
		ImGui::Text("is omnidirectional: %s", BOOL_STRING(omnidirectional));
		ImGui::SameLine();
		if (ImGui::Button("make omnidirectional"))
			audioSource.MakeOmnidirectional();

		if (ImGui::Button("play"))
			audioSource.Play();
		ImGui::SameLine();
		if (ImGui::Button("reset"))
			audioSource.Reset();
		ImGui::SameLine();
		if (ImGui::Button("replay"))
			audioSource.Replay();
		ImGui::SameLine();
		if (ImGui::Button("stop"))
			audioSource.Stop();
		ImGui::SameLine();
		if (ImGui::Button("pause"))
			audioSource.Stop();

		if (ImGui::DragFloat3("direction", &direction[0], 0.01f))
			audioSource.SetDirection(direction);

		if (ImGui::DragFloat3("velocity", &velocity[0], 0.01f))
			audioSource.SetVelocity(velocity);

		if (ImGui::DragFloat("volume", &volume, 0.001f))
			audioSource.SetVolume(volume);

		if (ImGui::DragFloat("playback speed", &speed, 0.001f))
			audioSource.SetPlaybackSpeed(speed);

		if (ImGui::DragFloat("outer angle", &outerAngle))
			audioSource.SetOuterAngle(outerAngle);

		if (ImGui::DragFloat("inner angle", &innerAngle))
			audioSource.SetInnerAngle(innerAngle);

		if (ImGui::TreeNode("other settings"))
		{
			if (ImGui::DragFloat("outer angle volume", &coneVolume))
				audioSource.SetOuterAngleVolume(coneVolume);

			if (ImGui::DragFloat("rollof factor", &rollofFactor))
				audioSource.SetRollofFactor(rollofFactor);

			if (ImGui::DragFloat("reference distance", &referenceDistance))
				audioSource.SetReferenceDistance(referenceDistance);
			ImGui::TreePop();
		}
	}

	void AudioListenerEditor(AudioListener& audioListener)
	{
		TREE_NODE_PUSH("AudioListener");
		REMOVE_COMPONENT_BUTTON(audioListener);

		auto volume = audioListener.GetVolume();
		auto velocity = audioListener.GetVelocity();
		auto soundSpeed = audioListener.GetSoundSpeed();
		auto dopplerFactor = audioListener.GetDopplerFactor();

		if (ImGui::DragFloat("volume", &volume, 0.001f))
			audioListener.SetVolume(volume);

		if (ImGui::DragFloat3("velocity", &velocity[0]))
			audioListener.SetVelocity(velocity);

		if (ImGui::DragFloat("speed of sound", &soundSpeed))
			audioListener.SetSoundSpeed(soundSpeed);

		if (ImGui::DragFloat("doppler factor", &dopplerFactor))
			audioListener.SetDopplerFactor(dopplerFactor);
	}
}