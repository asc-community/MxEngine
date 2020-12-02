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

#include "Utilities/Audio/SupportedAudioTypes.h"
#include "Utilities/STL/MxString.h"

namespace MxEngine
{
    class AudioBuffer
    {
        using BindableId = unsigned int;

        MxString filepath;
        BindableId id = 0;
        uint8_t channels = 0;
        AudioType type = AudioType::WAV;
        size_t frequency = 0;
        size_t sampleCount = 0;
        size_t nativeFormat = 0;

        void FreeAudioBuffer();
    public:
        AudioBuffer();
        ~AudioBuffer();
        AudioBuffer(const AudioBuffer&) = delete;
        AudioBuffer(AudioBuffer&&) noexcept;
        AudioBuffer& operator=(const AudioBuffer&) = delete;
        AudioBuffer& operator=(AudioBuffer&&) noexcept;

        template<typename FilePath>
        AudioBuffer(const FilePath& path);

        template<typename FilePath>
        void Load(const FilePath& path);

        BindableId GetNativeHandle() const;
        size_t GetChannelCount() const;
        size_t GetFrequency() const;
        size_t GetNativeFormat() const;
        size_t GetSampleCount() const;
        AudioType GetAudioType() const;
        const MxString& GetFilePath() const;
    };
}