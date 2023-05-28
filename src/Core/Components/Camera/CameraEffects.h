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

#include "Utilities/ECS/Component.h"

namespace MxEngine
{
    // TODO: split into multiple components
    class CameraEffects
    {
        MAKE_COMPONENT(CameraEffects);

        Vector3 fogColor = MakeVector3(0.5f, 0.6f, 0.7f);
        float fogDistance = 1.0f;
        float fogDensity = 0.001f;

        float bloomWeight = 0.5f;

        float vignetteRadius = 0.1f;
        float vignetteIntensity = 100.0f;

        float chromaticAberrationIntensity = 0.08f;
        float chromaticAberrationMinDistance = 0.8f;
        float chromaticAberrationDistortion = 0.8f;

        bool enableFXAA = false;
        uint8_t bloomIterations = 3;

        float bokehRadius = 10.f;
        float focusRange = 5.f;
        float focusDistance = 0.f;

        float lensFlareScale = 1.f;
        float lensFlareBias = -50.f;
        int lensFlareNumOfGhosts = 3;
        float lensFlareGhostDispersal = 0.3f;
        float lensFalreHaloWidth = 0.5f;
        bool lensFlareEnable = true;
    public:
        CameraEffects() = default;

        float GetBokehRadius()const;
        float GetFocusRange()const;
        float GetFocusDistance()const;
        void SetBokehRadius(float radius);
        void SetFocusRange(float range);
        void SetFocusDistance(float distance);

        const Vector3& GetFogColor() const;
        float GetFogDistance() const;
        float GetFogDensity() const;

        float GetBloomWeight() const;
        size_t GetBloomIterations() const;

        float GetVignetteIntensity() const;
        float GetVignetteRadius() const;

        float GetChromaticAberrationMinDistance() const;
        float GetChromaticAberrationIntensity() const;
        float GetChromaticAberrationDistortion() const;

        bool IsFXAAEnabled() const;

        void SetFogColor(const Vector3& color);
        void SetFogDistance(float distance);
        void SetFogDensity(float density);

        void SetBloomWeight(float weight);
        void SetBloomIterations(size_t iterations);

        void SetVignetteRadius(float radius);
        void SetVignetteIntensity(float intensity);

        void SetChromaticAberrationMinDistance(float distance);
        void SetChromaticAberrationIntensity(float intensity);
        void SetChromaticAberrationDistortion(float distortion);

        void ToggleFXAA(bool value);


        float GetLensFlareScale()const;
        float GetLensFlareBias()const;
        int GetLensFlareNumOfGhosts()const;
        float GetLensFlareGhostDispersal()const;
        float GetLensFalreHaloWidth()const;
        bool GetLensFlareEnable()const;
            
        void SetLensFlareScale(float);
        void SetLensFlareBias(float);
        void SetLensFlareNumOfGhosts(int);
        void SetLensFlareGhostDispersal(float);
        void SetLensFalreHaloWidth(float);
        void SetLensFlareEnable(bool);

    };
}