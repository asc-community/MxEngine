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
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    float CameraToneMapping::GetGamma() const
    {
        return this->gamma;
    }

    const CameraToneMapping::ColorChannels& CameraToneMapping::GetColorGrading() const
    {
        return colorChannels;
    }

    float CameraToneMapping::GetExposure() const
    {
        return this->exposure;
    }

    float CameraToneMapping::GetColorScale() const
    {
        return this->colorScale;
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

    void CameraToneMapping::SetColorGrading(const ColorChannels& colorChannels)
    {
        Vector3 zeroVector3 = { 0.0f, 0.0f, 0.0f };
        Vector3 oneVector3  = { 1.0f, 1.0f, 1.0f };
        this->colorChannels = {
            VectorMax(zeroVector3, VectorMin(oneVector3, colorChannels.R)), // Please, write Clamp function for that purposes!
            VectorMax(zeroVector3, VectorMin(oneVector3, colorChannels.G)),
            VectorMax(zeroVector3, VectorMin(oneVector3, colorChannels.B)),
        };
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
        this->colorScale = Max(0.0f, mult);
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

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<ACES>("ACES")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<ACES>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("a", &ACES::A)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("b", &ACES::B)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("c", &ACES::C)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("d", &ACES::D)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("e", &ACES::E)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("f", &ACES::F)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );

        rttr::registration::class_<CameraToneMapping::ColorChannels>("ColorChannels")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<ACES>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("R", &CameraToneMapping::ColorChannels::R)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("G", &CameraToneMapping::ColorChannels::G)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("B", &CameraToneMapping::ColorChannels::B)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            );

        rttr::registration::class_<CameraToneMapping>("CameraToneMapping")
            .constructor<>()
            .property("gamma", &CameraToneMapping::GetGamma, &CameraToneMapping::SetGamma)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("exposure", &CameraToneMapping::GetExposure, &CameraToneMapping::SetExposure)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("white point", &CameraToneMapping::GetWhitePoint, &CameraToneMapping::SetWhitePoint)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { -10000000.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("color scale", &CameraToneMapping::GetColorScale, &CameraToneMapping::SetColorScale)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("color scale", &CameraToneMapping::GetColorScale, &CameraToneMapping::SetColorScale)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("aces coefficients", &CameraToneMapping::GetACESCoefficients, &CameraToneMapping::SetACESCoefficients)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("eye adaptation speed", &CameraToneMapping::GetEyeAdaptationSpeed, &CameraToneMapping::SetEyeAdaptationSpeed)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("eye adaptation threshold", &CameraToneMapping::GetEyeAdaptationThreshold, &CameraToneMapping::SetEyeAdaptationThreshold)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("min luminance", &CameraToneMapping::GetMinLuminance, &CameraToneMapping::SetMinLuminance)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max luminance", &CameraToneMapping::GetMaxLuminance, &CameraToneMapping::SetMaxLuminance)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("color grading", &CameraToneMapping::GetColorGrading, &CameraToneMapping::SetColorGrading)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}