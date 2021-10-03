// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Window.h"

#include "Utilities/Logging/Logger.h"
#include "Utilities/EventDispatcher/EventDispatcher.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/Format/Format.h"
#include "Platform/Modules/GraphicModule.h"
#include "Core/Events/WindowResizeEvent.h"
#include "Core/Runtime/Reflection.h"

#include <array>

namespace MxEngine
{
    void Window::Destroy()
    {
        if (this->window != nullptr)
        {
            GraphicModule::OnWindowDestroy(this->GetNativeHandle());
            this->Close();
            delete this->window;
            this->window = nullptr;
            MXLOG_DEBUG("MxEngine::Window", "window destroyed");
        }
    }

    void Window::Move(Window&& other)
    {
        this->title = std::move(other.title);
        this->width = other.width;
        this->height = other.height;
        this->window = other.window;
        this->dispatcher = other.dispatcher;
        this->keyHeld = other.keyHeld;
        this->keyPressed = other.keyPressed;
        this->keyReleased = other.keyReleased;
        this->mouseHeld = other.mouseHeld;
        this->mousePressed = other.mousePressed;
        this->mouseReleased = other.mouseReleased;
        this->cursorMode = other.cursorMode;
        this->windowPosition = other.windowPosition;

        other.width = 0;
        other.height = 0;
        other.window = nullptr;
        other.dispatcher = nullptr;
        other.cursorMode = CursorMode::NORMAL;
        other.windowPosition = MakeVector2(0.0f);
        other.keyHeld.reset();
        other.keyReleased.reset();
        other.keyPressed.reset();
    }

    Window::Window(int width, int height, const MxString& title)
        : title(title), width(width), height(height)
    {        
        MXLOG_DEBUG("MxEngine::Window", "window object created");
    }

    Window::Window(Window&& other) noexcept
    {
        this->Move(std::move(other));
    }

    int Window::GetWidth() const
    {
        if (this->window == nullptr)
            return this->width;
        
        return this->window->GetSize().x;
    }

    int Window::GetHeight() const
    {
        if (this->window == nullptr)
            return this->height;

        return this->window->GetSize().y;
    }

    Window::WindowHandle Window::GetNativeHandle()
    {
        return this->window;
    }

    EventDispatcherImpl<EventBase>& Window::GetEventDispatcher()
    {
        return *this->dispatcher;
    }

    bool Window::IsCreated() const
    {
        return this->window != nullptr;
    }

    bool Window::IsOpen() const
    {
        if (this->window == nullptr)
        {
            MXLOG_WARNING("MxEngine::Window", "window was not created while calling Window::IsOpen");
            return false;
        }
        return !this->window->ShouldClose();
    }

    void Window::PullEvents() const
    {        
        MAKE_SCOPE_PROFILER("Window::PullEvents");
        this->keyPressed.reset();
        this->keyReleased.reset();
        this->mousePressed.reset();
        this->mouseReleased.reset();

        this->window->PollEvents();
    }

    void Window::OnUpdate()
    {
        GraphicModule::OnFrameBegin();

        if (this->dispatcher != nullptr)
        {
            Vector2 currentSize(this->GetWidth(), this->GetHeight());
            Vector2 prevSize(this->width, this->height);
            if (currentSize != prevSize)
            {
                this->dispatcher->AddEvent(MakeUnique<WindowResizeEvent>(prevSize, currentSize));
                this->width =  (int)currentSize.x;
                this->height = (int)currentSize.y;
            }

            auto keyEvent = MakeUnique<KeyEvent>(&this->keyHeld, &this->keyPressed, &this->keyReleased);
            this->dispatcher->AddEvent(std::move(keyEvent));
            auto mousePress = MakeUnique<MouseButtonEvent>(&this->mouseHeld, &this->mousePressed, &this->mouseReleased);
            this->dispatcher->AddEvent(std::move(mousePress));

            if (this->mousePressed.test((size_t)MouseButton::LEFT))
            {
                auto lbmEvent = MakeUnique<LeftMouseButtonPressedEvent>();
                this->dispatcher->AddEvent(std::move(lbmEvent));
            }
            if (this->mousePressed.test((size_t)MouseButton::RIGHT))
            {
                auto rbmEvent = MakeUnique<RightMouseButtonPressedEvent>();
                this->dispatcher->AddEvent(std::move(rbmEvent));
            }
            if (this->mousePressed.test((size_t)MouseButton::MIDDLE))
            {
                auto mbmEvent = MakeUnique<MiddleMouseButtonPressedEvent>();
                this->dispatcher->AddEvent(std::move(mbmEvent));
            }

            auto cursor = this->GetCursorPosition();
            auto mouseMoveEvent = MakeUnique<MouseMoveEvent>(cursor.x, cursor.y);
            this->dispatcher->AddEvent(std::move(mouseMoveEvent));
        }
        else // do not store key and mouse states if dispatcher is nullptr
        {
            this->keyPressed.reset();
            this->keyReleased.reset();
            this->keyHeld.reset();

            this->mousePressed.reset();
            this->mouseReleased.reset();
            this->mouseHeld.reset();
        }

        this->anyKeyEvent = false;
        this->anyMouseEvent = false;
    }

