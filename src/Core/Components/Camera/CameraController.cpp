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
#include "Core/Events/WindowResizeEvent.h"
#include "Core/Config/GlobalConfig.h"
#include "Core/Application/Event.h"
#include "Platform/Window/WindowManager.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "FrustrumCamera.h"

namespace MxEngine
{
	template<>
	PerspectiveCamera& CameraController::GetCamera<PerspectiveCamera>()
	{
		MX_ASSERT(this->GetCameraType() == CameraType::PERSPECTIVE);
		static_assert(AssertEquality<sizeof(PerspectiveCamera), sizeof(this->Camera)>::value, "camera byte storage size mismatch");
		return *reinterpret_cast<PerspectiveCamera*>(&this->Camera); //-V717
	}

	template<>
	const PerspectiveCamera& CameraController::GetCamera<PerspectiveCamera>() const
	{
		MX_ASSERT(this->GetCameraType() == CameraType::PERSPECTIVE);
		static_assert(AssertEquality<sizeof(PerspectiveCamera), sizeof(this->Camera)>::value, "camera byte storage size mismatch");
		return *reinterpret_cast<const PerspectiveCamera*>(&this->Camera); //-V717
	}

	template<>
	FrustrumCamera& CameraController::GetCamera<FrustrumCamera>()
	{
		MX_ASSERT(this->GetCameraType() == CameraType::FRUSTRUM);
		static_assert(AssertEquality<sizeof(FrustrumCamera), sizeof(this->Camera)>::value, "camera byte storage size mismatch");
		return *reinterpret_cast<FrustrumCamera*>(&this->Camera); //-V717
	}

	template<>
	const FrustrumCamera& CameraController::GetCamera<FrustrumCamera>() const
	{
		MX_ASSERT(this->GetCameraType() == CameraType::FRUSTRUM);
		static_assert(AssertEquality<sizeof(FrustrumCamera), sizeof(this->Camera)>::value, "camera byte storage size mismatch");
		return *reinterpret_cast<const FrustrumCamera*>(&this->Camera); //-V717
	}

	template<>
	OrthographicCamera& CameraController::GetCamera<OrthographicCamera>()
	{
		MX_ASSERT(this->GetCameraType() == CameraType::ORTHOGRAPHIC);
		static_assert(AssertEquality<sizeof(OrthographicCamera), sizeof(this->Camera)>::value, "camera byte storage size mismatch");
		return *reinterpret_cast<OrthographicCamera*>(&this->Camera); //-V717
	}

	template<>
	const OrthographicCamera& CameraController::GetCamera<OrthographicCamera>() const
	{
		MX_ASSERT(this->GetCameraType() == CameraType::ORTHOGRAPHIC);
		static_assert(AssertEquality<sizeof(OrthographicCamera), sizeof(this->Camera)>::value, "camera byte storage size mismatch");
		return *reinterpret_cast<const OrthographicCamera*>(&this->Camera); //-V717
	}

	CameraController::CameraController()
	{
		this->SetCameraType(CameraType::PERSPECTIVE);

		auto viewport = (VectorInt2)WindowManager::GetSize();
		this->renderBuffers->Init(viewport.x, viewport.y);

		this->renderTexture = GraphicFactory::Create<Texture>();
		this->renderTexture->Load(nullptr, viewport.x, viewport.y, 3, TextureFormat::RGB, TextureWrap::CLAMP_TO_EDGE);
		this->renderTexture->SetPath("[[camera output]]");
	}

	CameraController::~CameraController()
	{
		Event::RemoveEventListener(this->GetEventUUID());
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

		(void)this->GetViewMatrix(position);
		return this->Camera.GetMatrix();
	}

	const Matrix4x4& CameraController::GetViewMatrix(const Vector3& position) const
	{
		auto view = MakeViewMatrix(position, position + this->GetDirection(), this->GetUpVector());
		this->Camera.SetViewMatrix(view);
		return this->Camera.GetViewMatrix();
	}

	const Matrix4x4& CameraController::GetProjectionMatrix() const
	{
		if (this->Camera.UpdateProjection) this->SubmitMatrixProjectionChanges();
		return this->Camera.GetProjectionMatrix();
	}

    Matrix4x4 CameraController::GetStaticMatrix() const
    {
		if (this->Camera.UpdateProjection) this->SubmitMatrixProjectionChanges();

		auto ViewMatrix = this->GetStaticViewMatrix();
		const Matrix4x4& ProjectionMatrix = this->Camera.GetProjectionMatrix();
		return ProjectionMatrix * ViewMatrix;
    }

