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

    const char* EnumToString(MouseButton button);

    class MouseButtonEvent : public EventBase
    {
        MAKE_EVENT(MouseButtonEvent);

        using MouseVectorPointer = const std::bitset<8>*;

        // pointer to bitset. Is is okay as event lives less than bitset
        MouseVectorPointer mouseHeld;
        MouseVectorPointer mousePressed;
        MouseVectorPointer mouseReleased;
    public:
        inline MouseButtonEvent(MouseVectorPointer held, MouseVectorPointer pressed, MouseVectorPointer released) noexcept
            : mouseHeld(held), mousePressed(pressed), mouseReleased(released) { }

        inline bool IsHeld(MouseButton button)     const { return (*mouseHeld)[size_t(button)]; }
        inline bool IsPressed(MouseButton button)  const { return (*mousePressed)[size_t(button)]; }
        inline bool IsReleased(MouseButton button) const { return (*mouseReleased)[size_t(button)]; }
    };

    class LeftMouseButtonPressedEvent : public EventBase
    {
        MAKE_EVENT(LeftMouseButtonPressedEvent);
    };

    class RightMouseButtonPressedEvent : public EventBase
    {
        MAKE_EVENT(RightMouseButtonPressedEvent);
    };

    class MiddleMouseButtonPressedEvent : public EventBase
    {
        MAKE_EVENT(MiddleMouseButtonPressedEvent);
    };
}