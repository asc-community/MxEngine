#pragma once

#include <string>

#include "Utilities/Memory/Memory.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
    class GraphicConsole;
    class ScriptEngine;

    class DeveloperConsole
    {
        ScriptEngine* engine;
        GraphicConsole* console;
        bool shouldRender = false;
    public:
        DeveloperConsole();
        ~DeveloperConsole();

        void Log(const std::string& message);
        void ClearLog();
        void PrintHistory();
        void OnRender();
        void SetSize(const Vector2& size);
        void Toggle(bool isVisible);

        ScriptEngine& GetEngine();
        Vector2 GetSize() const;
    };
}