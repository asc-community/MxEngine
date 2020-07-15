#pragma once

#include "Core/Events/EventBase.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
	class MouseMoveEvent : public EventBase
	{
		MAKE_EVENT(MouseMoveEvent);
	public:
		const Vector2 position;

		inline MouseMoveEvent(float x, float y)
			: position(x, y) { }
	};

	enum class MouseButton
	{
		_1 = 0,
		_2 = 1,
		_3 = 2,
		_4 = 3,
		_5 = 4,
		_6 = 5,
		_7 = 6,
		_8 = 7,
		LAST = 7,
		LEFT = 0,
		RIGHT = 1,
		MIDDLE = 2,
	};

	class MousePressEvent : public EventBase
	{
		MAKE_EVENT(MousePressEvent);

		using MouseVectorPointer = const std::bitset<8>*;

		// pointer to bitset. Is is okay as event lives less than bitset
		MouseVectorPointer mouseHeld;
		MouseVectorPointer mousePressed;
		MouseVectorPointer mouseReleased;
	public:
		inline MousePressEvent(MouseVectorPointer held, MouseVectorPointer pressed, MouseVectorPointer released) noexcept
			: mouseHeld(held), mousePressed(pressed), mouseReleased(released) { }

		inline bool IsHeld(MouseButton key)     const { return (*mouseHeld)[size_t(key)]; }
		inline bool IsPressed(MouseButton key)  const { return (*mousePressed)[size_t(key)]; }
		inline bool IsReleased(MouseButton key) const { return (*mouseReleased)[size_t(key)]; }
	};
}