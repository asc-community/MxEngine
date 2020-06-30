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
#include "Utilities/Logger/Logger.h"

#define DR_FLAC_IMPLEMENTATION
#include "Vendors/miniaudio/dr_flac.h"     /* Enables FLAC decoding. */
#define DR_MP3_IMPLEMENTATION
#include "Vendors/miniaudio/dr_mp3.h"      /* Enables MP3 decoding. */
#define DR_WAV_IMPLEMENTATION
#include "Vendors/miniaudio/dr_wav.h"      /* Enables WAV decoding. */

#include "Vendors/stb/stb_vorbis.h"  /* Enables OGG decoding. */

namespace MxEngine
{
    AudioData AudioLoader::Load(const MxString& path)
    {
        auto ext = FilePath(path.c_str()).extension();
        // TODO: clear memory after load and store data in vector containers
        AudioData result;

        if (ext == ".wav")
        {
            unsigned int channels;
            unsigned int sampleRate;
            drwav_uint64 totalPCMFrameCount;
            drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(path.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);
            if (pSampleData == nullptr)
                return result;

            result.data = pSampleData;
            result.size = size_t(totalPCMFrameCount * sizeof(uint16_t) * channels);
            result.channels = channels;
            result.frequency = sampleRate;
            result.type = AudioType::WAV;
        }
        else if (ext == ".mp3")
        {
            drmp3_config config;
            drmp3_uint64 totalPCMFrameCount;
            drmp3_int16* pSampleData = drmp3_open_file_and_read_pcm_frames_s16(path.c_str(), &config, &totalPCMFrameCount, nullptr);
            if (pSampleData == nullptr)
                return result;

            result.data = pSampleData;
            result.size = size_t(totalPCMFrameCount * sizeof(uint16_t) * config.channels);
            result.channels = config.channels;
            result.frequency = config.sampleRate;
            result.type = AudioType::MP3;
        }
        else if (ext == ".flac")
        {
            unsigned int channels;
            unsigned int sampleRate;
            drflac_uint64 totalPCMFrameCount;
            drflac_int16* pSampleData = drflac_open_file_and_read_pcm_frames_s16(path.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);
            if (pSampleData == nullptr)
                return result;

            result.data = pSampleData;
            result.size = size_t(totalPCMFrameCount * sizeof(uint16_t) * channels);
            result.channels = channels;
            result.frequency = sampleRate;
            result.type = AudioType::FLAC;
        }
        else if (ext == ".ogg")
        {
            int channels;
            int rate;
            short* pSampleData;
            result.size = stb_vorbis_decode_filename(path.c_str(), &channels, &rate, &pSampleData);
            result.data = pSampleData;
            result.channels = channels;
            result.frequency = rate;
            result.type = AudioType::OGG;
        }
        else
        {
            Logger::Instance().Warning("MxEngine::AudioLoader", "file was not loaded as extension is unknown: " + ToMxString(ext));
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