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
#include "Platform/GraphicAPI.h"
#include "CameraBase.h"

namespace MxEngine
{
	enum class CameraType : uint8_t
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
	};

	class CameraController
	{
		MAKE_COMPONENT(CameraController);

		GResource<FrameBuffer> framebuffer;
		GResource<RenderBuffer> renderbuffer;
		GResource<Texture> renderTexture;

		Vector3 direction = { 1.0f, 0.0f, 0.0f };
		Vector3 up = { 0.0f, 1.0f, 0.0f };
		Vector3 forward ={ 0.0f, 0.0f, 1.0f };
		Vector3 right = { 1.0f, 0.0f, 0.0f };
		float verticalAngle = 0.0f;
		float horizontalAngle = 0.0f;
		float zoom = 1.0f;
		float exposure = 1.0f;
		float bloomWeight = 100.0f;
		float moveSpeed = 1.0f;
		float rotateSpeed = 1.0f;

		void SubmitMatrixProjectionChanges() const;

		uint8_t bloomIterations = 6;
		CameraType cameraType = CameraType::PERSPECTIVE;
	public:
		bool RenderingEnabled = true;
		mutable CameraBase Camera;

		CameraController();
		~CameraController();
		CameraController(CameraController&&) noexcept = default;
		CameraController& operator=(CameraController&&) noexcept = default;

		template<typename T>
		T& GetCamera();
		template<typename T>
		const T& GetCamera() const;
		void SetCameraType(CameraType type);
		CameraType GetCameraType() const;

		const Matrix4x4& GetMatrix(const Vector3& position) const;
		Matrix4x4 GetStaticMatrix() const;
		GResource<FrameBuffer> GetFrameBuffer() const;
		GResource<Texture> GetRenderTexture() const;
		void ListenWindowResizeEvent();
		void ResizeRenderTexture(size_t width, size_t height);

		const Vector3& GetDirection() const;
		void SetDirection(const Vector3& direction);
		float GetHorizontalAngle() const;
		float GetVerticalAngle() const;
		void SetBloomWeight(float weight);
		float GetBloomWeight() const;
		size_t GetBloomIterations() const;
		void SetBloomIterations(size_t iterCount);
		float GetExposure() const;
		void SetExposure(float exp);
		float GetMoveSpeed() const;
		void SetMoveSpeed(float speed);
		float GetRotateSpeed() const;
		void SetRotateSpeed(float speed);
		CameraController& Rotate(float horz, float vert);

		void SetForwardVector(const Vector3& forward);
		void SetUpVector(const Vector3& up);
		void SetRightVector(const Vector3& right);

		const Vector3& GetForwardVector() const;
		const Vector3& GetUpVector() const;
		const Vector3& GetRightVector() const;
	};
}