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

#include "AudioLoader.h"

#include "Utilities/FileSystem/File.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Macro/Macro.h"

#define DR_FLAC_IMPLEMENTATION
#include <dr_flac.h>     /* Enables FLAC decoding. */
#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>      /* Enables MP3 decoding. */
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>      /* Enables WAV decoding. */

#include <stb_vorbis.c>  /* Enables OGG decoding. */

namespace MxEngine
{
    template<>
    AudioData AudioLoader::Load(const std::filesystem::path& path)
    {
        MAKE_SCOPE_PROFILER("AudioLoader::Load");
        MAKE_SCOPE_TIMER("MxEngine::AudioLoader", "AudioLoader::Load");
        MXLOG_INFO("MxEngine::AudioLoader", "loading audio from file: " + ToMxString(path));

        auto ext = path.extension();
        AudioData result;

        if (ext == ".wav")
        {
            unsigned int channels;
            unsigned int sampleRate;
            drwav_uint64 totalPCMFrameCount;
            drwav_int16* sampleData = drwav_open_file_and_read_pcm_frames_s16(path.string().c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);
            if (sampleData == nullptr)
                return result;

            result.data = sampleData;
            result.sampleCount = size_t(totalPCMFrameCount * channels);
            result.channels = channels;
            result.frequency = sampleRate;
            result.type = AudioType::WAV; //-V1048
        }
        else if (ext == ".mp3")
        {
            drmp3_config config;
            drmp3_uint64 totalPCMFrameCount;
            drmp3_int16* sampleData = drmp3_open_file_and_read_pcm_frames_s16(path.string().c_str(), &config, &totalPCMFrameCount, nullptr);
            if (sampleData == nullptr)
                return result;

            result.data = sampleData;
            result.sampleCount = size_t(totalPCMFrameCount * config.channels);
            result.channels = config.channels;
            result.frequency = config.sampleRate;
            result.type = AudioType::MP3;
        }
        else if (ext == ".flac")
        {
            unsigned int channels;
            unsigned int sampleRate;
            drflac_uint64 totalPCMFrameCount;
            drflac_int16* sampleData = drflac_open_file_and_read_pcm_frames_s16(path.string().c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);
            if (sampleData == nullptr)
                return result;

            result.data = sampleData;
            result.sampleCount = size_t(totalPCMFrameCount * channels);
            result.channels = channels;
            result.frequency = sampleRate;
            result.type = AudioType::FLAC;
        }
        else if (ext == ".ogg")
        {
            int channels;
            int sampleRate;
            short* sampleData;
            int totalPCMFrameCount = stb_vorbis_decode_filename(path.string().c_str(), &channels, &sampleRate, &sampleData);
            if (sampleData == nullptr)
                return result;

            result.data = sampleData;
            result.sampleCount = size_t(totalPCMFrameCount * channels);
            result.channels = channels;
            result.frequency = sampleRate;
            result.type = AudioType::OGG;
        }
        else
        {
            MXLOG_WARNING("MxEngine::AudioLoader", "file was not loaded as extension is unknown: " + ToMxString(ext));
        }
        return result;
    }

    AudioData AudioLoader::ConvertToMono(AudioData& audio)
    {
        AudioData result;

        result.sampleCount = audio.sampleCount / audio.channels;
        result.data = (int16_t*)std::malloc(result.sampleCount * sizeof(int16_t));
        result.channels = 1;
        result.frequency = audio.frequency;
        result.type = audio.type;
        MX_ASSERT(audio.data != nullptr);

        for (size_t i = 0, j = 0; i < audio.sampleCount; i += 2, j++)
        {
            auto left = audio.data[i];
            auto right = audio.data[i + 1];
            result.data[j] = (int(left) + int(right)) / 2; //-V522
        }
        return result;
    }

    void AudioLoader::Free(AudioData& audio)
    {
        switch (audio.type)
        {
        case AudioType::WAV:
            drwav_free((void*)audio.data, nullptr);
            break;
        case AudioType::MP3:
            drmp3_free((void*)audio.data, nullptr);
            break;
        case AudioType::FLAC:
            drflac_free((void*)audio.data, nullptr);
            break;
        case AudioType::OGG:
            std::free((void*)audio.data);
            break;
        }
    }
}