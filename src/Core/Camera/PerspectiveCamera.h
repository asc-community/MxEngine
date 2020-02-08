#pragma once

#include "Core/Interfaces/ICamera.h"

namespace MxEngine
{
	class PerspectiveCamera : public ICamera
	{
		mutable Matrix4x4 matrix{0};
		mutable Matrix4x4 view{0};
		mutable Matrix4x4 projection{0};
		float FOV = Radians(65.0f);
		float zoom = 1.0f;
		float aspectRatio = 1.777f;
		float zNear = 0.1f, zFar = 100.0f;
		mutable bool updateProjection = true;
		mutable bool updateMatrix = true;
	public:
		void SetFOV(float angle);
		float GetFOV() const;

		// Inherited via ICamera
		virtual const Matrix4x4& GetMatrix() const override;
		virtual void SetViewMatrix(const Matrix4x4& view) override;
		virtual void SetAspectRatio(float w, float h = 1.0f) override;
		virtual float GetAspectRatio() const override;
		virtual float GetZFar() const override;
		virtual void SetZFar(float zFar) override;
		virtual float GetZNear() const override;
		virtual void SetZNear(float zNear) override;

		// Inherited via ICamera
		virtual void SetZoom(float zoom) override;
		virtual float GetZoom() const override;
	};
}