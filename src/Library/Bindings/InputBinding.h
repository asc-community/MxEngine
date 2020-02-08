#pragma once

#include "Core/Application/Application.h"
#include "Core/Interfaces/IMovable.h"
#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"

namespace MxEngine
{
	class InputBinding
	{
		std::string handle;
		IMovable& object;
	public:
		inline InputBinding(const std::string& eventHandle, IMovable& object) noexcept
			: handle(eventHandle), object(object) { }

		inline InputBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right)
		{
			Context::Instance()->GetEventDispatcher().AddEventListener<KeyEvent>(this->handle, 
				[forward, back, right, left, &object = object](KeyEvent& event)
				{
					auto dt = Context::Instance()->GetTimeDelta();
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

		inline InputBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down)
		{
            Context::Instance()->GetEventDispatcher().AddEventListener<KeyEvent>(this->handle,
				[forward, back, right, left, up, down, &object = object](KeyEvent& event)
				{
					auto dt = Context::Instance()->GetTimeDelta();
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

		inline InputBinding& BindRotation()
		{
            Context::Instance()->GetEventDispatcher().AddEventListener<MouseMoveEvent>(this->handle,
				[&object = object](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto dt = Context::Instance()->GetTimeDelta();
					object.Rotate(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}

		inline InputBinding& BindHorizontalRotation()
		{
            Context::Instance()->GetEventDispatcher().AddEventListener<MouseMoveEvent>(this->handle,
                [&object = object](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto dt = Context::Instance()->GetTimeDelta();
					object.Rotate(dt * (oldPos.x - event.position.x), 0.0f);
					oldPos = event.position;
				});
			return *this;
		}

		inline InputBinding& BindVerticalRotation()
		{
            Context::Instance()->GetEventDispatcher().AddEventListener<MouseMoveEvent>(this->handle,
                [&object = object](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto dt = Context::Instance()->GetTimeDelta();
					object.Rotate(0.0f, dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}
	};
}