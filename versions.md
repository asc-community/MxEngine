# Full MxEngine version List [alpha]
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
### v4.5.0
- added point and spot lights
- added imgui window for objects, lighting and profiling
- added dynamic VBO support
### v4.6.0
- added geometric primivited (cube, sphere, grid) and colored textures
- added macros to specify builds (libraries, scripting, etc.)
- added imgui draw functions for debug tools (included into developer console)
- added lighting bindings & fixed some issues
### v5.0.0
- added new utility classes (Random, ArrayView, Array2D, Counter, FileSystem)
- added format library (fmt lib)
- improved instanced models: interface, model instancing (still working on)
- added projects samples (will be updated over time)
- added resource manager for textures/meshes/shaders
- added transform component for MxObject and camera
- some code refactoring
### v6.0.0
- added scenes (with scene loader, auto managed resources, etc)
- exported almost all MxEngine API to python
- added more imgui editing options
- assimp library integration
- MxEngine now does not have file dependencies (shaders / textures / objects)
- scripting API in C++ with python (chaiscript has been removed)
### v6.1.0
- added shadow mapping (texture maps, cubemaps)
- added runtime script & shader editing support
- minor fixes in opengl debugging, instancing and imgui editor
- added new python api to render controller
### v6.2.0
- added skybox, skybox reflection (reflection will be improved soon)
- MxEngine now can be built from sources using Visual Studio and CMake, yay!
- included all dependencies as submodules or precompiled libraries
- added dynamic surfaces (can be configured in runtime via function f(x, y))
### 6.2.1
- improved object mesh loading (now all object has IBO for rendering)
- material editing from in runtime from ImGui and python
- fixed multiple bugs
### 6.3.0
- added LOD generator for meshes and auto LOD selection when rendering objects
- added per-submesh transformations and color (first step in animations)
- added MSAA support (default to 4x)
- other performance improvements and profile-logging
### 6.4.0
- normal & height mapping (per material)
- debug draw - AABB boxes and bounding spheres
- HDR rendering support
- customizable bloom effect
- some new MxEngine library utilities