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

#pragma once

#include "Core/Application/Application.h"
#include "Core/Event/Events/KeyEvent.h"
#include "Core/Event/Events/MouseEvent.h"

namespace MxEngine
{
	class InputControlBinding
	{
		CResource<CameraController> camera;
	public:
		InputControlBinding(const CResource<CameraController>& camera) noexcept
			: camera(camera) { }

		InputControlBinding& Unbind()
		{
			auto& dispatcher = Application::Get()->GetEventDispatcher();
			dispatcher.RemoveEventListener(MxFormat("Input_{0}", (MxString)camera.GetUUID()));
			return *this;
		}

		InputControlBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right)
		{
			this->BindMovement(forward, left, back, right, KeyCode::UNKNOWN, KeyCode::UNKNOWN);
			return *this;
		}

		InputControlBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down)
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object movement");
			auto& dispatcher = Application::Get()->GetEventDispatcher();

			auto& object = MxObject::GetByComponent(*this->camera);
			auto transform = object.GetComponent<Transform>();

			dispatcher.AddEventListener(MxFormat("Input_{0}", (MxString)camera.GetUUID()),
				[forward, back, right, left, up, down, camera = this->camera, transform = std::move(transform)](KeyEvent& event) mutable
				{
					if (!camera.IsValid()) { InputControlBinding(camera).Unbind(); return; }

					auto dt = Application::Get()->GetTimeDelta();
					if (event.IsHeld(forward))
					{
						transform->Translate(camera->GetForwardVector() * camera->GetMoveSpeed() * dt);
					}
					if (event.IsHeld(back))
					{
						transform->Translate(-1.0f * camera->GetForwardVector() * camera->GetMoveSpeed() * dt);
					}
					if (event.IsHeld(right))
					{
						transform->Translate(camera->GetRightVector() * camera->GetMoveSpeed() * dt);
					}
					if (event.IsHeld(left))
					{
						transform->Translate(-1.0f * camera->GetRightVector() * camera->GetMoveSpeed() * dt);
					}
					if (event.IsHeld(up))
					{
						transform->Translate(camera->GetUpVector() * camera->GetMoveSpeed() * dt);
					}
					if (event.IsHeld(down))
					{
						transform->Translate(-1.0f * camera->GetUpVector() * camera->GetMoveSpeed() * dt);
					}
				});
			return *this;
		}

		InputControlBinding& BindRotation()
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object rotation");
			auto& dispatcher = Application::Get()->GetEventDispatcher();
			dispatcher.AddEventListener(MxFormat("Input_{0}", (MxString)camera.GetUUID()),
				[camera = this->camera](MouseMoveEvent& event) mutable
				{
					if (!camera.IsValid()) { InputControlBinding(camera).Unbind(); return; }

					static Vector2 oldPos = event.position;
					auto dt = Application::Get()->GetTimeDelta();
					camera->Rotate(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}

		InputControlBinding& BindHorizontalRotation()
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object rotation");
			auto& dispatcher = Application::Get()->GetEventDispatcher();
			dispatcher.AddEventListener(MxFormat("Input_{0}", (MxString)camera.GetUUID()),
				[camera = this->camera](MouseMoveEvent& event) mutable
				{
					if (!camera.IsValid()) { InputControlBinding(camera).Unbind(); return; }

					static Vector2 oldPos = event.position;
					auto dt = Application::Get()->GetTimeDelta();
					camera->Rotate(dt * (oldPos.x - event.position.x), 0.0f);
					oldPos = event.position;
				});
			return *this;
		}

		InputControlBinding& BindVerticalRotation()
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object rotation");
			auto& dispatcher = Application::Get()->GetEventDispatcher();
			dispatcher.AddEventListener(MxFormat("Input_{0}", (MxString)camera.GetUUID()),
				[camera = this->camera](MouseMoveEvent& event) mutable
				{
					if (!camera.IsValid()) { InputControlBinding(camera).Unbind(); return; }

					static Vector2 oldPos = event.position;
					auto dt = Application::Get()->GetTimeDelta();
					camera->Rotate(0.0f, dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}
	};
}