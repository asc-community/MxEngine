#pragma once
#include <glm/glm.hpp>

namespace MomoEngine
{
	class Camera
	{
		mutable glm::mat4x4 matrix;
		mutable glm::mat4x4 view;
		mutable glm::mat4x4 projection;
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 direction = { 1.0f, 0.0f, 0.0f };
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 forward = { 1.0f, 0.0f, 0.0f };
		glm::vec3 right = { 0.0f, 0.0f, 1.0f };
		float FOV = 60.0f;
		float aspectRatio = 1.777f;
		float zNear = 0.1f, zFar = 100.0f;
		float verticalAngle = 0.0f, horizontalAngle = 0.0f;
		mutable bool updateProjection = false;
		mutable bool updateView = false;
		mutable bool updateMatrix = false;
	public:
		glm::mat4x4 GetMatrix() const;
		glm::vec3 GetPosition();
		glm::vec3 GetDirection();
		glm::vec3 GetUpVector();
		void SetPosition(const glm::vec3& position);
		void SetDirection(const glm::vec3& direction);
		void SetUpVector(const glm::vec3& up);
		Camera& Translate(float x, float y, float z);
		Camera& Translate(const glm::vec3& vec);
		Camera& TranslateX(float x);
		Camera& TranslateY(float y);
		Camera& TranslateZ(float z);
		Camera& Rotate(float horizontal, float vertical);
		Camera& TranslateForward(float dist);
		Camera& TranslateRight(float dist);
		Camera& TranslateUp(float dist);
		void SetFOV(float angle);
		float GetFOV() const;
		void SetAspectRatio(float w, float h);
		void SetAspectRatio(float aspectRatio);
		float GetAspectRatio() const;
		float GetZFar() const;
		void SetZFar(float zFar);
		float GetZNear() const;
		void SetZNear(float zNear);
		float GetHorizontalAngle() const;
		float GetVerticalAngle() const;
	};
}