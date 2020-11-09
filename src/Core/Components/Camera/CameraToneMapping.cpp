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

#include "CameraToneMapping.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
    float CameraToneMapping::GetGamma() const
    {
        return this->gamma;
    }

    float CameraToneMapping::GetExposure() const
    {
        return this->exposure;
    }

    float CameraToneMapping::GetColorScale() const
    {
        return this->colorMultiplier;
    }

    float CameraToneMapping::GetWhitePoint() const
    {
        return this->whitePoint;
    }

    const ACES& CameraToneMapping::GetACESCoefficients() const
    {
        return this->coefficients;
    }

    float CameraToneMapping::GetEyeAdaptationSpeed() const
    {
        return this->eyeAdaptationSpeed;
    }

    float CameraToneMapping::GetEyeAdaptationThreshold() const
    {
        return this->eyeAdaptationThreshold;
    }

    float CameraToneMapping::GetMinLuminance() const
    {
        return this->minLuminance;
    }

    float CameraToneMapping::GetMaxLuminance() const
    {
        return this->maxLuminance;
    }

    void CameraToneMapping::SetMinLuminance(float lum)
    {
        this->minLuminance = Clamp(lum, 0.0f, this->maxLuminance);
    }

    void CameraToneMapping::SetMaxLuminance(float lum)
    {
        this->maxLuminance = Max(lum, 0.0f);
        this->SetMinLuminance(this->GetMinLuminance());
    }

    void CameraToneMapping::SetGamma(float gamma)
    {
        this->gamma = Max(0.0f, gamma);
    }

    void CameraToneMapping::SetExposure(float exposure)
    {
        this->exposure = Max(0.0f, exposure);
    }

    void CameraToneMapping::SetColorScale(float mult)
    {
        this->colorMultiplier = Max(0.0f, mult);
    }

    void CameraToneMapping::SetWhitePoint(float point)
    {
        this->whitePoint = point;
    }

    void CameraToneMapping::SetEyeAdaptationThreshold(float threshold)
    {
        this->eyeAdaptationThreshold = Max(threshold, 0.0f);
    }

    void CameraToneMapping::SetEyeAdaptationSpeed(float speed)
    {
        this->eyeAdaptationSpeed = Max(speed, 0.0f);
    }

    void CameraToneMapping::SetACESCoefficients(const ACES& aces)
    {
        this->coefficients = {
            Max(0.0f, aces.A), Max(0.0f, aces.B),
            Max(0.0f, aces.C), Max(0.0f, aces.D),
            Max(0.0f, aces.E), Max(0.0f, aces.F),
        };
    }
}