# MxEngine
MxEngine is a modern C++ general-purpose 3D game engine aiming to bring easy game & graphic application development into C++. 
Right now MxEngine is developed only by me, #Momo, but any contributions are welcome and will be reviewed.
Fow now MxEngine supports OpenGL as graphic API and targeting Win64 only. My future plans include other OS and API support but not in the near future.

## Additional dependencies
MxEngine is not a huge framework so multiple third-party libraries are used. Here is the full list of dependencies for last MxEngine release version:
- [stbimage](https://github.com/nothings/stb/blob/master/stb_image.h) - header-only one-file library to load images from disk
- [ChaiScript](https://github.com/MomoDeve/ChaiScript) - embedded c++ scripting language for application debugging and runtime editing
- [GLEW](http://glew.sourceforge.net/) - OpenGL C/C++ extension loading library
- [GLFW](https://www.glfw.org/) - multi-platform library for OpenGL providing API for creating windows, receiving input and events
- [GLM](https://glm.g-truc.net/0.9.9/index.html) - header only C++ mathematics library for graphics software
- [ImGui](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++

Note that MxEngine may include more additional libraries in further releases. It may also use [Boost library](https://www.boost.org) in its core. Engine requires at least C++17 as depends on some new STL libraries. All souce files are compiled using MSVC through VS2019 and are not guaranteed to be compiled by GCC/clang (at least for now).

## Versions & Releases
MxEngine releases comes with version in format X.Y.Z where X stand for major release, Y for minor release and Z for bug fix or non-significant change. 

Major releases are prone to breakage of already existing API and functional but bring a lot new features to the engine. Usually it is possible to rewrite all code using new API and retain former behaviour.

Minor releases may change API or add new features but usually user code can be easily adapted to them. With this releases also come some new MxEngine user libraries (for example new bindings or non-required components)

Bug fixes & improvements are just fixes to already existing code to reestablish initially planned behaviour. This fixes may also be merged into major or minor releases if they come in the same time.

## Full MxEngine version List
### v1.0.0
- added basic renderer
- added basic window wrapper
- added OpenGL units abstractions (shaders, buffers)
- added logger class
### v1.1.0
- added OpenGL texture wrapper
- added image loader
- improved existing implementations
### v1.2.0
- added .obj file support
- improved source file hierarchy
- improved renderer API
### v1.3.0
- added SingletonHolder template
- moved OpenGL state to initializer class
- added renderable object abstraction
### v2.0.0
- added Application class
### v2.1.0
- added ObjectInstance class
- improved API for objects
### v2.2.0
- added Camera
- added new object loading API
- added basic material system
### v3.0.0
- added orthographic camera
- added camera controller
- added ImGui library support
- added ChaiScript scripting library
- added in-game developer console
- added Application object list controller
- improved material system
### v3.0.1
- added more ChaiScript bindings
### v4.0.0
- added Event system and event dispatcher
- added math library based on GLM
- added viewport binding
### v4.1.0
- added platform independent graphic API
- added new scripting binding methods
- added profiler to json format
### v4.2.0
- added IMovable interface and methods
- new API for objects
### v4.3.0
- improved object API
- added licening
- added console & fps bindings
### v4.4.0
- added boost (stacktrace & python)
- added python scripting
- added directional global light
- fixed event & object deletion bug