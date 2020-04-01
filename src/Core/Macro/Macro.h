#pragma once

#if defined(_WIN32)
    #define MXENGINE_WINDOWS
    #if defined(_WIN64)
        #define MXENGINE_WINDOWS64
    #else
        #define MXENGINE_WINDOWS32
    #endif
#else
#error MxEngine must be runned on Windows platform
#endif

#if defined(_DEBUG) && !defined(NDEBUG)
    #define MXENGINE_DEBUG
#else 
    #define MXENGINE_RELEASE
#endif

// at least one must be defined:
#define MXENGINE_USE_OPENGL
// #define MXENGINE_USE_DIRECTX // not supported
// #define MXENGINE_USE_VULKAN  // not supported

// scripting (optional):
#define MXENGINE_USE_PYTHON

// required in engine source:
#define MXENGINE_USE_BOOST
#define MXENGINE_USE_ASSIMP
#define MXENGINE_USE_FMT
#define MXENGINE_USE_IMGUI

#pragma comment(lib, "MxEngine.lib")

#if defined(MXENGINE_USE_OPENGL)
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "glu32.lib")
    #pragma comment(lib, "glew32s.lib")
    #pragma comment(lib, "glfw3.lib")
#endif

#if defined(MXENGINE_USE_IMGUI)
    #pragma comment(lib, "imgui.lib")
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

#if defined(MXENGINE_USE_FMT)
    #if defined(MXENGINE_RELEASE)
        #pragma comment(lib, "fmt.lib")
    #elif defined(MXENGINE_DEBUG)
        #pragma comment(lib, "fmtd.lib")
    #endif
#endif

#define MXENGINE_CONCAT_IMPL(x, y) x##y
#define MXENGINE_CONCAT(x, y) MXENGINE_CONCAT_IMPL(x, y)

#define INVOKE_ONCE(...) static char MXENGINE_CONCAT(unused, __LINE__) = [&](){ __VA_ARGS__; return '\0'; }()

#define BOOL_STRING(b) ((b) ? "true" : "false")