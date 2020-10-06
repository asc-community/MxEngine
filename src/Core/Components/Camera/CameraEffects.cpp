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
    float CameraEffects::GetGamma() const
    {
        return this->gamma;
    }

    float CameraEffects::GetBloomWeight() const
    {
        return this->bloomWeight;
    }

    float CameraEffects::GetExposure() const
    {
        return this->exposure;
    }

    float CameraEffects::GetColorScale() const
    {
        return this->colorMultiplier;
    }

    float CameraEffects::GetWhitePoint() const
    {
        return this->whitePoint;
    }

    const ACES& CameraEffects::GetACESCoefficients() const
    {
        return this->coefficients;
    }

    float CameraEffects::GetVignetteIntensity() const
    {
        return this->vignetteIntensity;
    }

    float CameraEffects::GetVignetteRadius() const
    {
        return this->vignetteRadius;
    }

    bool CameraEffects::IsFXAAEnabled() const
    {
        return this->enableFXAA;
    }

    bool CameraEffects::IsToneMappingEnabled() const
    {
        return this->toneMapping;
    }

    float CameraEffects::GetEyeAdaptation() const
    {
        return this->eyeAdaptation;
    }

    float CameraEffects::GetMinLuminance() const
    {
        return this->minLuminance;
    }

    float CameraEffects::GetMaxLuminance() const
    {
        return this->maxLuminance;
    }

    void CameraEffects::SetEyeAdaptation(float adaptation)
    {
        this->eyeAdaptation = Max(adaptation, 0.0f);
    }

    void CameraEffects::SetMinLuminance(float lum)
    {
        this->minLuminance = Clamp(lum, 0.0f, this->maxLuminance);
    }

    void CameraEffects::SetMaxLuminance(float lum)
    {
        this->maxLuminance = Max(lum, 0.0f);
        this->SetMinLuminance(this->GetMinLuminance());
    }

    size_t CameraEffects::GetBloomIterations() const
    {
        return size_t(this->bloomIterations);
    }

    float CameraEffects::GetSSRThickness() const
    {
        return this->ssrThickness;
    }

    float CameraEffects::GetSSRMaxCosAngle() const
    {
        return this->ssrMaxCosAngle;
    }

    float CameraEffects::GetSSRSteps() const
    {
        return this->ssrSteps;
    }

    float CameraEffects::GetSSRMaxDistance() const
    {
        return this->ssrMaxDistance;
    }

    void CameraEffects::SetGamma(float gamma)
    {
        this->gamma = Max(0.0f, gamma);
    }

    void CameraEffects::SetBloomWeight(float weight)
    {
        this->bloomWeight = Max(0.0f, weight);
    }

    void CameraEffects::SetExposure(float exposure)
    {
        this->exposure = Max(0.0f, exposure);
    }

    void CameraEffects::SetColorScale(float mult)
    {
        this->colorMultiplier = Max(0.0f, mult);
    }

    void CameraEffects::SetWhitePoint(float point)
    {
        this->whitePoint = point;
    }

    void CameraEffects::SetACESCoefficients(const ACES& aces)
    {
        this->coefficients = {
            Max(0.0f, aces.A), Max(0.0f, aces.B),
            Max(0.0f, aces.C), Max(0.0f, aces.D),
            Max(0.0f, aces.E), Max(0.0f, aces.F),
        };
    }

    void CameraEffects::SetVignetteRadius(float radius)
    {
        this->vignetteRadius = Max(0.0f, radius);
    }

    void CameraEffects::SetVignetteIntensity(float intensity)
    {
        this->vignetteIntensity = Max(0.0f, intensity);
    }

    void CameraEffects::ToggleFXAA(bool value)
    {
        this->enableFXAA = value;
    }

    void CameraEffects::ToggleToneMapping(bool value)
    {
        this->toneMapping = value;
    }

    void CameraEffects::SetBloomIterations(size_t iterations)
    {
        this->bloomIterations = (uint8_t)Min(100, iterations);
    }

    void CameraEffects::SetSSRThickness(float thickness)
    {
        this->ssrThickness = Max(thickness, 0.0f);
    }

    void CameraEffects::SetSSRMaxCosAngle(float angle)
    {
        this->ssrMaxCosAngle = Clamp(angle, -1.0f, 1.0f);
    }

    void CameraEffects::SetSSRSteps(size_t steps)
    {
        this->ssrSteps = steps;
    }

    void CameraEffects::SetSSRMaxDistance(float distance)
    {
        this->ssrMaxDistance = Max(distance, 0.0f);
    }
}