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
#include "Utilities/Memory/Memory.h"
#include "Platform/GraphicAPI.h"
#include "CameraBase.h"

namespace MxEngine
{
	enum class CameraType : uint8_t
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
		FRUSTRUM,
	};

	struct CameraRender
	{
		FrameBufferHandle GBuffer;
		TextureHandle Albedo;
		TextureHandle Normal;
		TextureHandle Material;
		TextureHandle Depth;
		TextureHandle AverageWhite;
		TextureHandle HDR;
		TextureHandle SwapHDR;

		void Init(int width, int height);
		void Resize(int width, int height);
		void DeInit();
	};

	class CameraController
	{
		MAKE_COMPONENT(CameraController);

		UniqueRef<CameraRender> renderBuffers = MakeUnique<CameraRender>();
		TextureHandle renderTexture;

		Vector3 direction = { 0.0f, 0.0f, 1.0f };
		Vector3 up = { 0.0f, 1.0f, 0.0f };
		Vector3 forward ={ 0.0f, 0.0f, 1.0f };
		Vector3 right = { -1.0f, 0.0f, 0.0f };
		float verticalAngle = 0.0f;
		float horizontalAngle = 0.0f;
		float moveSpeed = 1.0f;
		float rotateSpeed = 10.0f;

		void SubmitMatrixProjectionChanges() const;
		void RecalculateRotationAngles();
		UUID GetEventUUID() const;

		CameraType cameraType = CameraType::PERSPECTIVE;
		bool renderingEnabled = true;
	public:
		mutable CameraBase Camera;

		CameraController();
		~CameraController();

		template<typename T>
		T& GetCamera();
		template<typename T>
		const T& GetCamera() const;
		void SetCameraType(CameraType type);
		CameraType GetCameraType() const;
		template<typename T>
		void SetCamera(const T& camera);

		const Matrix4x4& GetMatrix(const Vector3& position) const;
		const Matrix4x4& GetViewMatrix(const Vector3& position) const;
		const Matrix4x4& GetProjectionMatrix() const;
		Matrix4x4 GetStaticMatrix() const;
		Matrix4x4 GetStaticViewMatrix() const;
		TextureHandle GetRenderTexture() const;
		void ListenWindowResizeEvent();
		bool IsListeningWindowResizeEvent() const;
		void ResizeRenderTexture(size_t width, size_t height);
		void SetRenderTexture(const TextureHandle& texture);
		bool IsRendering() const;
		void ToggleRendering(bool value);
		const FrustrumCuller& GetFrustrumCuller() const;

		Vector3 GetDirection() const;
		const Vector3& GetDirectionDenormalized() const;
		void SetDirection(const Vector3& direction);
		Vector3 GetDirectionUp() const;
		float GetHorizontalAngle() const;
		float GetVerticalAngle() const;
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

		FrameBufferHandle GetGBuffer() const;
		TextureHandle GetAlbedoTexture() const;
		TextureHandle GetNormalTexture() const;
		TextureHandle GetMaterialTexture() const;
		TextureHandle GetDepthTexture() const;
		TextureHandle GetAverageWhiteTexture() const;
		TextureHandle GetHDRTexture() const;
		TextureHandle GetSwapHDRTexture() const;
	};
}