#pragma once

#define CHAISCRIPT_NO_THREADS
#include <chaiscript/chaiscript.hpp>

namespace MxEngine
{
    class ScriptEngine
    {
        chaiscript::ChaiScript engine;
    public:
        inline chaiscript::ChaiScript& GetInterpreter()
        {
            return engine;
        }

        template<typename T>
        void AddReference(const std::string& name, T&& value)
        {
            engine.add(chaiscript::fun(std::forward<T>(value)), name);
        }

        template<typename T, typename U, typename... Args>
        void AddReference(const std::string& name, T* object, U(T::* value)(Args...))
        {
            engine.add(chaiscript::fun(value, object), name);
        }

        template<typename T, typename U, typename... Args>
        void AddReference(const std::string& name, U(T::* value)(Args...))
        {
            engine.add(chaiscript::fun(value), name);
        }

        template<typename T>
        void AddVariable(const std::string& name, T& value)
        {
            engine.add_global(chaiscript::var(std::ref(value)), name);
        }

        template<typename T>
        void AddType(const std::string& name, bool isAssignable = true)
        {
            using namespace chaiscript::bootstrap::standard_library;

            engine.add(chaiscript::user_type<T>(), name);
            if(isAssignable) engine.add(assignable_type<T>(name));
        }

        template<typename T, typename... Args>
        void AddTypeConstructor(const std::string& name)
        {
            engine.add(chaiscript::constructor<T(Args...)>(), name);
        }
    };
}