#pragma once

#include "Core/Event/IEvent.h"
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

	enum class MouseButton
	{
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		_8,
		LAST,
		LEFT,
		RIGHT,
		MIDDLE,
	};

	class MousePressEvent : public IEvent
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