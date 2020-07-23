#pragma once

#if defined(_WIN32)
    #define MXENGINE_WINDOWS
    #if defined(_WIN64)
        #define MXENGINE_WINDOWS64
    #else
        #define MXENGINE_WINDOWS32
        #error MxEngine can target only x64 platform
    #endif
#endif

#if defined(NDEBUG) || !defined(_DEBUG)
    #define MXENGINE_RELEASE
#else 
    #define MXENGINE_DEBUG
#endif

// graphic api
#define MXENGINE_USE_OPENGL

// audio api
#define MXENGINE_USE_OPENAL

// // scripting
// #define MXENGINE_USE_PYTHON

// physics
#define MXENGINE_USE_BULLET3

// assert handling 
#define MX_ASSERT_EXCEPTION

// engine code subsystems
#define MXENGINE_USE_BOOST

// model loader
#define MXENGINE_USE_ASSIMP

#if defined(MXENGINE_USE_OPENGL)
    #define PLATFORM_SHADER_PATH Platform/OpenGL/Shaders/
    #define PLATFORM_SHADER_EXTENSION .glsl
#endif

#define MXENGINE_CONCAT_IMPL(x, y) x##y
#define MXENGINE_CONCAT(x, y) MXENGINE_CONCAT_IMPL(x, y)

#define INVOKE_ONCE(...) static char MXENGINE_CONCAT(unused, __LINE__) = [&](){ __VA_ARGS__; return '\0'; }()

#define BOOL_STRING(b) (bool(b) ? "true" : "false")

#define MXENGINE_STRING_IMPL(x) #x
#define MXENGINE_STRING(x) MXENGINE_STRING_IMPL(x)
#define MAKE_PLATFORM_SHADER(name) MXENGINE_STRING(MXENGINE_CONCAT(MXENGINE_CONCAT(PLATFORM_SHADER_PATH, name), PLATFORM_SHADER_EXTENSION))

#define MXENGINE_MAKE_MOVEONLY(class_name) class_name() = default; class_name(const class_name&) = delete; class_name& operator=(const class_name&) = delete;\
        class_name(class_name&&) noexcept = default; class_name& operator=(class_name&&) noexcept = default

#if defined(MXENGINE_DEBUG)
    #if defined(MX_ASSERT_EXCEPTION)
            #include <exception>
            namespace MxEngine { class assert_exception : public std::exception { public: assert_exception(const char* msg) : std::exception(msg) {} }; }
        #define MX_ASSERT(expr) if(!(expr)) throw MxEngine::assert_exception(#expr) 
    #else
        #define MX_ASSERT(expr) assert(expr)
    #endif
#else
    #define MX_ASSERT(expr)
#endif

#if !defined(MXENGINE_SHIPPING)
#define MXENGINE_PROFILING_ENABLED
#endif

#define GENERATE_METHOD_CHECK(NAME, ...) namespace MxEngine { template<typename T> class has_method_##NAME {\
    template<typename U> constexpr static auto check(int) -> decltype(std::declval<U>().__VA_ARGS__, bool()) { return true; }\
    template<typename> constexpr static bool check(...) { return false; } public:\
    static constexpr bool value = check<T>(0); }; }