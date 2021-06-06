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
    bool Transform::operator==(const Transform& other) const
    {
        return this->position == other.position && this->rotation == other.rotation && this->scale == other.scale;
    }

    bool Transform::operator!=(const Transform& other) const
    {
        return !(*this == other);
    }

    Transform Transform::operator*(const Transform& other) const
    {
        Transform result;
        result.scale = this->scale * other.scale;
        result.position = this->position + other.position;
        result.rotation = this->rotation + other.rotation;
        return result;
    }

    const Matrix4x4& Transform::GetMatrix() const
    {
        if (this->needTransformUpdate)
        {
            this->GetMatrix(this->transform);
            this->GetNormalMatrix(this->transform, this->normalMatrix);
            this->needTransformUpdate = false;
        }
        return this->transform;
    }

    const Matrix3x3& Transform::GetNormalMatrix() const
    {
        (void)this->GetMatrix();
        return this->normalMatrix;
    }

    void Transform::GetMatrix(Matrix4x4& inPlaceMatrix) const
    {
        Matrix4x4 Translation = MxEngine::Translate(Matrix4x4(1.0f), this->position);
        Matrix4x4 Rotation = MakeRotationMatrix(RadiansVec(this->rotation));
        Matrix4x4 Scale = MxEngine::Scale(Matrix4x4(1.0f), this->scale);
        inPlaceMatrix = Translation * Rotation * Scale;
    }

    void Transform::GetNormalMatrix(const Matrix4x4& model, Matrix3x3& inPlaceMatrix) const
    {
        if (this->scale.x == this->scale.y && this->scale.y == this->scale.z)
            inPlaceMatrix = Matrix3x3(model);
        else
            inPlaceMatrix = Transpose(Inverse(model));
    }

    const Vector3& Transform::GetRotation() const
    {
        return this->rotation;
    }

    const Vector3& Transform::GetScale() const
    {
        return this->scale;
    }

    Quaternion Transform::GetRotationQuaternion() const
    {
        return MakeQuaternion(MakeRotationMatrix(RadiansVec(this->rotation)));
    }

    const Vector3& Transform::GetPosition() const
    {
        return this->position;
    }

    Transform& Transform::SetRotation(const Quaternion& q)
    {
        return this->SetRotation(DegreesVec(MakeEulerAngles(q)));
    }

    Transform& Transform::SetRotation(const Vector3& angles)
    {
        this->rotation = MakeVector3(0.0f);
        this->Rotate(angles);
        return *this;
    }

    Transform& Transform::SetScale(const Vector3& scale)
    {
        this->scale = scale;
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::SetScale(float scale) 
    {
        return this->SetScale(MakeVector3(scale));
    }

    Transform& Transform::SetPosition(const Vector3& position)
    {
        this->position = position;
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::Scale(float scale) 
    {
        return this->Scale(MakeVector3(scale));
    }

    Transform& Transform::Scale(const Vector3& scale)
    {
        this->scale *= scale;
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::ScaleX(float scale) 
    {
        return this->Scale(MakeVector3(scale, 1.0f, 1.0f));
    }

    Transform& Transform::ScaleY(float scale) 
    {
        return this->Scale(MakeVector3(1.0f, scale, 1.0f));
    }

    Transform& Transform::ScaleZ(float scale) 
    {
        return this->Scale(MakeVector3(1.0f, 1.0f, scale));
    }

    Transform& Transform::Rotate(const Quaternion& q)
    {
        return this->Rotate(MakeEulerAngles(q));
    }

    Transform& Transform::Rotate(const Vector3& angles)
    {
        this->rotation += angles;
        this->rotation.x = std::fmod(this->rotation.x + 360.0f, 360.0f);
        this->rotation.y = std::fmod(this->rotation.y + 360.0f, 360.0f);
        this->rotation.z = std::fmod(this->rotation.z + 360.0f, 360.0f);
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::RotateX(float angle)
    {
        return this->Rotate(Vector3(angle, 0.0f, 0.0f));
    }

    Transform& Transform::RotateY(float angle)
    {
        return this->Rotate(Vector3(0.0f, angle, 0.0f));
    }

    Transform& Transform::RotateZ(float angle)
    {
        return this->Rotate(Vector3(0.0f, 0.0f, angle));
    }

    Transform& Transform::Translate(const Vector3& dist)
    {
        this->position += dist;
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::TranslateX(float x)
    {
        return this->Translate(MakeVector3(x, 0.0f, 0.0f));
    }

    Transform& Transform::TranslateY(float y)
    {
        return this->Translate(MakeVector3(0.0f, y, 0.0f));
    }

    Transform& Transform::TranslateZ(float z)
    {
        return this->Translate(MakeVector3(0.0f, 0.0f, z));
    }

    Transform& Transform::LookAt(const Vector3& point)
    {
        auto q = LookAtRotation(Normalize(this->position - point), MakeVector3(0.00001f, 1.0f, 0.0f));
        this->SetRotation(q);
        return *this;
    }

    Transform& Transform::LookAtXY(const Vector3& point)
    {
        auto v = this->position - point;
        auto q = LookAtRotation(Normalize(MakeVector3(v.x, v.y, 0.0f)), MakeVector3(0.00001f, 0.0f, 1.0f));
        this->SetRotation(q);
        return *this;
    }

    Transform& Transform::LookAtXZ(const Vector3& point)
    {
        auto v = this->position - point;
        auto q = LookAtRotation(Normalize(MakeVector3(v.x, 0.0f, v.z)), MakeVector3(0.00001f, 1.0f, 0.0f));
        this->SetRotation(q);
        return *this;
    }

    Transform& Transform::LookAtYZ(const Vector3& point)
    {
        auto v = this->position - point;
        auto q = LookAtRotation(Normalize(MakeVector3(0.0f, v.y, v.z)), MakeVector3(1.0f, 0.0f, 0.00001f));
        this->SetRotation(q);
        return *this;
    }

    Transform LocalToWorld(const Transform& parent, const Transform& child)
    {
        Transform result;
        result.SetScale(parent.GetScale() * child.GetScale());
        result.SetRotation(parent.GetRotation() + child.GetRotation());
        result.SetPosition(parent.GetPosition() + parent.GetScale() * child.GetPosition());
        return result;
    }

    Transform WorldToLocal(const Transform& parent, const Transform& world)
    {
        Transform result;
        result.SetScale(world.GetScale() / parent.GetScale());
        result.SetRotation(world.GetRotation() - parent.GetRotation());
        result.SetPosition((world.GetPosition() - parent.GetPosition()) / parent.GetScale());
        return result;
    }

    MXENGINE_REFLECT_TYPE
    {
        using Scale3 = Transform& (Transform::*)(const Vector3&);
        using RotateEuler = Transform& (Transform::*)(const Vector3&);

        rttr::registration::class_<Transform>("Transform")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<Transform>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("position", &Transform::GetPosition, &Transform::SetPosition)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.5f)
            )
            .property("rotation", &Transform::GetRotation, (RotateEuler)&Transform::SetRotation)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.5f)
            )
            .property("scale", &Transform::GetScale, (Scale3)&Transform::SetScale)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.01f, 100.0f })
            );
    }
}