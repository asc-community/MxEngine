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

#include "DirectionalLight.h"
#include "Core/Application/Rendering.h"
#include "Core/Config/GlobalConfig.h"
#include "Core/Application/Timer.h"
#include "Core/Components/Camera/CameraController.h"
#include "Core/Components/Camera/PerspectiveCamera.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    DirectionalLight::DirectionalLight()
    {
        auto depthTextureSize = (int)GlobalConfig::GetDirectionalLightTextureSize();
        this->DepthMap = Factory<Texture>::Create();
        this->DepthMap->LoadDepth(DirectionalLight::TextureCount * depthTextureSize, depthTextureSize);
        this->DepthMap->SetInternalEngineTag(MXENGINE_MAKE_INTERNAL_TAG("directional light"));
    }

    Matrix4x4 DirectionalLight::ComputeCascadeMatrix(size_t cascadeIndex, float aspectRatio, float fov, const Matrix4x4& viewMatrix) const
    {
        float nearPlane = this->Projections[cascadeIndex];
        float farPlane = cascadeIndex + 1 < DirectionalLight::TextureCount ? this->Projections[cascadeIndex + 1] : 100000.0f;

        auto projection = MakeReversedPerspectiveMatrix(Radians(fov), aspectRatio, nearPlane, farPlane);
        auto invViewProj = Inverse(projection * viewMatrix);

        std::array corners = {
            MakeVector4(-1.0f, -1.0f, -1.0f, 1.0f),
            MakeVector4(-1.0f, -1.0f,  1.0f, 1.0f),
            MakeVector4(-1.0f,  1.0f, -1.0f, 1.0f),
            MakeVector4(-1.0f,  1.0f,  1.0f, 1.0f),
            MakeVector4( 1.0f, -1.0f, -1.0f, 1.0f),
            MakeVector4( 1.0f, -1.0f,  1.0f, 1.0f),
            MakeVector4( 1.0f,  1.0f, -1.0f, 1.0f),
            MakeVector4( 1.0f,  1.0f,  1.0f, 1.0f),
        };
        for (auto& corner : corners)
        {
            corner = invViewProj * corner;
            corner /= corner.w;
        }

        auto center = MakeVector3(0.0f);
        for (const auto& corner : corners)
        {
            center += Vector3(corner);
        }
        center /= corners.size();

        const auto lightView = MakeViewMatrix(center + Normalize(this->Direction), center, MakeVector3(0.001f, 1.0f, 0.001f));

        auto minVector = MakeVector3(std::numeric_limits<float>::max());
        auto maxVector = MakeVector3(std::numeric_limits<float>::lowest());
        for (const auto& corner : corners)
        {
            Vector3 transformedCorner = lightView * corner;
            minVector = VectorMin(minVector, transformedCorner);
            maxVector = VectorMax(maxVector, transformedCorner);
        }
        minVector.z *= minVector.z < 0.0f ? this->DepthScale : 1.0f / this->DepthScale;
        maxVector.z *= maxVector.z > 0.0f ? this->DepthScale : 1.0f / this->DepthScale;

        auto shadowMapSize = float(this->DepthMap->GetHeight() + 1);
        auto worldUnitsPerTexel = (maxVector - minVector) / shadowMapSize;
        minVector = floor(minVector / worldUnitsPerTexel) * worldUnitsPerTexel;
        maxVector = floor(maxVector / worldUnitsPerTexel) * worldUnitsPerTexel;

        auto lightProjection = MakeOrthographicMatrix(minVector.x, maxVector.x, minVector.y, maxVector.y, -maxVector.z, -minVector.z);
        return lightProjection * lightView;
    }

    void DirectionalLight::OnUpdate(float dt)
    {
        auto viewport = Rendering::GetViewport();
        if (this->IsFollowingViewport && viewport.IsValid())
        {
            auto& controller = *viewport.GetUnchecked();
            auto& object = MxObject::GetByComponent(*this);
            object.LocalTransform.SetPosition(MxObject::GetByComponent(controller).LocalTransform.GetPosition());

            float aspectRatio = controller.Camera.GetAspectRatio();
            float fov = controller.GetCameraType() == CameraType::PERSPECTIVE ? controller.GetCamera<PerspectiveCamera>().GetFOV() : controller.Camera.GetZoom();
            auto viewMatrix = controller.Camera.GetViewMatrix();

            for (size_t i = 0; i < this->matrices.size(); i++)
            {
                this->matrices[i] = this->ComputeCascadeMatrix(i, aspectRatio, fov, viewMatrix);
            }
        }
        else
        {
            float aspectRatio = 1.0f;
            float fov = PerspectiveCamera::DefaultFOV;
            auto viewMatrix = MakeViewMatrix(MakeVector3(0.0f, 1.0f, 0.0f), MakeVector3(0.0f), MakeVector3(0.001f, 1.0f, 0.001f));

            for (size_t i = 0; i < this->matrices.size(); i++)
            {
                this->matrices[i] = this->ComputeCascadeMatrix(i, aspectRatio, fov, viewMatrix);
            }
        }
    }

    const Matrix4x4& DirectionalLight::GetMatrix(size_t index) const
    {
        MX_ASSERT(index < this->matrices.size());
        return this->matrices[index];
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<DirectionalLight>("DirectionalLight")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("color", &DirectionalLight::GetColor, &DirectionalLight::SetColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("intensity", &DirectionalLight::GetIntensity, &DirectionalLight::SetIntensity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 10000000.0f })
            )
            .property("ambient intensity", &DirectionalLight::GetAmbientIntensity, &DirectionalLight::SetAmbientIntensity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1.0f })
            )
            .property("is following viewport", &DirectionalLight::IsFollowingViewport)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("direction", &DirectionalLight::Direction)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property_readonly("depth map", &DirectionalLight::DepthMap)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("depth scale", &DirectionalLight::DepthScale)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000000.0f })
            )
            .property("projections", &DirectionalLight::Projections)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}