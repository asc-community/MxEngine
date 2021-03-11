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

#include "PointLight.h"
#include "Core/Config/GlobalConfig.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void PointLight::LoadDepthCubeMap()
    {
        auto depthTextureSize = (int)GlobalConfig::GetPointLightTextureSize();
        this->DepthMap = GraphicFactory::Create<CubeMap>();
        this->DepthMap->LoadDepth(depthTextureSize, depthTextureSize);
        this->DepthMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("point light"));
    }

    bool PointLight::IsCastingShadows() const
    {
        return this->DepthMap.IsValid();
    }

    void PointLight::ToggleShadowCast(bool value)
    {
        if (value && !this->IsCastingShadows())
        {
            this->LoadDepthCubeMap();
        }
        else if(!value && this->IsCastingShadows())
        {
            this->DepthMap = { };
        }
    }

    float PointLight::GetRadius() const
    {
        return this->radius;
    }

    void PointLight::SetRadius(float radius)
    {
        this->radius = Max(0.0f, radius);
    }

    Vector3 DirectionTable[] =
    {
         Normalize(MakeVector3(   1.0f, 0.0001f, 0.0001f)),
         Normalize(MakeVector3(  -1.0f, 0.0001f, 0.0001f)),
         Normalize(MakeVector3(0.0001f,    1.0f, 0.0001f)),
         Normalize(MakeVector3(0.0001f,   -1.0f, 0.0001f)),
         Normalize(MakeVector3(0.0001f, 0.0001f,    1.0f)),
         Normalize(MakeVector3(0.0001f, 0.0001f,   -1.0f)),
    };

    Vector3 UpTable[] =
    {
         MakeVector3(0.0f, -1.0f,  0.0f),
         MakeVector3(0.0f, -1.0f,  0.0f),
         MakeVector3(0.0f,  0.0f,  1.0f),
         MakeVector3(0.0f,  0.0f, -1.0f),
         MakeVector3(0.0f, -1.0f,  0.0f),
         MakeVector3(0.0f, -1.0f,  0.0f),
    };

    Matrix4x4 PointLight::GetMatrix(size_t index, const Vector3& position) const
    {
        auto Projection = MakePerspectiveMatrix(Radians(90.0f), 1.0f, 0.1f, this->radius);
        auto directionNorm = DirectionTable[index];
        auto View = MakeViewMatrix(
            position,
            position + directionNorm,
            UpTable[index]
        );
        return Projection * View;
    }

    Matrix4x4 PointLight::GetSphereTransform(const Vector3& position) const
    {
        Matrix4x4 m{ 0.0f };
        const float scale = 2.0f * RootTwo<float>() * this->radius;
        m[0][0] = m[1][1] = m[2][2] = scale;
        m[3] = Vector4(position, 1.0f);
        return m;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<PointLight>("PointLight")
            .constructor<>()
            .property("color", &PointLight::GetColor, &PointLight::SetColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("intensity", &PointLight::GetIntensity, &PointLight::SetIntensity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f })
            )
            .property("ambient intensity", &PointLight::GetAmbientIntensity, &PointLight::SetAmbientIntensity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1.0f })
            )
            .property("radius", &PointLight::GetRadius, &PointLight::SetRadius)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 10000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
            )
            .property("casts shadows", &PointLight::IsCastingShadows, &PointLight::ToggleShadowCast)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property_readonly("depth map", &PointLight::DepthMap)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(MetaInfo::CONDITION, +([](rttr::instance& obj) { return obj.try_convert<PointLight>()->IsCastingShadows(); }))
            );
    }
}