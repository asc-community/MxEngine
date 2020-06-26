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
#include "Core/Event/Events/WindowResizeEvent.h"
#include "Core/Application/EventManager.h"
#include "Platform/Window/WindowManager.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"

namespace MxEngine
{
	template<>
	PerspectiveCamera& CameraController::GetCamera<PerspectiveCamera>()
	{
		MX_ASSERT(this->GetCameraType() == CameraType::PERSPECTIVE);
		static_assert(sizeof(PerspectiveCamera) == sizeof(this->Camera), "camera byte storage size mismatch");
		return *reinterpret_cast<PerspectiveCamera*>(&this->Camera); //-V717
	}

	template<>
	const PerspectiveCamera& CameraController::GetCamera<PerspectiveCamera>() const
	{
		MX_ASSERT(this->GetCameraType() == CameraType::PERSPECTIVE);
		static_assert(sizeof(PerspectiveCamera) == sizeof(this->Camera), "camera byte storage size mismatch");
		return *reinterpret_cast<const PerspectiveCamera*>(&this->Camera); //-V717
	}

	template<>
	OrthographicCamera& CameraController::GetCamera<OrthographicCamera>()
	{
		MX_ASSERT(this->GetCameraType() == CameraType::ORTHOGRAPHIC);
		static_assert(sizeof(OrthographicCamera) == sizeof(this->Camera), "camera byte storage size mismatch");
		return *reinterpret_cast<OrthographicCamera*>(&this->Camera); //-V717
	}

	template<>
	const OrthographicCamera& CameraController::GetCamera<OrthographicCamera>() const
	{
		MX_ASSERT(this->GetCameraType() == CameraType::ORTHOGRAPHIC);
		static_assert(sizeof(OrthographicCamera) == sizeof(this->Camera), "camera byte storage size mismatch");
		return *reinterpret_cast<const OrthographicCamera*>(&this->Camera); //-V717
	}

	CameraController::CameraController()
		: framebuffer(GraphicFactory::Create<FrameBuffer>()), renderbuffer(GraphicFactory::Create<RenderBuffer>()), renderTexture(GraphicFactory::Create<Texture>())
	{
		this->SetCameraType(CameraType::PERSPECTIVE);

		// TODO: let user create texture or do it here automatically?
		auto viewport = (VectorInt2)WindowManager::GetSize();
		constexpr size_t samples = 4;
		auto multisampledTexture = GraphicFactory::Create<Texture>();

		multisampledTexture->LoadMultisample(viewport.x, viewport.y, TextureFormat::RGBA16F, samples);
		this->framebuffer->AttachTexture(multisampledTexture);
		this->renderbuffer->InitStorage((int)multisampledTexture->GetWidth(), (int)multisampledTexture->GetHeight(), multisampledTexture->GetSampleCount());
		this->renderbuffer->LinkToFrameBuffer(*this->framebuffer);
		this->framebuffer->Validate();

		this->renderTexture->Load(nullptr, viewport.x, viewport.y, TextureFormat::RGB, TextureWrap::CLAMP_TO_EDGE);
	}

	CameraController::~CameraController()
	{
		EventManager::FlushEvents(); // avoid removing event while its still in queue
		EventManager::RemoveEventListener(this->framebuffer.GetUUID());
	}

	void CameraController::SetCameraType(CameraType type)
	{
		this->cameraType = type;
		this->Camera.SetZoom(1.0f);
	}

	CameraType CameraController::GetCameraType() const
	{
		return this->cameraType;
	}

	const Matrix4x4& CameraController::GetMatrix(const Vector3& position) const
	{
		if (this->Camera.UpdateProjection) this->SubmitMatrixProjectionChanges();
		
		auto view = MakeViewMatrix(position, position + direction, up);
		this->Camera.SetViewMatrix(view);

		return this->Camera.GetMatrix();
	}

    Matrix4x4 CameraController::GetStaticMatrix() const
    {
		if (this->Camera.UpdateProjection) this->SubmitMatrixProjectionChanges();
		
		auto view = MakeViewMatrix(MakeVector3(0.0f), direction, up);
		this->Camera.SetViewMatrix(view);

		auto ViewMatrix = (Matrix3x3)this->Camera.GetViewMatrix();
		const Matrix4x4& ProjectionMatrix = this->Camera.GetProjectionMatrix();
		return ProjectionMatrix * (Matrix4x4)ViewMatrix;
    }

	GResource<FrameBuffer> CameraController::GetFrameBuffer() const
	{
		return this->framebuffer;
	}

	GResource<Texture> CameraController::GetRenderTexture() const
	{
		return this->renderTexture;
	}

	void CameraController::ListenWindowResizeEvent()
	{
		EventManager::RemoveEventListener(this->framebuffer.GetUUID());

		EventManager::AddEventListener<WindowResizeEvent>(this->framebuffer.GetUUID(),
		[camera = MxObject::GetByComponent(*this).GetComponent<CameraController>()](WindowResizeEvent& e) mutable
		{
			if (camera.IsValid() && (e.Old != e.New))
			{
				camera->ResizeRenderTexture((size_t)e.New.x, (size_t)e.New.y);
			}
		});
	}

	void CameraController::ResizeRenderTexture(size_t w, size_t h)
	{
		auto width = (int)w;
		auto height = (int)h;
		auto framebufferTexture = GetAttachedTexture(this->GetFrameBuffer());

		if (framebufferTexture->IsMultisampled())
			framebufferTexture->LoadMultisample(width, height, framebufferTexture->GetFormat(), framebufferTexture->GetSampleCount(), framebufferTexture->GetWrapType());
		else if (framebufferTexture->IsDepthOnly())
			framebufferTexture->LoadDepth(width, height, framebufferTexture->GetWrapType());
		else
			framebufferTexture->Load(nullptr, width, height, framebufferTexture->GetFormat(), framebufferTexture->GetWrapType());

		this->renderbuffer->InitStorage((int)framebufferTexture->GetWidth(), (int)framebufferTexture->GetHeight(), framebufferTexture->GetSampleCount());
		this->renderTexture->Load(nullptr, width, height, this->renderTexture->GetFormat(), this->renderTexture->GetWrapType());
	}

    void CameraController::SubmitMatrixProjectionChanges() const
    {
		if (this->GetCameraType() == CameraType::PERSPECTIVE)
		{
			auto* cam = reinterpret_cast<PerspectiveCamera*>(&this->Camera); //-V717
			auto fov = cam->GetFOV();
			cam->SetFOV(fov);
		}
		else if (this->GetCameraType() == CameraType::ORTHOGRAPHIC)
		{
			auto* cam = reinterpret_cast<OrthographicCamera*>(&this->Camera); //-V717
			auto size = cam->GetSize();
			cam->SetSize(size);
		}
    }

	const Vector3& CameraController::GetDirection() const
	{
		return this->direction;
	}

	void CameraController::SetDirection(const Vector3& direction)
	{
		this->direction = direction + MakeVector3(0.0f, 0.0f, 0.00001f);
	}

	float CameraController::GetHorizontalAngle() const
	{
		return this->horizontalAngle;
	}

	float CameraController::GetVerticalAngle() const
	{
		return this->verticalAngle;
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
}