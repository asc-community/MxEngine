// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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
#include "Core/Interfaces/IMovable.h"
#include "Core/Event/Events/KeyEvent.h"
#include "Core/Event/Events/MouseEvent.h"

namespace MxEngine
{
	class InputControlBinding
	{
		std::string handle;
		IMovable& object;
	public:
		inline InputControlBinding(const std::string& eventHandle, IMovable& object) noexcept
			: handle(eventHandle), object(object) { }

		inline InputControlBinding& Unbind()
		{
			auto& dispatcher = Application::Get()->GetCurrentScene().GetEventDispatcher();
			dispatcher.RemoveEventListener(this->handle);
			return *this;
		}

		inline InputControlBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right)
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object movement");
			auto& dispatcher = Application::Get()->GetCurrentScene().GetEventDispatcher();
			dispatcher.AddEventListener(this->handle, 
				[forward, back, right, left, &object = object](KeyEvent& event)
				{
					auto dt = Application::Get()->GetTimeDelta();
					if (forward != KeyCode::UNKNOWN && event.IsHeld(forward))
					{
						object.TranslateForward(dt);
					}
					if (back != KeyCode::UNKNOWN && event.IsHeld(back))
					{
						object.TranslateForward(-dt);
					}
					if (right != KeyCode::UNKNOWN && event.IsHeld(right))
					{
						object.TranslateRight(dt);
					}
					if (left != KeyCode::UNKNOWN && event.IsHeld(left))
					{
						object.TranslateRight(-dt);
					}
				});
			return *this;
		}

		inline InputControlBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down)
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object movement");
			auto& dispatcher = Application::Get()->GetCurrentScene().GetEventDispatcher();
			dispatcher.AddEventListener(this->handle,
				[forward, back, right, left, up, down, &object = object](KeyEvent& event)
				{
					auto dt = Application::Get()->GetTimeDelta();
					if (forward != KeyCode::UNKNOWN && event.IsHeld(forward))
					{
						object.TranslateForward(dt);
					}
					if (back != KeyCode::UNKNOWN && event.IsHeld(back))
					{
						object.TranslateForward(-dt);
					}
					if (right != KeyCode::UNKNOWN && event.IsHeld(right))
					{
						object.TranslateRight(dt);
					}
					if (left != KeyCode::UNKNOWN && event.IsHeld(left))
					{
						object.TranslateRight(-dt);
					}
					if (up != KeyCode::UNKNOWN && event.IsHeld(up))
					{
						object.TranslateUp(dt);
					}
					if (down != KeyCode::UNKNOWN && event.IsHeld(down))
					{
						object.TranslateUp(-dt);
					}
				});
			return *this;
		}

		inline InputControlBinding& BindRotation()
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object rotation");
			auto& dispatcher = Application::Get()->GetCurrentScene().GetEventDispatcher();
			dispatcher.AddEventListener(this->handle,
				[&object = object](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto dt = Application::Get()->GetTimeDelta();
					object.Rotate(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}

		inline InputControlBinding& BindHorizontalRotation()
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object rotation");
			auto& dispatcher = Application::Get()->GetCurrentScene().GetEventDispatcher();
			dispatcher.AddEventListener(this->handle,
				[&object = object](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto dt = Application::Get()->GetTimeDelta();
					object.Rotate(dt * (oldPos.x - event.position.x), 0.0f);
					oldPos = event.position;
				});
			return *this;
		}

		inline InputControlBinding& BindVerticalRotation()
		{
			Logger::Instance().Debug("MxEngine::InputControlBinding", "bound object rotation");
			auto& dispatcher = Application::Get()->GetCurrentScene().GetEventDispatcher();
			dispatcher.AddEventListener(this->handle,
				[&object = object](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto dt = Application::Get()->GetTimeDelta();
					object.Rotate(0.0f, dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}
	};
}