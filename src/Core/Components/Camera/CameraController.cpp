// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "CameraController.h"
#include "Core/Application/Application.h"
#include "Core/Event/Events/WindowResizeEvent.h"

namespace MxEngine
{
	CameraController::CameraController()
		: framebuffer(GraphicFactory::Create<FrameBuffer>()), renderbuffer(GraphicFactory::Create<RenderBuffer>()), texture(GraphicFactory::Create<Texture>())
	{
		// TODO: let user create texture or do it here automatically?
		VectorInt2 viewport{ Application::Get()->GetWindow().GetWidth(), Application::Get()->GetWindow().GetHeight() };
		constexpr size_t samples = 4;
		auto multisampledTexture = GraphicFactory::Create<Texture>();

		multisampledTexture->LoadMultisample(viewport.x, viewport.y, TextureFormat::RGBA16F, samples);
		this->framebuffer->AttachTexture(multisampledTexture);
		this->renderbuffer->InitStorage((int)multisampledTexture->GetWidth(), (int)multisampledTexture->GetHeight(), multisampledTexture->GetSampleCount());
		this->renderbuffer->LinkToFrameBuffer(*this->framebuffer);
		this->framebuffer->Validate();

		this->texture->Load(nullptr, viewport.x, viewport.y, TextureFormat::RGB16F, TextureWrap::CLAMP_TO_EDGE);
	}

	CameraController::~CameraController()
	{
		Application::Get()->GetEventDispatcher().RemoveEventListener(this->framebuffer.GetUUID());
	}

	bool CameraController::HasCamera() const
	{
		return this->camera != nullptr;
	}

	void CameraController::SetCamera(UniqueRef<ICamera> camera)
	{
		this->camera = std::move(camera);
		this->updateCamera = true;
	}

	ICamera& CameraController::GetCamera()
	{
		return *this->camera;
	}

	const ICamera& CameraController::GetCamera() const
	{
		return *this->camera;
	}

	const Matrix4x4& CameraController::GetMatrix(const Vector3& position) const
	{
		static Matrix4x4 defaultMatrix(0.0f);
		if (!this->HasCamera()) return defaultMatrix;

		auto view = MakeViewMatrix(position, position + direction, up);
		this->camera->SetViewMatrix(view);
		this->updateCamera = false;

		return this->camera->GetMatrix();
	}

    Matrix4x4 CameraController::GetStaticMatrix() const
    {
		if (!this->HasCamera()) return Matrix4x4(0.0f);
		auto ViewMatrix = (Matrix3x3)this->GetCamera().GetViewMatrix();
		const Matrix4x4& ProjectionMatrix = this->GetCamera().GetProjectionMatrix();
		return ProjectionMatrix * (Matrix4x4)ViewMatrix;
    }

	GResource<FrameBuffer> CameraController::GetFrameBuffer() const
	{
		return this->framebuffer;
	}

	GResource<Texture> CameraController::GetTexture() const
	{
		return this->texture;
	}

	void CameraController::FitScreenViewport()
	{
		Application::Get()->GetEventDispatcher().RemoveEventListener(this->framebuffer.GetUUID());

		Application::Get()->GetEventDispatcher().AddEventListener<WindowResizeEvent>(this->framebuffer.GetUUID(),
		[framebufferTexture = GetAttachedTexture(this->framebuffer), renderbuffer = this->renderbuffer, outputTexture = this->texture](WindowResizeEvent& e) mutable
		{
			if (e.Old != e.New)
			{
				auto width = (int)e.New.x;
				auto height = (int)e.New.y;
				if (framebufferTexture->IsMultisampled())
					framebufferTexture->LoadMultisample(width, height, framebufferTexture->GetFormat(), framebufferTexture->GetSampleCount(), framebufferTexture->GetWrapType());
				else if (framebufferTexture->IsDepthOnly())
					framebufferTexture->LoadDepth(width, height, framebufferTexture->GetWrapType());
				else
					framebufferTexture->Load(nullptr, width, height, framebufferTexture->GetFormat(), framebufferTexture->GetWrapType());

				renderbuffer->InitStorage((int)framebufferTexture->GetWidth(), (int)framebufferTexture->GetHeight(), framebufferTexture->GetSampleCount());
				outputTexture->Load(nullptr, width, height, TextureFormat::RGB, TextureWrap::CLAMP_TO_EDGE);
			}
		});
	}

	const Vector3& CameraController::GetDirection() const
	{
		return this->direction;
	}

	void CameraController::SetDirection(const Vector3& direction)
	{
		this->direction = direction;
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

    void CameraController::SetBloomWeight(float weight)
    {
		this->bloomWeight = Max(0.0f, weight);
    }

	float CameraController::GetBloomWeight() const
	{
		return this->bloomWeight;
	}

	size_t CameraController::GetBloomIterations() const
	{
		return (size_t)this->bloomIterations;
	}

	void CameraController::SetBloomIterations(size_t iterCount)
	{
		this->bloomIterations = (uint8_t)Min(iterCount + iterCount % 2, std::numeric_limits<decltype(this->bloomIterations)>::max() - 1);
	}

	float CameraController::GetExposure() const
	{
		return this->exposure;
	}

	void CameraController::SetExposure(float exp)
	{
		this->exposure = Max(0.0f, exp);
	}

	float CameraController::GetMoveSpeed() const
	{
		return this->moveSpeed;
	}

	void CameraController::SetMoveSpeed(float speed)
	{
		this->moveSpeed = Max(speed, 0.0f);
	}

	float CameraController::GetRotateSpeed() const
	{
		return this->rotateSpeed;
	}

	void CameraController::SetRotateSpeed(float speed)
	{
		this->rotateSpeed = Max(speed, 0.0f);
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

		this->forward = MakeVector3(
			sin(horizontalAngle),
			0.0f,
			cos(horizontalAngle)
		);

		this->right = MakeVector3(
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
		this->updateCamera = true;
	}

	void CameraController::SetRightVector(const Vector3& right)
	{
		this->right = right;
	}

	const Vector3& CameraController::GetForwardVector() const
	{
		return this->forward;
	}

	const Vector3& CameraController::GetUpVector() const //-V524
	{
		return this->up;
	}

	const Vector3& CameraController::GetRightVector() const
	{
		return this->right;
	}
}