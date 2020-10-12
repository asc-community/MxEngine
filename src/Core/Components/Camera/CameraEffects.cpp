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

#include "CameraEffects.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
    float CameraEffects::GetBloomWeight() const
    {
        return this->bloomWeight;
    }

    float CameraEffects::GetVignetteIntensity() const
    {
        return this->vignetteIntensity;
    }

    float CameraEffects::GetVignetteRadius() const
    {
        return this->vignetteRadius;
    }

    float CameraEffects::GetChromaticAberrationMinDistance() const
    {
        return this->chromaticAberrationMinDistance;
    }

    float CameraEffects::GetChromaticAberrationIntensity() const
    {
        return this->chromaticAberrationIntensity;
    }

    float CameraEffects::GetChromaticAberrationDistortion() const
    {
        return this->chromaticAberrationDistortion;
    }

    bool CameraEffects::IsFXAAEnabled() const
    {
        return this->enableFXAA;
    }

    size_t CameraEffects::GetBloomIterations() const
    {
        return size_t(this->bloomIterations);
    }

    void CameraEffects::SetBloomWeight(float weight)
    {
        this->bloomWeight = Max(0.0f, weight);
    }

    void CameraEffects::SetVignetteRadius(float radius)
    {
        this->vignetteRadius = Max(0.0f, radius);
    }

    void CameraEffects::SetVignetteIntensity(float intensity)
    {
        this->vignetteIntensity = Max(0.0f, intensity);
    }

    void CameraEffects::SetChromaticAberrationMinDistance(float distance)
    {
        this->chromaticAberrationMinDistance = Max(distance, 0.0f);
    }

    void CameraEffects::SetChromaticAberrationIntensity(float intensity)
    {
        this->chromaticAberrationIntensity = Max(intensity, 0.0f);
    }

    void CameraEffects::SetChromaticAberrationDistortion(float distortion)
    {
        this->chromaticAberrationDistortion = Max(distortion, 0.0f);
    }

    void CameraEffects::ToggleFXAA(bool value)
    {
        this->enableFXAA = value;
    }

    void CameraEffects::SetBloomIterations(size_t iterations)
    {
        this->bloomIterations = (uint8_t)Min(100, iterations);
    }
}