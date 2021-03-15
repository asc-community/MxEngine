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

#include "Transform.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    bool TransformComponent::operator==(const TransformComponent& other) const
    {
        return this->position == other.position && this->rotation == other.rotation && this->scale == other.scale;
    }

    bool TransformComponent::operator!=(const TransformComponent& other) const
    {
        return !(*this == other);
    }

    TransformComponent TransformComponent::operator*(const TransformComponent& other) const
    {
        TransformComponent result;
        result.scale = this->scale * other.scale;
        result.position = this->position + other.position;
        result.rotation = this->rotation + other.rotation;
        return result;
    }

    const Matrix4x4& TransformComponent::GetMatrix() const
    {
        if (this->needTransformUpdate)
        {
            this->GetMatrix(this->transform);
            this->GetNormalMatrix(this->transform, this->normalMatrix);
            this->needTransformUpdate = false;
        }
        return this->transform;
    }

    const Matrix3x3& TransformComponent::GetNormalMatrix() const
    {
        (void)this->GetMatrix();
        return this->normalMatrix;
    }

    void TransformComponent::GetMatrix(Matrix4x4& inPlaceMatrix) const
    {
        Matrix4x4 Translation = MxEngine::Translate(Matrix4x4(1.0f), this->position);
        Matrix4x4 Rotation = MakeRotationMatrix(RadiansVec(this->rotation));
        Matrix4x4 Scale = MxEngine::Scale(Matrix4x4(1.0f), this->scale);
        inPlaceMatrix = Translation * Rotation * Scale;
    }

    void TransformComponent::GetNormalMatrix(const Matrix4x4& model, Matrix3x3& inPlaceMatrix) const
    {
        if (this->scale.x == this->scale.y && this->scale.y == this->scale.z)
            inPlaceMatrix = Matrix3x3(model);
        else
            inPlaceMatrix = Transpose(Inverse(model));
    }

    const Vector3& TransformComponent::GetRotation() const
    {
        return this->rotation;
    }

    const Vector3& TransformComponent::GetScale() const
    {
        return this->scale;
    }

    Quaternion TransformComponent::GetRotationQuaternion() const
    {
        return MakeQuaternion(MakeRotationMatrix(RadiansVec(this->rotation)));
    }

    const Vector3& TransformComponent::GetPosition() const
    {
        return this->position;
    }

    TransformComponent& TransformComponent::SetRotation(const Quaternion& q)
    {
        return this->SetRotation(DegreesVec(MakeEulerAngles(q)));
    }

    TransformComponent& TransformComponent::SetRotation(const Vector3& angles)
    {
        this->rotation = MakeVector3(0.0f);
        this->Rotate(angles);
        return *this;
    }

    TransformComponent& TransformComponent::SetScale(const Vector3& scale)
    {
        this->scale = scale;
        this->needTransformUpdate = true;
        return *this;
    }

    TransformComponent& TransformComponent::SetScale(float scale) 
    {
        return this->SetScale(MakeVector3(scale));
    }

    TransformComponent& TransformComponent::SetPosition(const Vector3& position)
    {
        this->position = position;
        this->needTransformUpdate = true;
        return *this;
    }

    TransformComponent& TransformComponent::Scale(float scale) 
    {
        return this->Scale(MakeVector3(scale));
    }

    TransformComponent& TransformComponent::Scale(const Vector3& scale)
    {
        this->scale *= scale;
        this->needTransformUpdate = true;
        return *this;
    }

    TransformComponent& TransformComponent::ScaleX(float scale) 
    {
        return this->Scale(MakeVector3(scale, 1.0f, 1.0f));
    }

    TransformComponent& TransformComponent::ScaleY(float scale) 
    {
        return this->Scale(MakeVector3(1.0f, scale, 1.0f));
    }

    TransformComponent& TransformComponent::ScaleZ(float scale) 
    {
        return this->Scale(MakeVector3(1.0f, 1.0f, scale));
    }

    TransformComponent& TransformComponent::Rotate(const Quaternion& q)
    {
        return this->Rotate(MakeEulerAngles(q));
    }

    TransformComponent& TransformComponent::Rotate(const Vector3& angles)
    {
        this->rotation += angles;
        this->rotation.x = std::fmod(this->rotation.x, 360.0f);
        this->rotation.y = std::fmod(this->rotation.y, 360.0f);
        this->rotation.z = std::fmod(this->rotation.z, 360.0f);
        this->needTransformUpdate = true;
        return *this;
    }

    TransformComponent& TransformComponent::RotateX(float angle)
    {
        return this->Rotate(Vector3(angle, 0.0f, 0.0f));
    }

    TransformComponent& TransformComponent::RotateY(float angle)
    {
        return this->Rotate(Vector3(0.0f, angle, 0.0f));
    }

    TransformComponent& TransformComponent::RotateZ(float angle)
    {
        return this->Rotate(Vector3(0.0f, 0.0f, angle));
    }

    TransformComponent& TransformComponent::Translate(const Vector3& dist)
    {
        this->position += dist;
        this->needTransformUpdate = true;
        return *this;
    }

    TransformComponent& TransformComponent::TranslateX(float x)
    {
        return this->Translate(MakeVector3(x, 0.0f, 0.0f));
    }

    TransformComponent& TransformComponent::TranslateY(float y)
    {
        return this->Translate(MakeVector3(0.0f, y, 0.0f));
    }

    TransformComponent& TransformComponent::TranslateZ(float z)
    {
        return this->Translate(MakeVector3(0.0f, 0.0f, z));
    }

    TransformComponent& TransformComponent::LookAt(const Vector3& point)
    {
        auto q = LookAtRotation(Normalize(this->position - point), MakeVector3(0.00001f, 1.0f, 0.0f));
        this->SetRotation(q);
        return *this;
    }

    TransformComponent& TransformComponent::LookAtXY(const Vector3& point)
    {
        auto v = this->position - point;
        auto q = LookAtRotation(Normalize(MakeVector3(v.x, v.y, 0.0f)), MakeVector3(0.00001f, 0.0f, 1.0f));
        this->SetRotation(q);
        return *this;
    }

    TransformComponent& TransformComponent::LookAtXZ(const Vector3& point)
    {
        auto v = this->position - point;
        auto q = LookAtRotation(Normalize(MakeVector3(v.x, 0.0f, v.z)), MakeVector3(0.00001f, 1.0f, 0.0f));
        this->SetRotation(q);
        return *this;
    }

    TransformComponent& TransformComponent::LookAtYZ(const Vector3& point)
    {
        auto v = this->position - point;
        auto q = LookAtRotation(Normalize(MakeVector3(0.0f, v.y, v.z)), MakeVector3(1.0f, 0.0f, 0.00001f));
        this->SetRotation(q);
        return *this;
    }

    MXENGINE_REFLECT_TYPE
    {
        using Scale3 = TransformComponent& (TransformComponent::*)(const Vector3&);
        using RotateEuler = TransformComponent& (TransformComponent::*)(const Vector3&);

        rttr::registration::class_<TransformComponent>("Transform")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<TransformComponent>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("position", &TransformComponent::GetPosition, &TransformComponent::SetPosition)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.5f)
            )
            .property("rotation", &TransformComponent::GetRotation, (RotateEuler)&TransformComponent::SetRotation)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.5f)
            )
            .property("scale", &TransformComponent::GetScale, (Scale3)&TransformComponent::SetScale)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.01f, 100.0f })
            );
    }
}