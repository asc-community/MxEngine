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
### 7.0.0
- Entity Component System added - almost like in Unity (for more info see separate repository https://github.com/MomoDeve/MxEngineECS )
- Resource handlers and factories - no more leaked pointers!
- json format support and engine config file (will be updated with new releases)
- fog rendering, transparency and proper instanced rendering
- better Colors and Primitives API, AssetManager, RenderManager, FileManager and EventManager classes
- new runtime editor based on ImGui docking - now windows can be moved, merged, splitted and resized, including viewport
- now engine support multiple camera rendering, custom behaviour component, and custom MeshLOD generation
- EASTL library finally integrated, you can use it with aliases like MxVector<T>, MxString, MxHashMap<T>
- due to massive API refactoring python scripting was removed, but do not worry! it will be returned in next releases
### 7.0.1
- fixed some bugs with camera and window resize
- now displacement should work with shadows and without global light shader
- now methods with paths have const char\* overload to avoid collisions with other methods
### 7.0.2
- Render Editor window now has window settings (size, position, title)
- InputManager and WindowManager classes with better API
### 7.0.3
- textures now can be saved as image files (PNG, JPG, TGA, BMP, HDR are supported)
- main viewport and camera render textures can be resized to extend even belong window limit
- added offline rendering project to examples (show basics of viewport image capture and resize)
### 7.0.4
- textures now can be captured from runtime editor
- add LookAt methods to Transform component
- multiple bug fixes
### 7.0.5
- added VRCameraController component
- optimized CameraController rendering
- added VRCameraSample to example projects
### 7.1.0
- improved VR toolkit - now camera has focus distance and there is good sample
- added new debug draw utilities (frustrum for cameras, cones and spheres for light objects)
- added Frustrum camera for tiled rendering (alternative to Perspective and Orthographic)
- added offline renderer sample, showing how to render large image with offline tile rendering
### 7.2.0
- audio support added for mp3, ogg, wav and flac files
- added AudioSource and AudioListener components
- audio sample with 3D sound example
### 7.2.1
- added typedefs for graphic and audio handles
- added static InstanceFactory data submission
- added mouse button events and InputManager functions
### 7.2.2
- Transform is no longer component and stored directly in MxObject
- InstanceFactory now produces MxObjects with Instance component (is used to access color or parent)
- Removed dependency from python36.dll if MXENGINE_USE_PYTHON macro is not defined
### 7.2.3
- key and mouse events are not activated in editor mode (if you still want to access them, consider Window::Is???HeldUnchecked() method)
- you can now access custom behaviour using GetBehaviour<T>() where T is your behaviour callback
- time delta now cannot be bigger than 1/30 of second
### 7.2.4
- fixed bug for InstanceFactory which does not tracked instances if parent object has no mesh attached
- fixed bug for InstanceFactory which did not update instances for one frame if mesh had changed
### 7.3.0
- added new logger class (now static, has logging to a file, expanded macros based on build settings, severity levels)
- fixed bugs with imgui windows, now they have normal layout. Object Editor is opened by default instead of Render Editor
- fixed bug with audio source which triggers error when deleted before audio buffer
### 7.3.1
- cubemap now can be loaded from six images
- fix multiple bugs with logger and graphic resources
- now components can be automatically updated with RuntimeManager::RegisterComponentUpdate<T>() method
- high-poly cube added (also fixed height maps on cube's sharp edges)
### 7.3.2
- added Schedule() method to behaviour, now it can be called each time delta or once after some timedelta
- Behaviour timer now can be tweaked from runtime editor
- fixed bug with cubemap not loading from six strings
### 7.4.0
- added Bullet physics library and engine abstractions other it
- new physics components: RigidBody, BoxCollider, SphereCollider, CyllinderCollider
- removed ambient map and replaced diffuse map with albedo
- some small fixes in engine utilities
- added raycast functions for RigidBodies (needs collider)
### 7.4.1
- now colliders can be edited via runtime editor
- BoxCollider bounding box now replicate actual collider, not AABB
- added config class from which engine_config.json data can be accessed
### 7.4.2
- added time scale and pause option which blocks all update events
- added event logger which tracks what events are invoked
- all invoked events during one frame now can be captured and displayed in editor
### 7.4.3
- improved RigidBody api - added raycasting filter toggle, convinient function Make Static/Dynamic/Kinematic
- added some missing methods to many classes (GlobalConfig, Rendering, Shader, etc.)
- added transparency map for materials (will be useful for particle system later)
### 7.4.4
- added texture list viewer and primitive mesh loader for runtime editor
- fixed bugs with displacement, camera frustrum and OpenAL buffers
- added texture sizes to engine_config.json file (please regenerate configs!)
### 7.4.5
- editor style settings in config and new editor theme in Visual Studio style
- frustrum culling per camera
### 7.5.0
- engine now using deferred rendering! Await for many new features to come
- replaced MSAA with FXAA (I will try my best to fix remain issues in the next engine releases)
- now spot lights and point lights amount is unlimited (but if you want many of them, turn of shadows)
- now its possible to render objects with transparent mask (useful for grass, see GrassSample)
- other minor fixes (more info in application window, fixed non-mipmapped textures in editor, etc.)