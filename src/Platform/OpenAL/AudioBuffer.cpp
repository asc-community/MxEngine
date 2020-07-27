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

#include "AudioBuffer.h"
#include "ALUtilities.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/Audio/AudioLoader.h"

namespace MxEngine
{
    void AudioBuffer::FreeAudioBuffer()
    {
        if (id != 0)
        {
            ALCALL(alDeleteBuffers(1, &id));
        }
    }

    AudioBuffer::AudioBuffer()
    {
        if(!ALIsInitialized())
        {
            MXLOG_ERROR("OpenAL::AudioBuffer", "buffer cannot be created as there is no audio device available");
            return;
        }
        ALCALL(alGenBuffers(1, &id));
        MXLOG_DEBUG("OpenAL::AudioBuffer", "created audio buffer with id = " + ToMxString(id));
    }

    AudioBuffer::~AudioBuffer()
    {
        this->FreeAudioBuffer();
    }

    AudioBuffer::AudioBuffer(AudioBuffer&& other) noexcept
    {
        this->id = other.id;
        this->filepath = std::move(other.filepath);
        this->channels = other.channels;
        this->type = other.type;
        this->sampleCount = other.sampleCount;
        this->frequency = other.frequency;
        this->nativeFormat = other.nativeFormat;
        other.id = 0;
    }

    AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) noexcept
    {
        this->FreeAudioBuffer();

        this->id = other.id;
        this->filepath = std::move(other.filepath);
        this->channels = other.channels;
        this->type = other.type;
        this->sampleCount = other.sampleCount;
        this->frequency = other.frequency;
        this->nativeFormat = other.nativeFormat;
        other.id = 0;
        return *this;
    }

    void AudioBuffer::Load(const MxString& path)
    {
        auto audio = AudioLoader::Load(path);
        if (audio.data != nullptr)
        {
            if (audio.channels != 1)
            {
                auto copy = audio;
                audio = AudioLoader::ConvertToMono(audio);
                AudioLoader::Free(copy);
            }

            this->nativeFormat = AL_FORMAT_MONO16;
            this->channels = (uint8_t)audio.channels;
            this->frequency = audio.frequency;
            this->type = audio.type;
            this->sampleCount = audio.sampleCount;
            this->filepath = path;

            ALCALL(alBufferData(id, (ALenum) this->nativeFormat, audio.data, ALsizei(audio.sampleCount * sizeof(int16_t)), (ALsizei) audio.frequency));

            AudioLoader::Free(audio);
        }
        else
        {
            MXLOG_ERROR("MxEngine::AudioLoader", "audio file was not loaded: " + path);
        }
    }

    AudioBuffer::BindableId AudioBuffer::GetNativeHandle() const
    {
        return id;
    }

    size_t AudioBuffer::GetChannelCount() const
    {
        return (size_t)this->channels;
    }

    size_t AudioBuffer::GetFrequency() const
    {
        return this->frequency;
    }

    size_t AudioBuffer::GetNativeFormat() const
    {
        return this->nativeFormat;
    }

    size_t AudioBuffer::GetSampleCount() const
    {
        return this->sampleCount;
    }

    AudioType AudioBuffer::GetAudioType() const
    {
        return this->type;
    }

    const MxString& AudioBuffer::GetFilePath() const
    {
        return this->filepath;
    }
}