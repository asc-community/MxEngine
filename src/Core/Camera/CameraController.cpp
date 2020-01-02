#include "CameraController.h"
#include <glm/ext.hpp>

namespace MomoEngine
{
	bool CameraController::HasCamera() const
	{
		return this->camera != nullptr;
	}

	void CameraController::SetCamera(UniqueRef<ICamera> camera)
	{
		this->camera = std::move(camera);
	}

	ICamera& CameraController::GetCamera()
	{
		return *this->camera;
	}

	const glm::mat4x4& CameraController::GetCameraMatrix() const
	{
		if (updateCamera)
		{
			auto view = glm::lookAt(position, position + direction, up);
			this->camera->SetViewMatrix(view);
			this->updateCamera = false;
		}
		return this->camera->GetMatrix();
	}

	glm::vec3 CameraController::GetPosition() const
	{
		return this->position;
	}

	glm::vec3 CameraController::GetDirection() const
	{
		return this->direction;
	}

	glm::vec3 CameraController::GetUpVector() const
	{
		return this->up;
	}

	void CameraController::SetPosition(const glm::vec3& position)
	{
		this->position = position;
		this->updateCamera = true;
	}

	void CameraController::SetDirection(const glm::vec3& direction)
	{
		this->direction = direction;
		this->updateCamera = true;
	}

	void CameraController::SetUpVector(const glm::vec3& up)
	{
		this->up = up;
		this->updateCamera = true;
	}

	void CameraController::SetZoom(float zoom)
	{
		this->camera->SetZoom(zoom);
	}

	float CameraController::GetHorizontalAngle() const
	{
		return this->horizontalAngle;
	}

	float CameraController::GetVerticalAngle() const
	{
		return this->verticalAngle;
	}

	float CameraController::GetZoom() const
	{
		return this->camera->GetZoom();
	}

	CameraController& CameraController::Translate(float x, float y, float z)
	{
		return Translate({ x, y, z });
	}

	CameraController& CameraController::Translate(const glm::vec3& vec)
	{
		this->position += vec;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::TranslateX(float x)
	{
		this->position.x += x;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::TranslateY(float y)
	{
		this->position.y += y;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::TranslateZ(float z)
	{
		this->position.z += z;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::Rotate(float horizontal, float vertical)
	{
		this->horizontalAngle += horizontal;
		this->verticalAngle += vertical;
		this->verticalAngle = glm::clamp(this->verticalAngle, 
			-glm::half_pi<float>() + 0.001f, glm::half_pi<float>() - 0.001f);

		this->SetDirection({
			std::cos(verticalAngle) * std::sin(horizontalAngle),
			std::sin(verticalAngle),
			std::cos(verticalAngle) * std::cos(horizontalAngle)
			});

		this->forward = glm::vec3(
			sin(horizontalAngle),
			0.0f,
			cos(horizontalAngle)
		);

		this->right = glm::vec3(
			sin(horizontalAngle - glm::half_pi<float>()),
			0.0f,
			cos(horizontalAngle - glm::half_pi<float>())
		);
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::TranslateForward(float dist)
	{
		return Translate(this->forward * dist);
	}

	CameraController& CameraController::TranslateRight(float dist)
	{
		return Translate(this->right * dist);
	}

	CameraController& CameraController::TranslateUp(float dist)
	{
		return Translate(this->up * dist);
	}
}