#include "PerspectiveCamera.h"

namespace MxEngine
{
	const Matrix4x4& PerspectiveCamera::GetMatrix() const
	{
		if (this->updateMatrix)
		{
			if (this->updateProjection)
			{
				const float fov = Clamp(zoom * FOV, Radians(10.0f), Radians(150.0f));
				this->projection = MakePerspectiveMatrix(fov, aspectRatio, zNear, zFar);
				this->updateProjection = false;
			}
			this->matrix = this->projection * this->view;
			this->updateMatrix = false;
		}
		return this->matrix;
	}

	void PerspectiveCamera::SetViewMatrix(const Matrix4x4& view)
	{
		this->view = view;
		this->updateMatrix = true;
	}

	void PerspectiveCamera::SetFOV(float angle)
	{
		updateMatrix = true;
		updateProjection = true;
		this->FOV = Radians(angle);
	}

	float PerspectiveCamera::GetFOV() const
	{
		return Degrees(this->FOV);
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