#pragma once

#include "Core/Application/Application.h"
#include "Core/Camera/CameraController.h"
#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"

namespace MomoEngine
{
	class ViewPortBinding
	{
		std::string handle;
		Application* application;
	public:
		inline ViewPortBinding(const std::string& eventHandle, Application* application) noexcept
			: handle(eventHandle), application(application) { }

		inline ViewPortBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right)
		{
			this->application->GetEventDispatcher().AddEventListener<KeyEvent>(this->handle, 
				[forward, back, right, left, app = application](KeyEvent& event)
				{
					auto& viewport = app->GetRenderer().ViewPort;
					auto dt = app->GetTimeDelta();
					if (forward != KeyCode::UNKNOWN && event.IsHeld(forward))
					{
						viewport.TranslateForward(dt);
					}
					if (back != KeyCode::UNKNOWN && event.IsHeld(back))
					{
						viewport.TranslateForward(-dt);
					}
					if (right != KeyCode::UNKNOWN && event.IsHeld(right))
					{
						viewport.TranslateRight(dt);
					}
					if (left != KeyCode::UNKNOWN && event.IsHeld(left))
					{
						viewport.TranslateRight(-dt);
					}
				});
			return *this;
		}

		inline ViewPortBinding& BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down)
		{
			this->application->GetEventDispatcher().AddEventListener<KeyEvent>(this->handle, 
				[forward, back, right, left, up, down, app = application](KeyEvent& event)
				{
					auto& viewport = app->GetRenderer().ViewPort;
					auto dt = app->GetTimeDelta();
					if (forward != KeyCode::UNKNOWN && event.IsHeld(forward))
					{
						viewport.TranslateForward(dt);
					}
					if (back != KeyCode::UNKNOWN && event.IsHeld(back))
					{
						viewport.TranslateForward(-dt);
					}
					if (right != KeyCode::UNKNOWN && event.IsHeld(right))
					{
						viewport.TranslateRight(dt);
					}
					if (left != KeyCode::UNKNOWN && event.IsHeld(left))
					{
						viewport.TranslateRight(-dt);
					}
					if (up != KeyCode::UNKNOWN && event.IsHeld(up))
					{
						viewport.TranslateUp(dt);
					}
					if (down != KeyCode::UNKNOWN && event.IsHeld(down))
					{
						viewport.TranslateUp(-dt);
					}
				});
			return *this;
		}

		inline ViewPortBinding& BindRotation()
		{
			this->application->GetEventDispatcher().AddEventListener<MouseMoveEvent>(this->handle, 
				[app = application](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto& viewport = app->GetRenderer().ViewPort;
					auto dt = app->GetTimeDelta();
					viewport.Rotate(dt * (oldPos.x - event.position.x), dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}

		inline ViewPortBinding& BindHorizontalRotation()
		{
			this->application->GetEventDispatcher().AddEventListener<MouseMoveEvent>(this->handle,
				[app = application](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto& viewport = app->GetRenderer().ViewPort;
					auto dt = app->GetTimeDelta();
					viewport.Rotate(dt * (oldPos.x - event.position.x), 0.0f);
					oldPos = event.position;
				});
			return *this;
		}

		inline ViewPortBinding& BindVerticalRotation()
		{
			this->application->GetEventDispatcher().AddEventListener<MouseMoveEvent>(this->handle,
				[app = application](MouseMoveEvent& event)
				{
					static Vector2 oldPos = event.position;
					auto& viewport = app->GetRenderer().ViewPort;
					auto dt = app->GetTimeDelta();
					viewport.Rotate(0.0f, dt * (oldPos.y - event.position.y));
					oldPos = event.position;
				});
			return *this;
		}
	};
}