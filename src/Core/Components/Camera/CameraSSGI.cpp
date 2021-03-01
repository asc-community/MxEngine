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

#include "CameraSSGI.h"
#include "Utilities/Math/Math.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    float CameraSSGI::GetIntensity() const
    {
        return this->intensity;
    }

    float CameraSSGI::GetDistance() const
    {
        return this->distance;
    }

    size_t CameraSSGI::GetRaySteps() const
    {
        return this->raySteps;
    }

    size_t CameraSSGI::GetBlurIterations() const
    {
        return this->blurIterations;
    }

    size_t CameraSSGI::GetBlurLOD() const
    {
        return this->blurLOD;
    }

    void CameraSSGI::SetIntensity(float intensity)
    {
        this->intensity = Max(intensity, 0.0f);
    }

    void CameraSSGI::SetDistance(float distance)
    {
        this->distance = Max(distance, 0.0f);
    }

    void CameraSSGI::SetRaySteps(size_t raySteps)
    {
        this->raySteps = Clamp<size_t>(raySteps, 0, 50);
    }

    void CameraSSGI::SetBlurIterations(size_t iterations)
    {
        this->blurIterations = (uint8_t)Min(iterations, (size_t)std::numeric_limits<uint8_t>::max());
    }

    void CameraSSGI::SetBlurLOD(size_t lod)
    {
        this->blurLOD = (uint8_t)Min(lod, (size_t)std::numeric_limits<uint8_t>::max());
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<CameraSSGI>("CameraSSGI")
            .constructor<>()
            .property("intensity", &CameraSSGI::GetIntensity, &CameraSSGI::SetIntensity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("distance", &CameraSSGI::GetDistance, &CameraSSGI::SetDistance)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("ray steps", &CameraSSGI::GetRaySteps, &CameraSSGI::SetRaySteps)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 50.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("blur iterations", &CameraSSGI::GetBlurIterations, &CameraSSGI::SetBlurIterations)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 50.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("blur lod", &CameraSSGI::GetBlurLOD, &CameraSSGI::SetBlurLOD)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            );
    }
}