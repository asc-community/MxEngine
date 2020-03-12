// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

namespace MxEngine
{
    const Matrix4x4& Transform::GetMatrix() const
    {
        if (this->needTransformUpdate)
        {
            Matrix4x4 Translation = MxEngine::Translate(Matrix4x4(1.0f), this->translation);
            Matrix4x4 Rotation = ToMatrix(this->rotation);
            Matrix4x4 Scale = MxEngine::Scale(Matrix4x4(1.0f), this->scale);
            this->transform = Translation * Rotation * Scale;
            this->needTransformUpdate = false;
        }
        return this->transform;
    }

    const Vector3& Transform::GetTranslation() const
    {
        return this->translation;
    }

    const Quaternion& Transform::GetRotation() const
    {
        return this->rotation;
    }

    const Vector3& Transform::GetScale() const
    {
        return this->scale;
    }

    const Vector3& Transform::GetEulerRotation() const
    {
        if (this->needRotationUpdate)
        {
            this->eulerRotation = MakeEulerAngles(this->rotation);
            this->needRotationUpdate = false;
        }
        return this->eulerRotation;
    }

    Transform& Transform::SetTranslation(const Vector3& dist)
    {
        this->translation = dist;
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::SetRotation(float angle, const Vector3& axis)
    {
        return this->SetRotation(MakeQuaternion(angle, axis));
    }

    Transform& Transform::SetRotation(const Quaternion& q)
    {
        this->rotation = q;
        this->needRotationUpdate = true;
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::SetScale(const Vector3& scale)
    {
        this->scale = scale;
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
        return this->Scale(MakeVector3(scale, 0.0f, 0.0f));
    }

    Transform& Transform::ScaleY(float scale)
    {
        return this->Scale(MakeVector3(0.0f, scale, 0.0f));
    }

    Transform& Transform::ScaleZ(float scale)
    {
        return this->Scale(MakeVector3(0.0f, 0.0f, scale));
    }

    Transform& Transform::Rotate(float angle, const Vector3& axis)
    {
        return this->Rotate(MakeQuaternion(angle, axis));
    }

    Transform& Transform::Rotate(const Quaternion& q)
    {
        this->rotation *= q;
        this->needRotationUpdate = true;
        this->needTransformUpdate = true;
        return *this;
    }

    Transform& Transform::RotateX(float angle)
    {
        return this->Rotate(angle, MakeVector3(1.0f, 0.0f, 0.0f));
    }

    Transform& Transform::RotateY(float angle)
    {
        return this->Rotate(angle, MakeVector3(0.0f, 1.0f, 0.0f));
    }

    Transform& Transform::RotateZ(float angle)
    {
        return this->Rotate(angle, MakeVector3(0.0f, 0.0f, 1.0f));
    }

    Transform& Transform::Translate(const Vector3& dist)
    {
        this->translation += dist;
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
}