    void Window::Present()
    {
        GraphicModule::OnFrameEnd();
    }

    Window& Window::Close()
    {
        if (this->window != nullptr && this->IsOpen())
        {
            this->window->Close();
            MXLOG_DEBUG("MxEngine::Window", "window closed");
        }
        return *this;
    }

    Vector2 Window::GetCursorPosition() const
    {
        if (this->window == nullptr)
            return Vector2(0.0f, 0.0f);

        return this->window->GetCursorPosition();
    }

    Vector2 Window::GetWindowPosition() const
    {
        if (this->window == nullptr)
            return this->windowPosition;

        return this->window->GetPosition();
    }

    float Window::GetTimeSinceCreation() const
    {
        return this->window->GetTimeSinceCreation();
    }

    void Window::SetTimeSinceCreation(float time)
    {
        this->window->SetTimeSinceCreation(time);
    }

    bool Window::IsKeyHeld(KeyCode key) const
    {
        return key != KeyCode::UNKNOWN && this->keyHeld[(size_t)key];
    }

    bool Window::IsKeyPressed(KeyCode key) const
    {
        return key != KeyCode::UNKNOWN && this->keyPressed[(size_t)key];
    }

    bool Window::IsKeyReleased(KeyCode key) const
    {
        return key != KeyCode::UNKNOWN && this->keyReleased[(size_t)key];
    }

    bool Window::IsMouseHeld(MouseButton button) const
    {
        return this->mouseHeld[(size_t)button];
    }

    bool Window::IsMousePressed(MouseButton button) const
    {
        return this->mousePressed[(size_t)button];
    }

    bool Window::IsMouseReleased(MouseButton button) const
    {
        return this->mouseReleased[(size_t)button];
    }

    bool Window::IsKeyHeldUnchecked(KeyCode key) const
    {
        if (this->window == nullptr) 
            return false;

        return this->window->IsKeyPressed(key);
    }

    bool Window::IsMouseHeldUnchecked(MouseButton button) const
    {
        if (this->window == nullptr) 
            return false;

        return this->window->IsMousePressed(button);
    }

    Window& Window::Create()
    {
        {
            MAKE_SCOPE_PROFILER("Window::Create");
            MAKE_SCOPE_TIMER("MxEngine::Window", "Window::Create");

            VulkanAbstractionLayer::WindowCreateOptions windowOptions;
            windowOptions.Position = this->GetWindowPosition();
            windowOptions.Resizeable = true;
            windowOptions.Size = Vector2((float)this->GetWidth(), (float)this->GetHeight());
            windowOptions.TileBar = true;
            windowOptions.TransparentFramebuffer = false;
            windowOptions.Title = this->GetTitle().c_str();
            windowOptions.ErrorCallback = [](const std::string& message)
            {
                MXLOG_FATAL("MxEngine::Window", ToMxString(message));
            };

            this->window = new VulkanAbstractionLayer::Window(windowOptions);
            
            this->window->OnKeyChanged([this](VulkanAbstractionLayer::Window& window, KeyCode key, bool isPressed)
                {
                    this->anyKeyEvent = true;
                    this->keyPressed[(size_t)key] = isPressed;
                    this->keyReleased[(size_t)key] = !isPressed;
                    this->keyHeld[(size_t)key] = isPressed;
                });
            
            this->window->OnMouseChanged([this](VulkanAbstractionLayer::Window& window, MouseButton button, bool isPressed)
                {
                    this->anyMouseEvent = true;
                    this->mousePressed[(size_t)button] = isPressed;
                    this->mouseReleased[(size_t)button] = !isPressed;
                    this->mouseHeld[(size_t)button] = isPressed;
                });
        }
        GraphicModule::OnWindowCreate(this->GetNativeHandle());

        UseTitle(this->title);
        UseCursorMode(this->cursorMode);
        UseWindowPosition((int)this->windowPosition.x, (int)this->windowPosition.y);
        UseCursorPosition(this->windowPosition);
        MXLOG_DEBUG("MxEngine::Window", "window initialized");
        return *this;
    }

