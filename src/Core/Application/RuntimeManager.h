#pragma once

#include "Core/Application/Application.h"

namespace MxEngine
{
    class RuntimeManager
    {
    public:
        template<typename Func>
        static void RegisterComponentEditor(const char* name, Func&& callback)
        {
            Application::Get()->GetRuntimeEditor().RegisterComponentEditor(name, std::forward<Func>(callback));
        }

        template<typename T>
        static void RegisterComponentUpdate()
        {
            Application::Get()->RegisterComponentUpdate<T>();
        }
    };
}