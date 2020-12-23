MxEngine is not a huge framework so multiple third-party libraries are used. Here is the full list of dependencies for lasast MxEngine release version:
- [EASTL](https://github.com/electronicarts/EASTL) - EA standard library which is used in engine core as STL alternative
- [stb](https://github.com/nothings/stb) - header-only one-file library to work with images, audio files and etc.
- [GLEW](http://glew.sourceforge.net/) - OpenGL C/C++ extension loading library
- [GLFW](https://www.glfw.org/) - multi-platform library for OpenGL providing API for creating windows, receiving input and events
- [GLM](https://glm.g-truc.net/0.9.9/index.html) - header only C++ mathematics library for graphics software
- [ImGui](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++
- [boost](https://www.boost.org) - large C++ library that you should already know about
- [assimp](http://www.assimp.org/) - portable Open Source library to import various well-known 3D model formats
- [fmt](https://github.com/fmtlib/fmt) - formatting library for modern C++
- [json](https://github.com/nlohmann/json) - nlohmann's json library for modern C++
- [miniaudio](https://github.com/dr-soft/miniaudio) - header-only audio file loader collection
- [OpenAL Soft](https://github.com/kcat/openal-soft) - cross-platform, software implementation of the OpenAL 3D audio API
- [Bullet3](https://github.com/bulletphysics/bullet3) - real-time collision detection and multi-physics simulation library
- [portable file dialogs](https://github.com/samhocevar/portable-file-dialogs) - cross-platform file dialog library
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) - object transform manipulator based on ImGui
- [stduuid](https://github.com/mariusbancila/stduuid) - A C++17 cross-platform single-header library implementation for universally unique identifiers
- [RCC++](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus) - cross-platform library for C++ runtime code compilation

Note that MxEngine may include more additional libraries in further releases. It also uses [Boost library](https://www.boost.org) in its core, but does not expose it to user-code. Engine requires at least C++17-compatable compiler as it depends on some new STL features. All source files were successfully compiled and tested using MSVC through CMake on Windows and Linux