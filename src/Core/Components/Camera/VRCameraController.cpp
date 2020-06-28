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

#include "VRCameraController.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Application/RenderManager.h"
#include "Core/Application/EventManager.h"
#include "Core/Event/Events/UpdateEvent.h"

namespace MxEngine
{
    void VRCameraController::OnUpdate()
    {
        auto camera = MxObject::GetByComponent(*this).GetComponent<CameraController>();

        if (!this->LeftEye.IsValid() || !this->RightEye.IsValid() || !camera.IsValid())
            return;

        this->UpdateEyes(this->LeftEye, this->RightEye);

        camera->ToggleRendering(false);

        auto leftTexture  = this->LeftEye->GetRenderTexture();
        auto rightTexture = this->RightEye->GetRenderTexture();
        auto resultTexture = camera->GetRenderTexture();

        size_t widthTotal = leftTexture->GetWidth() + rightTexture->GetWidth();
        size_t heightTotal = leftTexture->GetHeight() + rightTexture->GetHeight();

        if (resultTexture->GetWidth() != widthTotal || resultTexture->GetHeight() != heightTotal)
            resultTexture->Load(nullptr, (int)widthTotal, (int)heightTotal, resultTexture->GetFormat(), resultTexture->GetWrapType());

        this->Render(resultTexture, leftTexture, rightTexture);
    }

    void VRCameraController::UpdateEyes(CameraController::Handle& cameraL, CameraController::Handle& cameraR)
    {
        auto& object = MxObject::GetByComponent(*this);
        auto position = object.Transform->GetPosition();
        auto camera = object.GetComponent<CameraController>();
        
        auto& LEyeTransform = MxObject::GetByComponent(*cameraL).Transform;
        auto& REyeTransform = MxObject::GetByComponent(*cameraR).Transform;

        auto LEyeDistance = -this->EyeDistance * camera->GetRightVector();
        auto REyeDistance = +this->EyeDistance * camera->GetRightVector();

        LEyeTransform->SetPosition(position + LEyeDistance);
        REyeTransform->SetPosition(position + REyeDistance);

        auto LEyeDirection = this->FocusDistance * camera->GetDirection() - LEyeDistance;
        auto REyeDirection = this->FocusDistance * camera->GetDirection() - REyeDistance;
        cameraL->SetDirection(Normalize(LEyeDirection));
        cameraR->SetDirection(Normalize(REyeDirection));
    }

    void VRCameraController::Render(GResource<Texture>& target, const GResource<Texture>& leftEye, const GResource<Texture>& rightEye)
    {
        leftEye->Bind(0);
        rightEye->Bind(1);
        this->shaderVR->SetUniformInt("leftEyeTex", 0);
        this->shaderVR->SetUniformInt("rightEyeTex", 1);
        RenderManager::GetController().RenderToTexture(target, *this->shaderVR);
        target->GenerateMipmaps();
    }

    VRCameraController::~VRCameraController()
    {
        EventManager::RemoveEventListener(this->shaderVR.GetUUID());
    }

    void VRCameraController::Init()
    {
        this->shaderVR = GraphicFactory::Create<Shader>();
        this->shaderVR->LoadFromString(
            #include MAKE_PLATFORM_SHADER(rect_vertex)
            ,
            #include MAKE_PLATFORM_SHADER(vr_fragment)
        );

        auto self = MxObject::GetComponentHandle(*this);

        EventManager::AddEventListener(this->shaderVR.GetUUID(), 
            [self](UpdateEvent&) mutable { self->OnUpdate(); });
    }
}