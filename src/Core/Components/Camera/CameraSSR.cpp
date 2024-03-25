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

#include "CameraSSR.h"
#include "Utilities/Math/Math.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    float CameraSSR::GetThickness() const
    {
        return this->thickness;
    }

    void CameraSSR::SetThickness(float thickness)
    {
        this->thickness = Max(thickness, 0.0f);
    }

    int CameraSSR::GetMaxLevel() const
    {
        return this->maxLevel;
    }

    void CameraSSR::SetMaxLevel(int level) 
    {
        this->maxLevel = level;
    }
    
    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<CameraSSR>("CameraSSR")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("thickness", &CameraSSR::GetThickness, &CameraSSR::SetThickness)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 1.0f, 100.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max level", &CameraSSR::GetMaxLevel, &CameraSSR::SetMaxLevel)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 1, 4 }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 1)
            );
    }
}