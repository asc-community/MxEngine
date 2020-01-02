#pragma once

#include "Core/Interfaces/ICamera.h"
#include "Utilities/Memory/Memory.h"

namespace MomoEngine
{
	class CameraController
	{
		UniqueRef<ICamera> camera;
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 direction = { 1.0f, 0.0f, 0.0f };
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 forward = { 1.0f, 0.0f, 0.0f };
		glm::vec3 right = { 0.0f, 0.0f, 1.0f };
		float verticalAngle = 0.0f;
		float horizontalAngle = 0.0f;
		float zoom = 1.0f;
		mutable bool updateCamera = true;
	public:

		bool HasCamera() const;
		void SetCamera(UniqueRef<ICamera> camera);
		ICamera& GetCamera();
		const glm::mat4x4& GetCameraMatrix() const;

		glm::vec3 GetPosition() const;
		glm::vec3 GetDirection() const;
		glm::vec3 GetUpVector() const;
		void SetPosition(const glm::vec3& position);
		void SetDirection(const glm::vec3& direction);
		void SetUpVector(const glm::vec3& up);
		void SetZoom(float zoom);
		float GetHorizontalAngle() const;
		float GetVerticalAngle() const;
		float GetZoom() const;

		CameraController& Translate(float x, float y, float z);
		CameraController& Translate(const glm::vec3& vec);
		CameraController& TranslateX(float x);
		CameraController& TranslateY(float y);
		CameraController& TranslateZ(float z);
		CameraController& Rotate(float horz, float vert);
		CameraController& TranslateForward(float dist);
		CameraController& TranslateRight(float dist);
		CameraController& TranslateUp(float dist);
	};
}