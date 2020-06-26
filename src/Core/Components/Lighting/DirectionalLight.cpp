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

#include "DirectionalLight.h"
#include "Core/Application/RenderManager.h"
#include "Core/Application/EventManager.h"
#include "Core/Event/Events/UpdateEvent.h"

namespace MxEngine
{
    DirectionalLight::DirectionalLight()
    { 
        auto texture = GraphicFactory::Create<Texture>();
        texture->LoadDepth(4096, 4096);
        this->AttachDepthTexture(texture);
    }

    DirectionalLight::~DirectionalLight()
    {
        MxString eventName = MxObject::GetByComponent(*this).GetComponent<DirectionalLight>().GetUUID();
        EventManager::FlushEvents(); // avoid problem with deleting event while its still in queue
        EventManager::RemoveEventListener(eventName);
    }

    GResource<Texture> DirectionalLight::GetDepthTexture() const
    {
        return this->texture;
    }

    void DirectionalLight::AttachDepthTexture(const GResource<Texture>& texture)
    {
        this->texture = texture;
    }

    Matrix4x4 DirectionalLight::GetMatrix(const Vector3& center) const
    {
        Vector3 Low  = MakeVector3(-this->ProjectionSize);
        Vector3 High = MakeVector3( this->ProjectionSize);
        
        Matrix4x4 OrthoProjection = MakeOrthographicMatrix(Low.x, High.x, Low.y, High.y, Low.z, High.z);
        Matrix4x4 LightView = MakeViewMatrix(
            center + this->Direction,
            center + MakeVector3(0.0f, 0.0f, 0.00001f),
            MakeVector3(0.0f, 1.0f, 0.00001f)
        );
        return OrthoProjection * LightView;
    }

    void DirectionalLight::FollowViewport()
    {
        auto& object = MxObject::GetByComponent(*this);
        MxString uuid = object.GetComponent<DirectionalLight>().GetUUID();

        EventManager::RemoveEventListener(uuid);

        EventManager::AddEventListener(uuid, [tr = object.Transform](UpdateEvent& e) mutable
        {
            auto viewport = RenderManager::GetViewport();
            if (viewport.IsValid()) 
                tr->SetPosition(MxObject::GetByComponent(*viewport).Transform->GetPosition());
        });
    }
}