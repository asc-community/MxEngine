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

#include "Utilities/Math/Math.h"

namespace MxEngine
{
	class TransformComponent
	{
		Vector3 position = MakeVector3(0.0f);
		Vector3 rotation = MakeVector3(0.0f);
		Vector3 scale = MakeVector3(1.0f);
		mutable Matrix4x4 transform{ 0.0f };
		mutable Matrix3x3 normalMatrix{ 0.0f };
		mutable bool needTransformUpdate = true;
	public:
		bool operator==(const TransformComponent& other) const;
		bool operator!=(const TransformComponent& other) const;
		TransformComponent operator*(const TransformComponent& other) const;

		const Matrix4x4& GetMatrix() const;
		const Matrix3x3& GetNormalMatrix() const;
		void GetMatrix(Matrix4x4& inPlaceMatrix) const;
		void GetNormalMatrix(const Matrix4x4& model, Matrix3x3& inPlaceMatrix) const;

		const Vector3& GetPosition() const;
		const Vector3& GetRotation() const;
		const Vector3& GetScale() const;

		Quaternion GetRotationQuaternion() const;

		TransformComponent& SetRotation(const Quaternion& q);
		TransformComponent& SetRotation(const Vector3& angles);
		TransformComponent& SetScale(const Vector3& scale);
		TransformComponent& SetScale(float scale);
		TransformComponent& SetPosition(const Vector3& position);

		TransformComponent& Scale(float scale);
		TransformComponent& Scale(const Vector3& scale);
		TransformComponent& ScaleX(float scale);
		TransformComponent& ScaleY(float scale);
		TransformComponent& ScaleZ(float scale);

		TransformComponent& Rotate(const Quaternion& q);
		TransformComponent& Rotate(const Vector3& angles);
		TransformComponent& RotateX(float angle);
		TransformComponent& RotateY(float angle);
		TransformComponent& RotateZ(float angle);

		TransformComponent& Translate(const Vector3& dist);
		TransformComponent& TranslateX(float x);
		TransformComponent& TranslateY(float y);
		TransformComponent& TranslateZ(float z);

		TransformComponent& LookAt(const Vector3& point);
		TransformComponent& LookAtXY(const Vector3& point);
		TransformComponent& LookAtXZ(const Vector3& point);
		TransformComponent& LookAtYZ(const Vector3& point);
	};
}