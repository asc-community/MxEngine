#include "Camera.h"
#include <glm/ext.hpp>
#include <algorithm>
#include <cmath>

namespace MomoEngine
{
	glm::mat4x4 Camera::GetMatrix() const
	{
		if (this->updateMatrix)
		{
			if (this->updateProjection)
			{
				this->projection = glm::perspective(FOV, aspectRatio, zNear, zFar);
				this->updateProjection = false;
			}
			if (this->updateView)
			{
				this->view = glm::lookAt(position, position + direction, up);
				this->updateView = false;
			}
			this->matrix = this->projection * this->view;
			this->updateMatrix = false;
		}
		return this->matrix;
	}

	glm::vec3 Camera::GetPosition()
	{
		return this->position;
	}

	glm::vec3 Camera::GetDirection()
	{
		return this->direction;
	}

	glm::vec3 Camera::GetUpVector()
	{
		return this->up;
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		updateMatrix = true;
		updateView = true;
		this->position = position;
	}

	void Camera::SetDirection(const glm::vec3& direction)
	{
		updateMatrix = true;
		updateView = true;
		this->direction = direction;
	}

	void Camera::SetUpVector(const glm::vec3& up)
	{
		updateMatrix = true;
		updateView = true;
		this->up = up;
	}

	Camera& Camera::Translate(float x, float y, float z)
	{
		return Translate({ x, y, z });
	}

	Camera& Camera::Translate(const glm::vec3& vec)
	{
		updateMatrix = true;
		updateView = true;
		this->position += vec;
		return *this;
	}

	Camera& Camera::TranslateX(float x)
	{
		this->position.x += x;
		updateMatrix = true;
		updateView = true;
		return *this;
	}

	Camera& Camera::TranslateY(float y)
	{
		this->position.y += y;
		updateMatrix = true;
		updateView = true;
		return *this;
	}

	Camera& Camera::TranslateZ(float z)
	{
		this->position.z += z;
		updateMatrix = true;
		updateView = true;
		return *this;
	}

	void Camera::SetFOV(float angle)
	{
		updateMatrix = true;
		updateProjection = true;
		this->FOV = glm::radians(angle);
	}

	float Camera::GetFOV() const
	{
		return glm::degrees(this->FOV);
	}

	void Camera::SetAspectRatio(float w, float h)
	{
		SetAspectRatio(w / h);
	}

	void Camera::SetAspectRatio(float aspectRatio)
	{
		updateMatrix = true;
		updateProjection = true;
		this->aspectRatio = aspectRatio;
	}

	float Camera::GetAspectRatio() const
	{
		return this->aspectRatio;
	}

	float Camera::GetZFar() const
	{
		return this->zFar;
	}

	void Camera::SetZFar(float zFar)
	{
		updateMatrix = true;
		updateProjection = true;
		this->zFar = zFar;
	}
	
	float Camera::GetZNear() const
	{
		return this->zNear;
	}

	void Camera::SetZNear(float zNear)
	{
		updateMatrix = true;
		updateProjection = true;
		this->zNear = zNear;
	}

	Camera& Camera::Rotate(float horizontal, float vertical)
	{
		this->horizontalAngle += horizontal;
		this->verticalAngle += vertical;
		this->verticalAngle = std::max(-glm::half_pi<float>() + 0.0001f, this->verticalAngle);
		this->verticalAngle = std::min( glm::half_pi<float>() - 0.0001f, this->verticalAngle);

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
		return *this;
	}

	Camera& Camera::TranslateForward(float dist)
	{
		return Translate(this->forward * dist);
	}

	Camera& Camera::TranslateRight(float dist)
	{
		return Translate(this->right * dist);
	}

	Camera& Camera::TranslateUp(float dist)
	{
		return Translate(this->up * dist);
	}

	float Camera::GetHorizontalAngle() const
	{
		return this->horizontalAngle;
	}

	float Camera::GetVerticalAngle() const
	{
		return this->verticalAngle;
	}
}