    Window& Window::SwitchContext()
    {
        // the call is required for dll context sync
        this->window->SetContext(this->window->GetNativeHandle());
        return *this;
    }

    Window& Window::UseCursorMode(CursorMode cursor)
    {
        if (window != nullptr)
        {
            this->window->SetCursorMode(cursor);
        }
        return *this;
    }

    Window& Window::UseCursorPosition(const Vector2& pos)
    {
        if (this->window != nullptr)
        {
            this->window->SetCursorPosition(pos);
        }
        return *this;
    }

    Window& Window::UseTitle(const MxString& title)
    {
        this->title = title;
        if (this->window != nullptr)
        {
            this->window->SetTitle(title.c_str());
        }
        return *this;
    }

    Window& Window::UseWindowPosition(int xpos, int ypos)
    {
        this->windowPosition = { (float)xpos, (float)ypos };
        if (window != nullptr)
        {
            this->window->SetPosition(this->windowPosition);
        }
        return *this;
    }

    Window& Window::UseWindowSize(int width, int height)
    {
        if (this->window != nullptr)
        {
            this->window->SetSize(Vector2((float)width, (float)height));
            if (this->dispatcher != nullptr)
                this->dispatcher->AddEvent(
                    MakeUnique<WindowResizeEvent>(MakeVector2((float)this->width, (float)this->height), MakeVector2((float)width, (float)height)));
        }
        this->width = width;
        this->height = height;
        return *this;
    }

    Window& Window::UseEventDispatcher(EventDispatcherImpl<EventBase>* dispatcher)
    {
        this->dispatcher = dispatcher;
        return *this;
    }

    CursorMode Window::GetCursorMode() const
    {
        if (this->window == nullptr)
            return this->cursorMode;

        return this->window->GetCursorMode();
    }

    const MxString& Window::GetTitle() const
    {
        return this->title;
    }

    Window& Window::operator=(Window&& other) noexcept
    {
        this->Destroy();
        this->Move(std::move(other));
        return *this;
    }

    Window::~Window()
    {
        this->Destroy();
    }

    const char* EnumToString(KeyCode value)
    {
        auto t = rttr::type::get(value).get_enumeration();
        return t.value_to_name(value).cbegin();
    }

    const char* EnumToString(MouseButton value)
    {
        auto t = rttr::type::get(value).get_enumeration();
        return t.value_to_name(value).cbegin();
    }

