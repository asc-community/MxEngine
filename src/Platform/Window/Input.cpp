#include "Input.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    #define WND(func, ...) Application::GetImpl()->GetWindow().func(__VA_ARGS__)

    Vector2 Input::GetCursorPosition()
    {
        return WND(GetCursorPosition);
    }

    void Input::SetCursorPosition(const Vector2& pos)
    {
        WND(UseCursorPosition, pos);
    }

    CursorMode Input::GetCursorMode()
    {
        return WND(GetCursorMode);
    }

    void Input::SetCursorMode(CursorMode mode)
    {
        WND(UseCursorMode, mode);
    }

    bool Input::IsKeyHeld(KeyCode key)
    {
        return WND(IsKeyHeld, key);
    }

    bool Input::IsKeyPressed(KeyCode key)
    {
        return WND(IsKeyPressed, key);
    }

    bool Input::IsKeyReleased(KeyCode key)
    {
        return WND(IsKeyReleased, key);
    }

    bool Input::IsMouseHeld(MouseButton button)
    {
        return WND(IsMouseHeld, button);
    }

    bool Input::IsMousePressed(MouseButton button)
    {
        return WND(IsMousePressed, button);
    }

    bool Input::IsMouseReleased(MouseButton button)
    {
        return WND(IsMouseReleased, button);
    }
}