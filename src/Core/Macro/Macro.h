#pragma once

#if defined(_WIN32)
    #define MXENGINE_WINDOWS
    #if defined(_WIN64)
        #define MXENGINE_WINDOWS64
    #else
        #define MXENGINE_WINDOWS32
        #error MxEngine can target only x64 platform
    #endif
#else
#warning MxEngine should be compiled under Windows platform
#endif

#if defined(NDEBUG) || !defined(_DEBUG)
    #define MXENGINE_RELEASE
#else 
    #define MXENGINE_DEBUG
#endif

// at least one must be defined:
#define MXENGINE_USE_OPENGL
// #define MXENGINE_USE_DIRECTX // not supported
// #define MXENGINE_USE_VULKAN  // not supported

// scripting (optional):
#define MXENGINE_USE_PYTHON

// assert handling 
#define MX_ASSERT_EXCEPTION

// required in engine source:
#define MXENGINE_USE_BOOST
#define MXENGINE_USE_ASSIMP

#pragma comment(lib, "MxEngine.lib")

#if defined(MXENGINE_USE_OPENGL)
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "glu32.lib")
    #pragma comment(lib, "glew32s.lib")
    #pragma comment(lib, "glfw3.lib")

    #define PLATFORM_SHADER_PATH Platform/OpenGL/Shaders/
    #define PLATFORM_SHADER_EXTENSION .glsl
#endif

#if defined(MXENGINE_USE_ASSIMP)
    #if defined(MXENGINE_RELEASE)
        #pragma comment(lib, "assimp-vc142-mt.lib")
        #pragma comment(lib, "IrrXML.lib")
        #pragma comment(lib, "zlibstatic.lib")
    #elif defined(MXENGINE_DEBUG)
        #pragma comment(lib, "assimp-vc142-mtd.lib")
        #pragma comment(lib, "IrrXMLd.lib")
        #pragma comment(lib, "zlibstaticd.lib")
    #endif
#endif

#define MXENGINE_CONCAT_IMPL(x, y) x##y
#define MXENGINE_CONCAT(x, y) MXENGINE_CONCAT_IMPL(x, y)

#define INVOKE_ONCE(...) static char MXENGINE_CONCAT(unused, __LINE__) = [&](){ __VA_ARGS__; return '\0'; }()

#define BOOL_STRING(b) (bool(b) ? "true" : "false")

#define MXENGINE_STRING_IMPL(x) #x
#define MXENGINE_STRING(x) MXENGINE_STRING_IMPL(x)
#define MAKE_PLATFORM_SHADER(name) MXENGINE_STRING(MXENGINE_CONCAT(MXENGINE_CONCAT(PLATFORM_SHADER_PATH, name), PLATFORM_SHADER_EXTENSION))

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

#define GENERATE_METHOD_CHECK(NAME, ...) namespace MxEngine { template<typename T> class has_method_##NAME {\
    template<typename U> constexpr static auto check(int) -> decltype(std::declval<U>().__VA_ARGS__, bool()) { return true; }\
    template<typename> constexpr static bool check(...) { return false; } public:\
    static constexpr bool value = check<T>(0); }; }