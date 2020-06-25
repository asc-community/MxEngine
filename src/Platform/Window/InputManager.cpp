#include "InputManager.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    #define WND(func, ...) Application::Get()->GetWindow().func(__VA_ARGS__)

    Vector2 InputManager::GetCursorPosition()
    {
        return WND(GetCursorPosition);
    }

    void InputManager::SetCursorPosition(const Vector2& pos)
    {
        WND(UseCursorPosition, pos);
    }

    CursorMode InputManager::GetCursorMode()
    {
        return WND(GetCursorMode);
    }

    void InputManager::SetCursorMode(CursorMode mode)
    {
        WND(UseCursorMode, mode);
    }

    bool InputManager::IsKeyHeld(KeyCode key)
    {
        return WND(IsKeyHeld, key);
    }

    bool InputManager::IsKeyPressed(KeyCode key)
    {
        return WND(IsKeyPressed, key);
    }

    bool InputManager::IsKeyReleased(KeyCode key)
    {
        return WND(IsKeyReleased, key);
    }
}