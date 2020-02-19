// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
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

#pragma once

#include "Core/Interfaces/ICamera.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Interfaces/IMovable.h"

namespace MxEngine
{
	class CameraController : public IMovable
	{
		UniqueRef<ICamera> camera;
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		Vector3 direction = { 1.0f, 0.0f, 0.0f };
		Vector3 up = { 0.0f, 1.0f, 0.0f };
        Vector3 forward{ 1.0f, 0.0f, 0.0f };
        Vector3 right{ 0.0f, 0.0f, 1.0f };
		float verticalAngle = 0.0f;
		float horizontalAngle = 0.0f;
		float zoom = 1.0f;
		float moveSpeed = 1.0f;
		float rotateSpeed = 1.0f;
		mutable bool updateCamera = true;
	public:
		bool HasCamera() const;
		void SetCamera(UniqueRef<ICamera> camera);
		ICamera& GetCamera();
		const Matrix4x4& GetCameraMatrix() const;

		float GetMoveSpeed() const;
		float GetRotateSpeed() const;
        const Vector3& GetPosition() const;
        const Vector3& GetDirection() const;
        const Vector3& GetUp() const;
		void SetPosition(const Vector3& position);
		void SetDirection(const Vector3& direction);
		void SetUp(const Vector3& up);
		void SetMoveSpeed(float speed);
		void SetRotateSpeed(float speed);
		void SetZoom(float zoom);
		float GetHorizontalAngle() const;
		float GetVerticalAngle() const;
		float GetZoom() const;

		CameraController& Translate(const Vector3& vec);
		CameraController& TranslateX(float x);
		CameraController& TranslateY(float y);
		CameraController& TranslateZ(float z);

        // Inherited via IMovable
        virtual CameraController& Translate(float x, float y, float z) override;
        virtual CameraController& TranslateForward(float dist) override;
        virtual CameraController& TranslateRight(float dist) override;
        virtual CameraController& TranslateUp(float dist) override;
        virtual CameraController& Rotate(float horz, float vert) override;
        virtual void SetForwardVector(const Vector3& forward) override;
        virtual void SetUpVector(const Vector3& up) override;
        virtual void SetRightVector(const Vector3& right) override;
        virtual const Vector3& GetForwardVector() const override;
        virtual const Vector3& GetUpVector() const override;
        virtual const Vector3& GetRightVector() const override;
    };
}