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

#pragma once

#include "Utilities/ECS/Component.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
	class Transform
	{
		MAKE_COMPONENT(Transform);

		Vector3 translation = MakeVector3(0.0f);
		Vector3 scale = MakeVector3(1.0f);
		Quaternion rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		mutable Vector3 eulerRotation{ 0.0f };
		mutable Matrix4x4 transform{ 0.0f };
		mutable bool needTransformUpdate = true;
		mutable bool needRotationUpdate = true;
		mutable Matrix3x3 normalMatrix{ 0.0f };
	public:
		const Matrix4x4& GetMatrix() const;
		const Matrix3x3& GetNormalMatrix() const;
		void GetMatrix(Matrix4x4& inPlaceMatrix) const;
		void GetNormalMatrix(const Matrix4x4& model, Matrix3x3& inPlaceMatrix) const;

		const Vector3& GetTranslation() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;
		const Vector3& GetEulerRotation() const;
		const Vector3& GetPosition() const;

		Transform& SetTranslation(const Vector3& dist);
		Transform& SetRotation(float angle, const Vector3& axis);
		Transform& SetRotation(const Quaternion& q);
		Transform& SetScale(const Vector3& scale);
		Transform& SetScale(float scale);
		Transform& SetPosition(const Vector3& position);

		Transform& Scale(float scale);
		Transform& Scale(const Vector3& scale);
		Transform& ScaleX(float scale);
		Transform& ScaleY(float scale);
		Transform& ScaleZ(float scale);

		Transform& Rotate(float angle, const Vector3& axis);
		Transform& Rotate(const Quaternion& q);
		Transform& RotateX(float angle);
		Transform& RotateY(float angle);
		Transform& RotateZ(float angle);

		Transform& Translate(const Vector3& dist);
		Transform& TranslateX(float x);
		Transform& TranslateY(float y);
		Transform& TranslateZ(float z);

		Transform& TranslateForward(float dist);
		Transform& TranslateRight(float dist);
		Transform& TranslateUp(float dist);
	};
}