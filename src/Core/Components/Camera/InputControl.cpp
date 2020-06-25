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
		MxString uuid = MxObject::GetByComponent(*this).GetComponent<InputControl>().GetUUID();
		EventManager::FlushEvents();
		EventManager::RemoveEventListener(uuid);
	}

	void InputControl::BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right)
	{
		this->BindMovement(forward, left, back, right, KeyCode::UNKNOWN, KeyCode::UNKNOWN);
	}
	
	void InputControl::BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down)
	{
		Logger::Instance().Debug("MxEngine::InputControl", "bound object movement");
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		MxString uuid = object.GetComponent<InputControl>().GetUUID();
	
		EventManager::AddEventListener(uuid,
			[forward, back, right, left, up, down, camera, transform = object.Transform](KeyEvent& event) mutable
		{
			auto vecForward = MakeVector3( 0.0f, 0.0f, 1.0f);
			auto vecRight   = MakeVector3(-1.0f, 0.0f, 0.0f);
			auto vecUp      = MakeVector3( 0.0f, 1.0f, 0.0f);
			float moveSpeed = 1.0f;
			if (camera.IsValid())
			{
				vecForward = camera->GetForwardVector();
				vecRight   = camera->GetRightVector();
				vecUp      = camera->GetUpVector();
				moveSpeed  = camera->GetMoveSpeed();
			}
			else
			{
				vecForward = transform->GetRotation() * vecForward;
				vecRight   = transform->GetRotation() * vecRight;
				vecUp      = transform->GetRotation() * vecUp;
			}

			auto dt = Application::Get()->GetTimeDelta();
			if (event.IsHeld(forward))
			{
				transform->Translate(vecForward * moveSpeed * dt);
			}
			if (event.IsHeld(back))
			{
				transform->Translate(-1.0f * vecForward * moveSpeed * dt);
			}
			if (event.IsHeld(right))
			{
				transform->Translate(vecRight * moveSpeed * dt);
			}
			if (event.IsHeld(left))
			{
				transform->Translate(-1.0f * vecRight * moveSpeed * dt);
			}
			if (event.IsHeld(up))
			{
				transform->Translate(vecUp * moveSpeed * dt);
			}
			if (event.IsHeld(down))
			{
				transform->Translate(-1.0f * vecUp * moveSpeed * dt);
			}
		});
	}
	
	void InputControl::BindRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			Logger::Instance().Warning("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		Logger::Instance().Debug("MxEngine::InputControl", "bound object rotation");
		MxString uuid = object.GetComponent<InputControl>().GetUUID(); 

		EventManager::AddEventListener(uuid, [camera](MouseMoveEvent& event) mutable
		{
			if (!camera.IsValid()) return;
			static Vector2 oldPos = event.position;
			auto dt = Application::Get()->GetTimeDelta();
			camera->Rotate(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
			oldPos = event.position;
		});
	}
	
	void InputControl::BindHorizontalRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			Logger::Instance().Warning("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		Logger::Instance().Debug("MxEngine::InputControl", "bound object rotation");
		MxString uuid = object.GetComponent<InputControl>().GetUUID();
	
		EventManager::AddEventListener(uuid, [camera](MouseMoveEvent& event) mutable
		{
			if (!camera.IsValid()) return;
			static Vector2 oldPos = event.position;
			auto dt = Application::Get()->GetTimeDelta();
			camera->Rotate(dt * (oldPos.x - event.position.x), 0.0f);
			oldPos = event.position;
		});
	}
	
	void InputControl::BindVerticalRotation()
	{
		auto& object = MxObject::GetByComponent(*this);
		auto camera = object.GetComponent<CameraController>();
		if (!camera.IsValid())
		{
			Logger::Instance().Warning("MxEngine::InputControl", "rotation can be bound only to CameraController component");
			return;
		}

		Logger::Instance().Debug("MxEngine::InputControl", "bound object rotation");
		MxString uuid = object.GetComponent<InputControl>().GetUUID();
	
		EventManager::AddEventListener(uuid, [camera](MouseMoveEvent& event) mutable
		{
			if (!camera.IsValid()) return;
			static Vector2 oldPos = event.position;
			auto dt = Application::Get()->GetTimeDelta();
			camera->Rotate(0.0f, dt * (oldPos.y - event.position.y));
			oldPos = event.position;
		});
	}
}