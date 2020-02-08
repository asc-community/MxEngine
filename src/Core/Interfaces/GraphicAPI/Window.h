#pragma once

#include "Utilities/Math/Math.h"
#include "Core/Event/KeyEvent.h"

namespace MxEngine
{
    enum class Profile
    {
        ANY = 0,
        COMPAT = 0x00032002,
        CORE = 0x00032001,
    };

    enum class CursorMode
    {
        NORMAL = 0x00034001,
        HIDDEN = 0x00034002,
        DISABLED = 0x00034003,
    };

    struct WindowHandler;

    struct Window
    {
        virtual int GetHeight() const = 0;
        virtual int GetWidth() const = 0;
        virtual bool IsOpen() const = 0;
        virtual void PullEvents() const = 0;
        virtual void OnUpdate() = 0;

        virtual Vector2 GetCursorPos() const = 0;
        virtual Vector2 GetWindowPos() const = 0;
        virtual CursorMode GetCursorMode() const = 0;
        virtual bool IsKeyHeld(KeyCode key) const = 0;
        virtual bool IsKeyPressed(KeyCode key) const = 0;
        virtual bool IsKeyReleased(KeyCode key) const = 0;
        virtual WindowHandler* GetNativeHandler() const = 0;

        virtual Window& Create() = 0;
        virtual Window& Close() = 0;
        virtual Window& SwitchContext() = 0;
        virtual Window& UseDoubleBuffering(bool value = true) = 0;
        virtual Window& UseCursorMode(CursorMode cursor) = 0;
        virtual Window& UseCursorPos(const Vector2& pos) = 0;
        virtual Window& UseTitle(const std::string& title) = 0;
        virtual Window& UsePosition(float xpos, float ypos) = 0;
        virtual Window& UseSize(int width, int height) = 0;
        virtual Window& UseEventDispatcher(AppEventDispatcher* dispatcher) = 0;
        virtual Window& UseProfile(int majorVersion, int minorVersion, Profile profile) = 0;
        virtual Window& UseSampling(int samples) = 0;
    };
}