#include "OrthographicCamera.h"
#include <glm/ext.hpp>
#include <algorithm>

namespace MomoEngine
{
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		this->updateProjection = true;
		this->updateMatrix = true;
		this->projectLimits = { left, right, bottom, top };
	}

	const glm::mat4x4& OrthographicCamera::GetMatrix() const
	{
		if (this->updateMatrix)
		{
			if (this->updateProjection)
			{
				float left = this->projectLimits.x;
				float right = this->projectLimits.y;
				float bottom = this->projectLimits.z;
				float top = this->projectLimits.w;
				this->projection = glm::ortho(
					left * this->aspectRatio,
					right * this->aspectRatio,
					bottom,
					top,
					this->zNear,
					this->zFar
				);
				this->updateProjection = false;
			}
			this->matrix = this->projection * this->view;
			this->updateMatrix = false;
		}
		return this->matrix;
	}

	void OrthographicCamera::SetViewMatrix(const glm::mat4x4& view)
	{
		this->view = view;
		this->updateMatrix = true;
	}

	void OrthographicCamera::SetAspectRatio(float w, float h)
	{
		updateMatrix = true;
		updateProjection = true;
		this->aspectRatio = w / h;
	}

	float OrthographicCamera::GetAspectRatio() const
	{
		return this->aspectRatio;
	}

	void OrthographicCamera::SetZNear(float zNear)
	{
		this->zNear = zNear;
		this->updateProjection = true;
		this->updateMatrix = true;
	}

	void OrthographicCamera::SetZFar(float zFar)
	{
		this->zFar = zFar;
		this->updateProjection = true;
		this->updateMatrix = true;
	}

	float OrthographicCamera::GetZNear() const
	{
		return this->zNear;
	}

	float OrthographicCamera::GetZFar() const
	{
		return this->zFar;
	}
	void OrthographicCamera::SetZoom(float zoom)
	{
		this->SetProjection(-zoom, zoom, -zoom, zoom);
	}

	float OrthographicCamera::GetZoom() const
	{
		return this->projectLimits.y; // y, w possible as they are 1 * zoom
	}
}