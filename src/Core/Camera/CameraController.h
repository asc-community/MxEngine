#pragma once

#include "Core/Interfaces/ICamera.h"
#include "Utilities/Memory/Memory.h"

namespace chaiscript
{
	class ChaiScript;
}

namespace MomoEngine
{
	class CameraController
	{
		UniqueRef<ICamera> camera;
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		Vector3 direction = { 1.0f, 0.0f, 0.0f };
		Vector3 up = { 0.0f, 1.0f, 0.0f };
		Vector3 forward = { 1.0f, 0.0f, 0.0f };
		Vector3 right = { 0.0f, 0.0f, 1.0f };
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

		Vector3 GetPosition() const;
		Vector3 GetDirection() const;
		Vector3 GetUpVector() const;
		float GetMoveSpeed() const;
		float GetRotateSpeed() const;
		void SetPosition(const Vector3& position);
		void SetDirection(const Vector3& direction);
		void SetUpVector(const Vector3& up);
		void SetMoveSpeed(float speed);
		void SetRotateSpeed(float speed);
		void SetZoom(float zoom);
		float GetHorizontalAngle() const;
		float GetVerticalAngle() const;
		float GetZoom() const;

		CameraController& Translate(float x, float y, float z);
		CameraController& Translate(const Vector3& vec);
		CameraController& TranslateX(float x);
		CameraController& TranslateY(float y);
		CameraController& TranslateZ(float z);
		CameraController& Rotate(float horz, float vert);
		CameraController& TranslateForward(float dist);
		CameraController& TranslateRight(float dist);
		CameraController& TranslateUp(float dist);
	};

	class ChaiScriptCamera
	{
	public:
		static void Init(chaiscript::ChaiScript& chai);
	};
}