#pragma once

#include "Core/Events/EventBase.h"
#include "Utilities/Math/Math.h"
#include <VulkanAbstractionLayer/Window.h>

namespace MxEngine
{
    using MouseButton = VulkanAbstractionLayer::MouseButton;

    class MouseMoveEvent : public EventBase
    {
        MAKE_EVENT(MouseMoveEvent);
    public:
        const Vector2 position;

        inline MouseMoveEvent(float x, float y)
            : position(x, y) { }
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