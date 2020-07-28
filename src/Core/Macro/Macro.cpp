#include "Macro.h"

#if !defined(MXENGINE_BUILD_FROM_CMAKE)
    #pragma comment(lib, "MxEngine.lib")
    
    #if defined(MXENGINE_USE_OPENGL)
        #pragma comment(lib, "opengl32.lib")
        #pragma comment(lib, "glu32.lib")
        #pragma comment(lib, "glew32s.lib")
        #pragma comment(lib, "glfw3.lib")
    #endif
    
    #if defined(MXENGINE_USE_OPENAL)
        #pragma comment(lib, "OpenAL32.lib")
    #endif
    
    #if defined(MXENGINE_USE_ASSIMP)
        #if defined(MXENGINE_DEBUG)
            #pragma comment(lib, "assimp-vc142-mtd.lib")
            #pragma comment(lib, "IrrXMLd.lib")
            #pragma comment(lib, "zlibstaticd.lib")
        #else
            #pragma comment(lib, "assimp-vc142-mt.lib")
            #pragma comment(lib, "IrrXML.lib")
            #pragma comment(lib, "zlibstatic.lib")
        #endif
    #endif
    
    #if defined(MXENGINE_USE_BULLET3)
        #if defined(MXENGINE_DEBUG)
            #pragma comment(lib, "BulletCollision-mtd.lib")
            #pragma comment(lib, "BulletDynamics-mtd.lib")
            #pragma comment(lib, "LinearMath-mtd.lib")
        #else
            #pragma comment(lib, "BulletCollision-mt.lib")
            #pragma comment(lib, "BulletDynamics-mt.lib")
            #pragma comment(lib, "LinearMath-mt.lib")
        #endif
    #endif
#endif