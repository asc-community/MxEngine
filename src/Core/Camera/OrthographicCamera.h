#pragma once

#include "Core/Interfaces/ICamera.h"

namespace MomoEngine
{
	class OrthographicCamera : public ICamera
	{
		mutable glm::mat4x4 matrix;
		mutable glm::mat4x4 view;
		mutable glm::mat4x4 projection;
		glm::vec4 projectLimits = { -1.0f, 1.0f, -1.0f, 1.0f };
		float aspectRatio = 1.777f;
		float zNear = 0.1f, zFar = 100.0f;
		mutable bool updateMatrix = true;
		mutable bool updateProjection = true;
	public:
		void SetProjection(float left, float right, float bottom, float top);
		
		// Inherited via ICamera
		virtual const glm::mat4x4& GetMatrix() const override;
		virtual void SetViewMatrix(const glm::mat4x4& view) override;
		virtual void SetAspectRatio(float w, float h = 1.0f) override;
		virtual float GetAspectRatio() const override;
		virtual void SetZNear(float zNear) override;
		virtual void SetZFar(float zFar) override;
		virtual float GetZNear() const override;
		virtual float GetZFar() const override;

		// Inherited via ICamera
		virtual void SetZoom(float zoom) override;
		virtual float GetZoom() const override;
	};
}