	Matrix4x4 CameraController::GetStaticViewMatrix() const
	{
		auto view = MakeViewMatrix(MakeVector3(0.0f), this->GetDirection(), this->GetUpVector());
		this->Camera.SetViewMatrix(view);
		auto viewMatrix = (Matrix3x3)this->Camera.GetViewMatrix();
		return (Matrix4x4)viewMatrix;
	}

	TextureHandle CameraController::GetRenderTexture() const
	{
		return this->renderTexture;
	}

	void CameraController::ListenWindowResizeEvent()
	{
		Event::RemoveEventListener(this->GetEventUUID());

		Event::AddEventListener<WindowResizeEvent>(this->GetEventUUID(),
		[camera = MxObject::GetComponentHandle(*this)](WindowResizeEvent& e) mutable
		{
			if (camera.IsValid() && (e.Old != e.New))
			{
				camera->ResizeRenderTexture((size_t)e.New.x, (size_t)e.New.y);
			}
		});
	}

	void CameraController::ResizeRenderTexture(size_t w, size_t h)
	{
		this->renderTexture->Load(nullptr, (int)w, (int)h, 3, this->renderTexture->GetFormat(), this->renderTexture->GetWrapType());
		this->renderTexture->SetPath("[[camera output]]");
		if(this->IsRendered())
			this->renderBuffers->Resize((int)w, (int)h);
	}

	void CameraController::SetRenderTexture(const TextureHandle& texture)
	{
		MX_ASSERT(texture.IsValid());
		this->renderTexture = texture;
		if (this->IsRendered())
		{
			this->renderBuffers->Resize((int)texture->GetWidth(), (int)texture->GetHeight());
		}
	}

    bool CameraController::IsRendered() const
    {
		return this->renderingEnabled;
    }

    void CameraController::ToggleRendering(bool value)
	{
		if (this->renderingEnabled != value)
		{
			if (value)
				this->renderBuffers->Init((int)this->renderTexture->GetWidth(), (int)this->renderTexture->GetHeight());
			else
				this->renderBuffers->DeInit();
		}
		this->renderingEnabled = value;
	}

    const FrustrumCuller& CameraController::GetFrustrumCuller() const
    {
		return this->Camera.GetFrustrumCuller();
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
		else if (this->GetCameraType() == CameraType::FRUSTRUM)
		{
			auto* cam = reinterpret_cast<FrustrumCamera*>(&this->Camera); //-V717
			auto zoom = cam->GetZoom();
			auto center = cam->GetProjectionCenter();
			cam->SetBounds(center.x, center.y, zoom);
		}
    }

	void CameraController::RecalculateRotationAngles()
	{
		auto normDir = this->GetDirection();
		float verticalAngle = std::asin(normDir.y);
		float horizontalAngle = std::acos(Dot(normDir, MakeVector3(0.0f, 0.0f, 1.0f)));

		auto x = std::sin(horizontalAngle);
		auto z = std::cos(horizontalAngle);

		bool xCorrect = std::signbit(normDir.x) == std::signbit(x);
		bool zCorrect = std::signbit(normDir.z) == std::signbit(z);
		if(!xCorrect || !zCorrect) horizontalAngle = TwoPi<float>() - horizontalAngle;

		this->horizontalAngle = horizontalAngle;
		this->verticalAngle = verticalAngle;
	}

	const UUID& CameraController::GetEventUUID() const
	{
		return this->GetGBuffer().GetUUID();
	}

	const Vector3& CameraController::GetDirectionDenormalized() const
	{
		return this->direction;
	}

    Vector3 CameraController::GetDirection() const
    {
		return Normalize(this->direction);
    }

	void CameraController::SetDirection(const Vector3& direction)
	{
		this->direction = direction + MakeVector3(0.0f, 0.0f, 0.00001f);
		this->RecalculateRotationAngles();
	}

	Vector3 CameraController::GetDirectionUp() const
	{
		return -Cross(this->GetDirection(), this->right);
	}

	float CameraController::GetHorizontalAngle() const
	{
		return this->horizontalAngle;
	}

