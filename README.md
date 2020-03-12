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
- [Boost](https://www.boost.org) - large C++ library that you should already know about
- [Assimp](http://www.assimp.org/) - portable Open Source library to import various well-known 3D model formats
- [fmt](https://github.com/fmtlib/fmt) - formatting library for modern C++

Note that MxEngine may include more additional libraries in further releases. It also uses [Boost library](https://www.boost.org) in its core. Engine requires at least C++17 as depends on some new STL libraries. All source files are compiled using MSVC through VS2019 and are not guaranteed to be compiled by GCC/clang (at least for now).

## Versions & Releases
MxEngine releases comes with version in format X.Y.Z where X stand for major release, Y for minor release and Z for bug fix or non-significant change. 

Major releases are prone to breakage of already existing API and functional but bring a lot new features to the engine. Usually it is possible to rewrite all code using new API and retain former behaviour.

Minor releases may change API or add new features but usually user code can be easily adapted to them. With this releases also come some new MxEngine user libraries (for example new bindings or non-required components)

Bug fixes & improvements are just fixes to already existing code to reestablish initially planned behaviour. This fixes may also be merged into major or minor releases if they come in the same time.

For full version list see versions.md file