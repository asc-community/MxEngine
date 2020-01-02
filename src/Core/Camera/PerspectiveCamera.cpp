#include "PerspectiveCamera.h"
#include <glm/ext.hpp>
#include <cmath>

namespace MomoEngine
{
	const glm::mat4x4& PerspectiveCamera::GetMatrix() const
	{
		if (this->updateMatrix)
		{
			if (this->updateProjection)
			{
				float fov = glm::clamp(zoom * FOV, glm::radians(10.0f), glm::radians(150.0f));
				this->projection = glm::perspective(fov, aspectRatio, zNear, zFar);
				this->updateProjection = false;
			}
			this->matrix = this->projection * this->view;
			this->updateMatrix = false;
		}
		return this->matrix;
	}

	void PerspectiveCamera::SetViewMatrix(const glm::mat4x4& view)
	{
		this->view = view;
		this->updateMatrix = true;
	}

	void PerspectiveCamera::SetFOV(float angle)
	{
		updateMatrix = true;
		updateProjection = true;
		this->FOV = glm::radians(angle);
	}

	float PerspectiveCamera::GetFOV() const
	{
		return glm::degrees(this->FOV);
	}

	void PerspectiveCamera::SetAspectRatio(float w, float h)
	{
		updateMatrix = true;
		updateProjection = true;
		this->aspectRatio = w / h;
	}
	float PerspectiveCamera::GetAspectRatio() const
	{
		return this->aspectRatio;
	}

	float PerspectiveCamera::GetZFar() const
	{
		return this->zFar;
	}

	void PerspectiveCamera::SetZFar(float zFar)
	{
		updateMatrix = true;
		updateProjection = true;
		this->zFar = zFar;
	}
	
	float PerspectiveCamera::GetZNear() const
	{
		return this->zNear;
	}

	void PerspectiveCamera::SetZNear(float zNear)
	{
		updateMatrix = true;
		updateProjection = true;
		this->zNear = zNear;
	}

	float PerspectiveCamera::GetZoom() const
	{
		return this->zoom;
	}

	void PerspectiveCamera::SetZoom(float zoom)
	{
		this->zoom = zoom;
		this->updateProjection = true;
		this->updateMatrix = true;
	}
}