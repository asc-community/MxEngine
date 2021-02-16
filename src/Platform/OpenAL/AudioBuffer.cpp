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
#include "Utilities/FileSystem/File.h"
#include "Core/Runtime/Reflection.h"

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

    template<>
    void AudioBuffer::Load(const std::filesystem::path& path)
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
            this->filepath = ToMxString(path);

            ALCALL(alBufferData(id, (ALenum) this->nativeFormat, audio.data, ALsizei(audio.sampleCount * sizeof(int16_t)), (ALsizei) audio.frequency));

            AudioLoader::Free(audio);
        }
        else
        {
            MXLOG_ERROR("MxEngine::AudioLoader", "audio file was not loaded: " + ToMxString(path));
        }
    }

    void AudioBuffer::Load(const MxString& path)
    {
        this->Load(ToFilePath(path));
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

    static size_t Max(size_t v1, size_t v2)
    {
        return v1 > v2 ? v1 : v2;
    }

    float AudioBuffer::GetLength() const
    {
        return float(this->GetSampleCount()) / float(Max(this->GetFrequency(), 1));
    }

    size_t AudioBuffer::GetLengthInSeconds() const
    {
        return this->GetSampleCount() / Max(this->GetFrequency(), 1);
    }

    AudioType AudioBuffer::GetAudioType() const
    {
        return this->type;
    }

    const MxString& AudioBuffer::GetFilePath() const
    {
        return this->filepath;
    }

    void AudioBuffer::SetInternalEngineTag(const MxString& tag)
    {
        this->filepath = tag;
    }

    bool AudioBuffer::IsInternalEngineResource() const
    {
        return this->filepath.find(MXENGINE_INTERNAL_TAG_SYMBOL) == 0;
    }

    namespace GUI
    {
        rttr::variant AudioBufferHandleEditorExtra(rttr::instance&);
    }

    const char* EnumToString(AudioType type)
    {
        auto t = rttr::type::get(type).get_enumeration();
        return t.value_to_name(type).cbegin();
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::enumeration<AudioType>("AudioType")
        (
            rttr::value("WAV" , AudioType::WAV ),
            rttr::value("MP3" , AudioType::MP3 ),
            rttr::value("OGG" , AudioType::OGG ),
            rttr::value("FLAC", AudioType::FLAC)
        );

        using SetFilePath = void(AudioBuffer::*)(const MxString&);

        rttr::registration::class_<AudioBuffer>("AudioBuffer")
            (
                rttr::metadata(EditorInfo::HANDLE_EDITOR, GUI::HandleEditorExtra<AudioBuffer>)
            )
            .constructor<>()
            .property_readonly("filepath", &AudioBuffer::GetFilePath)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("_filepath", &AudioBuffer::GetFilePath, (SetFilePath)&AudioBuffer::Load)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE)
            )
            .property_readonly("length in seconds", &AudioBuffer::GetLengthInSeconds)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("audio type", &AudioBuffer::GetAudioType)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("frequency", &AudioBuffer::GetFrequency)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("channel count", &AudioBuffer::GetChannelCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("sample count", &AudioBuffer::GetSampleCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("native handle", &AudioBuffer::GetNativeHandle)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("native format", &AudioBuffer::GetNativeFormat)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            );
    }
}