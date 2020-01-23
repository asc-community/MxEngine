#pragma once

#include "Utilities/Math/Math.h"

namespace MomoEngine
{
	struct ICamera
	{
		virtual const Matrix4x4& GetMatrix() const = 0;
		virtual void SetViewMatrix(const Matrix4x4& view) = 0;
		virtual void SetAspectRatio(float w, float h = 1.0f) = 0;
		virtual float GetAspectRatio() const = 0;
		virtual void SetZNear(float zNear) = 0;
		virtual void SetZFar(float zFar) = 0;
		virtual float GetZNear() const = 0;
		virtual float GetZFar() const = 0;
		virtual void SetZoom(float zoom) = 0;
		virtual float GetZoom() const = 0;

		virtual ~ICamera() = default;
	};
}