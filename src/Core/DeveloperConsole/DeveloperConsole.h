#pragma once

#include <string>

#include "Utilities/Memory/Memory.h"

#define CHAISCRIPT_NO_THREADS
#include <chaiscript/chaiscript.hpp>

namespace MomoEngine
{
    class GraphicConsole;

    class DeveloperConsole
    {
        using ScriptEngine = chaiscript::ChaiScript*;
        using Console = GraphicConsole*;

        ScriptEngine engine;
        Console console;
    public:
        DeveloperConsole();
        ~DeveloperConsole();

        void Log(const std::string& message);
        void ClearLog();
        void PrintHistory();

        void _SetSize(float width, float height);
        void _Draw() const;

        template<typename T>
        void AddReference(const std::string& name, T&& value)
        {
            engine->add(chaiscript::fun(std::forward<T>(value)), name);
        }

        template<typename T, typename U, typename... Args>
        void AddReference(const std::string& name, T* object, U(T::*value)(Args...))
        {
            engine->add(chaiscript::fun(value, object), name);
        }

        template<typename T, typename U, typename... Args>
        void AddReference(const std::string& name, U(T::*value)(Args...))
        {
            engine->add(chaiscript::fun(value), name);
        }

        template<typename T>
        void AddVariable(const std::string& name, T&& value)
        {
            engine->add_global(chaiscript::var(std::ref(std::forward<T>(value))), name);
        }
    };
}