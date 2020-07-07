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

#include "InputControl.h"
#include "Core/Application/EventManager.h"
#include "Core/Event/Events/MouseEvent.h"

namespace MxEngine
{
	InputControl::~InputControl()
	{	
		MxString uuid = MxObject::GetComponentUUID(*this);
		EventManager::RemoveEventListener(uuid);
	}

	void InputControl::BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right)
	{
		this->BindMovement(forward, left, back, right, KeyCode::UNKNOWN, KeyCode::UNKNOWN);
	}
	
	void InputControl::BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down)
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		MxString uuid = object.GetComponent<InputControl>().GetUUID();

		MXLOG_DEBUG("MxEngine::InputControl", "bound object movement: " + object.Name);
	
		EventManager::AddEventListener(uuid,
			[forward, back, right, left, up, down, camera, object = MxObject::GetHandleByComponent(*this)](KeyEvent& event) mutable
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
				vecForward = object->Transform.GetRotation() * vecForward;
				vecRight   = object->Transform.GetRotation() * vecRight;
				vecUp      = object->Transform.GetRotation() * vecUp;
			}

			auto dt = Application::Get()->GetTimeDelta();
			if (event.IsHeld(forward))
			{
				moveDirection += vecForward;
			}
			if (event.IsHeld(back))
			{
				moveDirection -= vecForward;
			}
			if (event.IsHeld(right))
			{
				moveDirection += vecRight;
			}
			if (event.IsHeld(left))
			{
				moveDirection -= vecRight;
			}
			if (event.IsHeld(up))
			{
				moveDirection += vecUp;
			}
			if (event.IsHeld(down))
			{
				moveDirection -= vecUp;
			}
			if(moveDirection != MakeVector3(0.0f))
				object->Transform.Translate(Normalize(moveDirection) * moveSpeed * dt);
		});
	}
	
	void InputControl::BindRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			MXLOG_WARNING("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		MXLOG_DEBUG("MxEngine::InputControl", "bound object rotation: " + object.Name);
		MxString uuid = object.GetComponent<InputControl>().GetUUID(); 

		EventManager::AddEventListener(uuid, [camera](MouseMoveEvent& event) mutable
		{
			if (!camera.IsValid()) return;
			static Vector2 oldPos = event.position;
			auto dt = Application::Get()->GetTimeDelta();
			Vector2 diff(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
			camera->Rotate(diff.x, diff.y);
			oldPos = event.position;
		});
	}
	
	void InputControl::BindHorizontalRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			MXLOG_WARNING("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		MXLOG_DEBUG("MxEngine::InputControl", "bound object rotation: " + object.Name);
		MxString uuid = object.GetComponent<InputControl>().GetUUID();
	
		EventManager::AddEventListener(uuid, [camera](MouseMoveEvent& event) mutable
		{
				if (!camera.IsValid()) return;
				static Vector2 oldPos = event.position;
				auto dt = Application::Get()->GetTimeDelta();
				Vector2 diff(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
				camera->Rotate(diff.x, 0.0f);
				oldPos = event.position;
		});
	}
	
	void InputControl::BindVerticalRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			MXLOG_WARNING("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		MXLOG_DEBUG("MxEngine::InputControl", "bound object rotation: " + object.Name);
		MxString uuid = object.GetComponent<InputControl>().GetUUID();
	
		EventManager::AddEventListener(uuid, [camera](MouseMoveEvent& event) mutable
		{
				if (!camera.IsValid()) return;
				static Vector2 oldPos = event.position;
				auto dt = Application::Get()->GetTimeDelta();
				Vector2 diff(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
				camera->Rotate(0.0f, diff.y);
				oldPos = event.position;
		});
	}
}