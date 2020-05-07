#pragma once

#include "Core/Interfaces/IEvent.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
	class MouseMoveEvent : public IEvent
	{
		MAKE_EVENT(MouseMoveEvent);
	public:
		const Vector2 position;

		inline MouseMoveEvent(float x, float y)
			: position(x, y) { }
	};
}