    const char* EnumToString(CursorMode value)
    {
        auto t = rttr::type::get(value).get_enumeration();
        return t.value_to_name(value).cbegin();
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::enumeration<KeyCode>("KeyCode")
        (
            rttr::value("UNKNOWN"      , KeyCode::UNKNOWN      ),
            rttr::value("SPACE"        , KeyCode::SPACE        ),
            rttr::value("APOSTROPHE"   , KeyCode::APOSTROPHE   ),
            rttr::value("COMMA"        , KeyCode::COMMA        ),
            rttr::value("MINUS"        , KeyCode::MINUS        ),
            rttr::value("PERIOD"       , KeyCode::PERIOD       ),
            rttr::value("SLASH"        , KeyCode::SLASH        ),
            rttr::value("_0"           , KeyCode::_0           ),
            rttr::value("_1"           , KeyCode::_1           ),
            rttr::value("_2"           , KeyCode::_2           ),
            rttr::value("_3"           , KeyCode::_3           ),
            rttr::value("_4"           , KeyCode::_4           ),
            rttr::value("_5"           , KeyCode::_5           ),
            rttr::value("_6"           , KeyCode::_6           ),
            rttr::value("_7"           , KeyCode::_7           ),
            rttr::value("_8"           , KeyCode::_8           ),
            rttr::value("_9"           , KeyCode::_9           ),
            rttr::value("SEMICOLON"    , KeyCode::SEMICOLON    ),
            rttr::value("EQUAL"        , KeyCode::EQUAL        ),
            rttr::value("A"            , KeyCode::A            ),
            rttr::value("B"            , KeyCode::B            ),
            rttr::value("C"            , KeyCode::C            ),
            rttr::value("D"            , KeyCode::D            ),
            rttr::value("E"            , KeyCode::E            ),
            rttr::value("F"            , KeyCode::F            ),
            rttr::value("G"            , KeyCode::G            ),
            rttr::value("H"            , KeyCode::H            ),
            rttr::value("I"            , KeyCode::I            ),
            rttr::value("J"            , KeyCode::J            ),
            rttr::value("K"            , KeyCode::K            ),
            rttr::value("L"            , KeyCode::L            ),
            rttr::value("M"            , KeyCode::M            ),
            rttr::value("N"            , KeyCode::N            ),
            rttr::value("O"            , KeyCode::O            ),
            rttr::value("P"            , KeyCode::P            ),
            rttr::value("Q"            , KeyCode::Q            ),
            rttr::value("R"            , KeyCode::R            ),
            rttr::value("S"            , KeyCode::S            ),
            rttr::value("T"            , KeyCode::T            ),
            rttr::value("U"            , KeyCode::U            ),
            rttr::value("V"            , KeyCode::V            ),
            rttr::value("W"            , KeyCode::W            ),
            rttr::value("X"            , KeyCode::X            ),
            rttr::value("Y"            , KeyCode::Y            ),
            rttr::value("Z"            , KeyCode::Z            ),
            rttr::value("LEFT_BRACKET" , KeyCode::LEFT_BRACKET ),
            rttr::value("BACKSLASH"    , KeyCode::BACKSLASH    ),
            rttr::value("RIGHT_BRACKET", KeyCode::RIGHT_BRACKET),
            rttr::value("GRAVE_ACCENT" , KeyCode::GRAVE_ACCENT ),
            rttr::value("WORLD_1"      , KeyCode::WORLD_1      ),
            rttr::value("WORLD_2"      , KeyCode::WORLD_2      ),
            rttr::value("ESCAPE"       , KeyCode::ESCAPE       ),
            rttr::value("ENTER"        , KeyCode::ENTER        ),
            rttr::value("TAB"          , KeyCode::TAB          ),
            rttr::value("BACKSPACE"    , KeyCode::BACKSPACE    ),
            rttr::value("INSERT"       , KeyCode::INSERT       ),
            rttr::value("DELETE"       , KeyCode::DELETE       ),
            rttr::value("RIGHT"        , KeyCode::RIGHT        ),
            rttr::value("LEFT"         , KeyCode::LEFT         ),
            rttr::value("DOWN"         , KeyCode::DOWN         ),
            rttr::value("UP"           , KeyCode::UP           ),
            rttr::value("PAGE_UP"      , KeyCode::PAGE_UP      ),
            rttr::value("PAGE_DOWN"    , KeyCode::PAGE_DOWN    ),
            rttr::value("HOME"         , KeyCode::HOME         ),
            rttr::value("END"          , KeyCode::END          ),
            rttr::value("CAPS_LOCK"    , KeyCode::CAPS_LOCK    ),
            rttr::value("SCROLL_LOCK"  , KeyCode::SCROLL_LOCK  ),
            rttr::value("NUM_LOCK"     , KeyCode::NUM_LOCK     ),
            rttr::value("PRINT_SCREEN" , KeyCode::PRINT_SCREEN ),
            rttr::value("PAUSE"        , KeyCode::PAUSE        ),
            rttr::value("F1"           , KeyCode::F1           ),
            rttr::value("F2"           , KeyCode::F2           ),
            rttr::value("F3"           , KeyCode::F3           ),
            rttr::value("F4"           , KeyCode::F4           ),
            rttr::value("F5"           , KeyCode::F5           ),
            rttr::value("F6"           , KeyCode::F6           ),
            rttr::value("F7"           , KeyCode::F7           ),
            rttr::value("F8"           , KeyCode::F8           ),
            rttr::value("F9"           , KeyCode::F9           ),
            rttr::value("F10"          , KeyCode::F10          ),
            rttr::value("F11"          , KeyCode::F11          ),
            rttr::value("F12"          , KeyCode::F12          ),
            rttr::value("F13"          , KeyCode::F13          ),
            rttr::value("F14"          , KeyCode::F14          ),
            rttr::value("F15"          , KeyCode::F15          ),
            rttr::value("F16"          , KeyCode::F16          ),
            rttr::value("F17"          , KeyCode::F17          ),
            rttr::value("F18"          , KeyCode::F18          ),
            rttr::value("F19"          , KeyCode::F19          ),
            rttr::value("F20"          , KeyCode::F20          ),
            rttr::value("F21"          , KeyCode::F21          ),
            rttr::value("F22"          , KeyCode::F22          ),
            rttr::value("F23"          , KeyCode::F23          ),
            rttr::value("F24"          , KeyCode::F24          ),
            rttr::value("F25"          , KeyCode::F25          ),
            rttr::value("KP_0"         , KeyCode::KP_0         ),
            rttr::value("KP_1"         , KeyCode::KP_1         ),
            rttr::value("KP_2"         , KeyCode::KP_2         ),
            rttr::value("KP_3"         , KeyCode::KP_3         ),
            rttr::value("KP_4"         , KeyCode::KP_4         ),
            rttr::value("KP_5"         , KeyCode::KP_5         ),
            rttr::value("KP_6"         , KeyCode::KP_6         ),
            rttr::value("KP_7"         , KeyCode::KP_7         ),
            rttr::value("KP_8"         , KeyCode::KP_8         ),
            rttr::value("KP_9"         , KeyCode::KP_9         ),
            rttr::value("KP_DECIMAL"   , KeyCode::KP_DECIMAL   ),
            rttr::value("KP_DIVIDE"    , KeyCode::KP_DIVIDE    ),
            rttr::value("KP_MULTIPLY"  , KeyCode::KP_MULTIPLY  ),
            rttr::value("KP_SUBTRACT"  , KeyCode::KP_SUBTRACT  ),
            rttr::value("KP_ADD"       , KeyCode::KP_ADD       ),
            rttr::value("KP_ENTER"     , KeyCode::KP_ENTER     ),
            rttr::value("KP_EQUAL"     , KeyCode::KP_EQUAL     ),
            rttr::value("LEFT_SHIFT"   , KeyCode::LEFT_SHIFT   ),
            rttr::value("LEFT_CONTROL" , KeyCode::LEFT_CONTROL ),
            rttr::value("LEFT_ALT"     , KeyCode::LEFT_ALT     ),
            rttr::value("LEFT_SUPER"   , KeyCode::LEFT_SUPER   ),
            rttr::value("RIGHT_SHIFT"  , KeyCode::RIGHT_SHIFT  ),
            rttr::value("RIGHT_CONTROL", KeyCode::RIGHT_CONTROL),
            rttr::value("RIGHT_ALT"    , KeyCode::RIGHT_ALT    ),
            rttr::value("RIGHT_SUPER"  , KeyCode::RIGHT_SUPER  ),
            rttr::value("MENU"         , KeyCode::MENU         )
        );

        rttr::registration::enumeration<MouseButton>("MouseButton")
        (
            rttr::value("1"     , MouseButton::_1    ),
            rttr::value("2"     , MouseButton::_2    ),
            rttr::value("3"     , MouseButton::_3    ),
            rttr::value("4"     , MouseButton::_4    ),
            rttr::value("5"     , MouseButton::_5    ),
            rttr::value("6"     , MouseButton::_6    ),
            rttr::value("7"     , MouseButton::_7    ),
            rttr::value("8"     , MouseButton::_8    ),
            rttr::value("LAST"  , MouseButton::LAST  ),
            rttr::value("LEFT"  , MouseButton::LEFT  ),
            rttr::value("RIGHT" , MouseButton::RIGHT ),
            rttr::value("MIDDLE", MouseButton::MIDDLE)
        );

        rttr::registration::enumeration<CursorMode>("CursorMode")
        (
            rttr::value("DISABLED", CursorMode::DISABLED),
            rttr::value("HIDDEN"  , CursorMode::HIDDEN  ),
            rttr::value("NORMAL"  , CursorMode::NORMAL  )
        );
    }
}