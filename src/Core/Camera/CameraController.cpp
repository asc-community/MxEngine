#include "CameraController.h"

namespace MxEngine
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

	const Matrix4x4& CameraController::GetCameraMatrix() const
	{
		if (updateCamera)
		{
			auto view = MakeViewMatrix(position, position + direction, up);
			this->camera->SetViewMatrix(view);
			this->updateCamera = false;
		}
		return this->camera->GetMatrix();
	}

	const Vector3& CameraController::GetPosition() const
	{
		return this->position;
	}

	const Vector3& CameraController::GetDirection() const
	{
		return this->direction;
	}

    const Vector3& CameraController::GetUp() const
    {
        return this->up;
    }

	float CameraController::GetMoveSpeed() const
	{
		return this->moveSpeed;
	}

	float CameraController::GetRotateSpeed() const
	{
		return this->rotateSpeed;
	}

	void CameraController::SetPosition(const Vector3& position)
	{
		this->position = position;
		this->updateCamera = true;
	}

	void CameraController::SetDirection(const Vector3& direction)
	{
		this->direction = direction;
		this->updateCamera = true;
	}

	void CameraController::SetUp(const Vector3& up)
	{
		this->up = up;
		this->updateCamera = true;
	}

	void CameraController::SetMoveSpeed(float speed)
	{
		this->moveSpeed = speed;
	}

	void CameraController::SetRotateSpeed(float speed)
	{
		this->rotateSpeed = speed;
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

	CameraController& CameraController::Translate(const Vector3& vec)
	{
		this->position += this->moveSpeed * vec;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::TranslateX(float x)
	{
		this->position.x += this->moveSpeed * x;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::TranslateY(float y)
	{
		this->position.y += this->moveSpeed * y;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::TranslateZ(float z)
	{
		this->position.z += this->moveSpeed * z;
		this->updateCamera = true;
		return *this;
	}

	CameraController& CameraController::Rotate(float horizontal, float vertical)
	{
		this->horizontalAngle += this->rotateSpeed * horizontal;
		this->verticalAngle += this->rotateSpeed * vertical;

		this->verticalAngle = Clamp(this->verticalAngle, 
			-HalfPi<float>() + 0.001f, HalfPi<float>() - 0.001f);
		while (this->horizontalAngle >= TwoPi<float>())
			this->horizontalAngle -= TwoPi<float>();
		while (this->horizontalAngle < 0)
			this->horizontalAngle += TwoPi<float>();

		this->SetDirection({
			std::cos(verticalAngle) * std::sin(horizontalAngle),
			std::sin(verticalAngle),
			std::cos(verticalAngle) * std::cos(horizontalAngle)
			});

		this->forward = Vector3(
			sin(horizontalAngle),
			0.0f,
			cos(horizontalAngle)
		);

		this->right = Vector3(
			sin(horizontalAngle - HalfPi<float>()),
			0.0f,
			cos(horizontalAngle - HalfPi<float>())
		);
		this->updateCamera = true;
		return *this;
	}

    void CameraController::SetForwardVector(const Vector3& forward)
    {
        this->forward = forward;
    }

    void CameraController::SetUpVector(const Vector3& up)
    {
        this->up = up;
    }

    void CameraController::SetRightVector(const Vector3& right)
    {
        this->right = right;
    }

    const Vector3& CameraController::GetForwardVector() const
    {
        return this->forward;
    }

    const Vector3& CameraController::GetUpVector() const
    {
        return this->up;
    }

    const Vector3& CameraController::GetRightVector() const
    {
        return this->right;
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