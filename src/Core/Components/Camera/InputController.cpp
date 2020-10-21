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

#include "InputController.h"
#include "Core/Application/Event.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/UpdateEvent.h"
#include "Platform/Window/Input.h"
#include "Utilities/Logging/Logger.h"

namespace MxEngine
{
	InputController::~InputController()
	{	
		MxString uuid = MxObject::GetComponentUUID(*this);
		Event::RemoveEventListener(uuid);
	}

	void InputController::BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right)
	{
		this->BindMovement(forward, left, back, right, KeyCode::UNKNOWN, KeyCode::UNKNOWN);
	}
	
	void InputController::BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down)
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		auto input = object.GetComponent<InputController>();

		MXLOG_DEBUG("MxEngine::InputControl", "bound object movement: " + object.Name);
	
		Event::AddEventListener<UpdateEvent>(input.GetUUID(),
			[forward, back, right, left, up, down, camera, input, object = MxObject::GetHandleByComponent(*this)](auto& event) mutable
		{
			auto vecForward = MakeVector3( 0.0f, 0.0f, 1.0f);
			auto vecRight   = MakeVector3(-1.0f, 0.0f, 0.0f);
			auto vecUp      = MakeVector3( 0.0f, 1.0f, 0.0f);
			float moveSpeed = 1.0f;
			auto moveDirection = MakeVector3(0.0f);
			if (camera.IsValid())
			{
				vecForward = camera->GetForwardVector();
				vecRight   = camera->GetRightVector();
				vecUp      = camera->GetUpVector();
				moveSpeed  = camera->GetMoveSpeed();
			}
			else
			{
				vecForward = object->Transform.GetRotation() * vecForward; //-V807
				vecRight   = object->Transform.GetRotation() * vecRight;
				vecUp      = object->Transform.GetRotation() * vecUp;
			}

			auto dt = Application::Get()->GetUnscaledTimeDelta();
			if (Input::IsKeyHeld(forward))
			{
				moveDirection += vecForward;
			}
			if (Input::IsKeyHeld(back))
			{
				moveDirection -= vecForward;
			}
			if (Input::IsKeyHeld(right))
			{
				moveDirection += vecRight;
			}
			if (Input::IsKeyHeld(left))
			{
				moveDirection -= vecRight;
			}
			if (Input::IsKeyHeld(up))
			{
				moveDirection += vecUp;
			}
			if (Input::IsKeyHeld(down))
			{
				moveDirection -= vecUp;
			}

			if (moveDirection != MakeVector3(0.0f))
			{
				object->Transform.Translate(Normalize(moveDirection) * moveSpeed * dt);
				input->motion = Normalize(moveDirection);
			}
			else
			{
				input->motion = MakeVector3(0.0f);
			}
		});
	}
	
	void InputController::BindRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			MXLOG_WARNING("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		MXLOG_DEBUG("MxEngine::InputControl", "bound object rotation: " + object.Name);
		MxString uuid = object.GetComponent<InputController>().GetUUID();

		Event::AddEventListener<MouseMoveEvent>(uuid, [camera](auto& event) mutable
		{
			if (!camera.IsValid()) return;
			static Vector2 oldPos = event.position;
			auto dt = Application::Get()->GetUnscaledTimeDelta();
			Vector2 diff(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
			camera->Rotate(diff.x, diff.y);
			oldPos = event.position;
		});
	}
	
	void InputController::BindHorizontalRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			MXLOG_WARNING("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		MXLOG_DEBUG("MxEngine::InputControl", "bound object rotation: " + object.Name);
		MxString uuid = object.GetComponent<InputController>().GetUUID();
	
		Event::AddEventListener<MouseMoveEvent>(uuid, [camera](auto& event) mutable
		{
				if (!camera.IsValid()) return;
				static Vector2 oldPos = event.position;
				auto dt = Application::Get()->GetUnscaledTimeDelta();
				Vector2 diff(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
				camera->Rotate(diff.x, 0.0f);
				oldPos = event.position;
		});
	}
	
	void InputController::BindVerticalRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			MXLOG_WARNING("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		MXLOG_DEBUG("MxEngine::InputControl", "bound object rotation: " + object.Name);
		MxString uuid = object.GetComponent<InputController>().GetUUID();
	
		Event::AddEventListener<MouseMoveEvent>(uuid, [camera](auto& event) mutable
		{
				if (!camera.IsValid()) return;
				static Vector2 oldPos = event.position;
				auto dt = Application::Get()->GetUnscaledTimeDelta();
				Vector2 diff(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
				camera->Rotate(0.0f, diff.y);
				oldPos = event.position;
		});
	}

	const Vector3& InputController::GetMotionVector() const
	{
		return this->motion;
	}
}