	float CameraController::GetVerticalAngle() const
	{
		return this->verticalAngle;
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
		horizontal = Radians(horizontal);
		vertical   = Radians(vertical);
		this->horizontalAngle += this->rotateSpeed * horizontal;
		this->verticalAngle   += this->rotateSpeed * vertical;

		this->verticalAngle = Clamp(this->verticalAngle, 
			-HalfPi<float>() + 0.001f, HalfPi<float>() - 0.001f);
		
		this->horizontalAngle = this->horizontalAngle - TwoPi<float>() * std::floor(this->horizontalAngle / TwoPi<float>());

		this->direction = MakeVector3(
			std::cos(verticalAngle) * std::sin(horizontalAngle),
			std::sin(verticalAngle),
			std::cos(verticalAngle) * std::cos(horizontalAngle)
		);

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

	FrameBufferHandle CameraController::GetGBuffer() const
	{
		return this->renderBuffers->GBuffer;
	}

	TextureHandle CameraController::GetAlbedoTexture() const
	{
		return this->renderBuffers->Albedo;
	}

	TextureHandle CameraController::GetNormalTexture() const
	{
		return this->renderBuffers->Normal;
	}

	TextureHandle CameraController::GetMaterialTexture() const
	{
		return this->renderBuffers->Material;
	}

	TextureHandle CameraController::GetDepthTexture() const
	{
		return this->renderBuffers->Depth;
	}

	TextureHandle CameraController::GetAverageWhiteTexture() const
	{
		return this->renderBuffers->AverageWhite;
	}

	TextureHandle CameraController::GetHDRTexture() const
	{
		return this->renderBuffers->HDR;
	}

    TextureHandle CameraController::GetSwapHDRTexture() const
    {
		return this->renderBuffers->SwapHDR;
    }

	void CameraRender::Init(int width, int height)
	{
		this->GBuffer = GraphicFactory::Create<FrameBuffer>();
		this->Albedo = GraphicFactory::Create<Texture>();
		this->Normal = GraphicFactory::Create<Texture>();
		this->Material = GraphicFactory::Create<Texture>();
		this->Depth = GraphicFactory::Create<Texture>();
		this->AverageWhite = GraphicFactory::Create<Texture>();
		this->HDR = GraphicFactory::Create<Texture>();
		this->SwapHDR = GraphicFactory::Create<Texture>();

		this->Resize(width, height);
		
		this->GBuffer->AttachTexture(this->Albedo, Attachment::COLOR_ATTACHMENT0);
		this->GBuffer->AttachTextureExtra(this->Normal, Attachment::COLOR_ATTACHMENT1);
		this->GBuffer->AttachTextureExtra(this->Material, Attachment::COLOR_ATTACHMENT2);
		this->GBuffer->AttachTextureExtra(this->Depth, Attachment::DEPTH_ATTACHMENT);

		std::array attachments = {
			Attachment::COLOR_ATTACHMENT0,
			Attachment::COLOR_ATTACHMENT1,
			Attachment::COLOR_ATTACHMENT2,
		};
		this->GBuffer->UseDrawBuffers(attachments);
		this->GBuffer->Validate();
	}

	void CameraRender::Resize(int width, int height)
	{
		this->Albedo->Load(nullptr, width, height, 3, TextureFormat::RGBA, TextureWrap::CLAMP_TO_EDGE);
		this->Normal->Load(nullptr, width, height, 3, TextureFormat::RGBA16, TextureWrap::CLAMP_TO_EDGE);
		this->Material->Load(nullptr, width, height, 3, TextureFormat::RGBA, TextureWrap::CLAMP_TO_EDGE);
		this->Depth->LoadDepth(width, height, TextureFormat::DEPTH32F, TextureWrap::CLAMP_TO_EDGE);
		this->AverageWhite->Load(nullptr, 1, 1, 3, TextureFormat::RGBA16F, TextureWrap::CLAMP_TO_EDGE);
		this->HDR->Load(nullptr, width, height, 3, TextureFormat::RGBA16F, TextureWrap::CLAMP_TO_EDGE);
		this->SwapHDR->Load(nullptr, width, height, 3, TextureFormat::RGBA16F, TextureWrap::CLAMP_TO_EDGE);

		this->Albedo->SetPath("[[cam albedo]]");
		this->Normal->SetPath("[[cam normal]]");
		this->Material->SetPath("[[cam material]]");
		this->Depth->SetPath("[[cam depth]]");
		this->AverageWhite->SetPath("[[cam avg white]]");
		this->HDR->SetPath("[[cam hdr]]");
		this->SwapHDR->SetPath("[[cam swap hdr]]");
	}

	void CameraRender::DeInit()
	{
		GraphicFactory::Destroy(this->GBuffer);
		GraphicFactory::Destroy(this->Albedo);
		GraphicFactory::Destroy(this->Normal);
		GraphicFactory::Destroy(this->Material);
		GraphicFactory::Destroy(this->Depth);
		GraphicFactory::Destroy(this->HDR);
		GraphicFactory::Destroy(this->SwapHDR);
	}
}