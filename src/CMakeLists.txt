project(MxEngine CXX)

################################################################################
# Source groups
################################################################################
set(project_files_group
    "Core/Application/Application.cpp"
    "Core/Application/Application.h"
    "Core/Camera/CameraController.cpp"
    "Core/Camera/CameraController.h"
    "Core/Camera/OrthographicCamera.cpp"
    "Core/Camera/OrthographicCamera.h"
    "Core/Camera/PerspectiveCamera.cpp"
    "Core/Camera/PerspectiveCamera.h"
    "Core/Components/Instancing/Instancing.h"
    "Core/Components/Transform/Transform.cpp"
    "Core/Components/Transform/Transform.h"
    "Core/DeveloperConsole/DeveloperConsole.cpp"
    "Core/DeveloperConsole/DeveloperConsole.h"
    "Core/Event/Event.h"
    "Core/Event/Events/AppDestroyEvent.h"
    "Core/Event/Events/FpsUpdateEvent.h"
    "Core/Event/Events/KeyEvent.h"
    "Core/Event/Events/MouseEvent.h"
    "Core/Event/Events/RenderEvent.h"
    "Core/Event/Events/UpdateEvent.h"
    "Platform/OpenGL/CubeMap.h"
    "Platform/OpenGL/FrameBuffer.h"
    "Platform/OpenGL/RenderBuffer.h"
    "Platform/OpenGL/IndexBuffer.h"
    "Platform/OpenGL/Renderer.h"
    "Platform/OpenGL/Shader.h"
    "Platform/OpenGL/Texture.h"
    "Platform/OpenGL/VertexArray.h"
    "Platform/OpenGL/VertexBuffer.h"
    "Platform/OpenGL/VertexBufferLayout.h"
    "Core/Interfaces/ICamera.h"
    "Core/Interfaces/IComponent.h"
    "Core/Interfaces/IDrawable.h"
    "Core/Interfaces/IEvent.h"
    "Core/Interfaces/IMovable.h"
    "Core/Interfaces/IRenderable.h"
    "Core/Lighting/DirectionalLight/DirectionalLight.cpp"
    "Core/Lighting/DirectionalLight/DirectionalLight.h"
    "Core/Lighting/PointLight/PointLight.cpp"
    "Core/Lighting/PointLight/PointLight.h"
    "Core/Lighting/SpotLight/SpotLight.cpp"
    "Core/Lighting/SpotLight/SpotLight.h"
    "Core/Macro/Macro.h"
    "Core/Material/Material.h"
    "Core/MxObject/Mesh.cpp"
    "Core/MxObject/Mesh.h"
    "Core/MxObject/MxObject.cpp"
    "Core/MxObject/MxObject.h"
    "Core/RenderController/RenderController.cpp"
    "Core/RenderController/RenderController.h"
    "Core/Scene/Scene.cpp"
    "Core/Scene/Scene.h"
    "Core/Skybox/Skybox.cpp"
    "Core/Skybox/Skybox.h"
    "Core/DebugDraw/DebugBuffer.h"
    "Core/DebugDraw/DebugBuffer.cpp"
    "Core/DebugDraw/Rectangle.h"
    "Core/DebugDraw/Rectangle.cpp"
    "Core/BoundingObjects/AABB.h"
    "Core/BoundingObjects/BoundingSphere.h"
    "Library/Bindings/AppCloseBinding.h"
    "Library/Bindings/Bindings.h"
    "Library/Bindings/ConsoleBinding.h"
    "Library/Bindings/InputBinding.h"
    "Library/Bindings/LightBinding.h"
    "Library/Bindings/ShaderBinding.h"
    "Library/Primitives/AbstractPrimitive.h"
    "Library/Primitives/Colors.h"
    "Library/Primitives/Cube.h"
    "Library/Primitives/Grid.h"
    "Library/Primitives/Primitives.h"
    "Library/Primitives/Sphere.cpp"
    "Library/Primitives/Sphere.h"
    "Library/Primitives/Surface.h"
    "Library/Scripting/Python/PythonEngine.cpp"
    "Library/Scripting/Python/PythonEngine.h"
    "Library/Scripting/Script/Script.cpp"
    "Library/Scripting/Script/Script.h"
    "MxEngine.h"
    "Platform/GraphicAPI.h"
    "Platform/OpenGL/CubeMap.cpp"
    "Platform/OpenGL/CubeMap.h"
    "Platform/OpenGL/FrameBuffer.cpp"
    "Platform/OpenGL/FrameBuffer.h"
    "Platform/OpenGL/RenderBuffer.cpp"
    "Platform/OpenGL/RenderBuffer.h"
    "Platform/OpenGL/GLUtilities.cpp"
    "Platform/OpenGL/GLUtilities.h"
    "Platform/Modules/GraphicModule.cpp"
    "Platform/Modules/GraphicModule.h"
    "Platform/OpenGL/IndexBuffer.cpp"
    "Platform/OpenGL/IndexBuffer.h"
    "Platform/OpenGL/Renderer.cpp"
    "Platform/OpenGL/Renderer.h"
    "Platform/OpenGL/Shader.cpp"
    "Platform/OpenGL/Shader.h"
    "Platform/OpenGL/Texture.cpp"
    "Platform/OpenGL/Texture.h"
    "Platform/OpenGL/VertexArray.cpp"
    "Platform/OpenGL/VertexArray.h"
    "Platform/OpenGL/VertexBuffer.cpp"
    "Platform/OpenGL/VertexBuffer.h"
    "Platform/OpenGL/VertexBufferLayout.cpp"
    "Platform/OpenGL/VertexBufferLayout.h"
    "Platform/Window/Window.cpp"
    "Platform/Window/Window.h"
    "Utilities/Array/Array2D.h"
    "Utilities/Array/ArrayView.h"
    "Utilities/Counter/Counter.cpp"
    "Utilities/Counter/Counter.h"
    "Utilities/EventDispatcher/EventDispatcher.h"
    "Utilities/FileSystem/FileSystem.cpp"
    "Utilities/FileSystem/FileSystem.h"
    "Utilities/Format/Format.h"
    "Utilities/GenericStorage/GenericStorage.h"
    "Utilities/ImageLoader/ImageLoader.cpp"
    "Utilities/ImageLoader/ImageLoader.h"
    "Utilities/ImGui/Editors/CameraEditor.h"
    "Utilities/ImGui/Editors/LightEditor.h"
    "Utilities/ImGui/Editors/ObjectEditor.h"
    "Utilities/ImGui/GraphicConsole/GraphicConsole.cpp"
    "Utilities/ImGui/GraphicConsole/GraphicConsole.h"
    "Utilities/ImGui/ImGuiBase.h"
    "Utilities/ImGui/ImGuiUtils.h"
    "Utilities/ImGui/Layout/Layout.h"
    "Utilities/ImGui/Profiler/Profiler.h"
    "Utilities/LifetimeManager/LifetimeManager.h"
    "Utilities/Logger/Logger.cpp"
    "Utilities/Logger/Logger.h"
    "Utilities/Math/Math.h"
    "Utilities/Memory/Memory.h"
    "Utilities/ObjectLoader/ObjectLoader.cpp"
    "Utilities/ObjectLoader/ObjectLoader.h"
    "Utilities/LODGenerator/LODGenerator.cpp"
    "Utilities/LODGenerator/LODGenerator.h"
    "Utilities/Profiler/Profiler.cpp"
    "Utilities/Profiler/Profiler.h"
    "Utilities/Random/Random.cpp"
    "Utilities/Random/Random.h"
    "Utilities/ResourceStorage/ResourceStorage.h"
    "Utilities/SingletonHolder/CreationPolicy.h"
    "Utilities/SingletonHolder/LifetimePolicy.h"
    "Utilities/SingletonHolder/SingletonHolder.h"
    "Utilities/Threading/ThreadingModel.h"
    "Utilities/Time/Time.cpp"
    "Utilities/Time/Time.h"
    "Vendors/fmt/chrono.h"
    "Vendors/fmt/color.h"
    "Vendors/fmt/compile.h"
    "Vendors/fmt/core.h"
    "Vendors/fmt/format-inl.h"
    "Vendors/fmt/format.cc"
    "Vendors/fmt/format.h"
    "Vendors/fmt/locale.h"
    "Vendors/fmt/os.cc"
    "Vendors/fmt/os.h"
    "Vendors/fmt/ostream.h"
    "Vendors/fmt/posix.h"
    "Vendors/fmt/printf.h"
    "Vendors/fmt/ranges.h"
    "Vendors/imgui/imconfig.h"
    "Vendors/imgui/imgui.cpp"
    "Vendors/imgui/imgui.h"
    "Vendors/imgui/imgui_demo.cpp"
    "Vendors/imgui/imgui_draw.cpp"
    "Vendors/imgui/imgui_impl_glfw.cpp"
    "Vendors/imgui/imgui_impl_glfw.h"
    "Vendors/imgui/imgui_impl_opengl3.cpp"
    "Vendors/imgui/imgui_impl_opengl3.h"
    "Vendors/imgui/imgui_internal.h"
    "Vendors/imgui/imgui_widgets.cpp"
    "Vendors/imgui/imstb_rectpack.h"
    "Vendors/imgui/imstb_textedit.h"
    "Vendors/imgui/imstb_truetype.h"
)
source_group("" FILES ${project_files_group})

set(ALL_FILES
    ${project_files_group}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

set(ROOT_NAMESPACE MxEngine)

if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../boost_headers"
        "${CMAKE_CURRENT_SOURCE_DIR}/../python/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "GLEW_STATIC;"
        "MXENGINE_BUILD_LIB;"
        "_MBCS;"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /MDd;
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Release>:
                /MD;
                /O2;
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /bigobj;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        )
    endif()
    
    if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
           $<$<CONFIG:Debug>:
                /INCREMENTAL;
                /DEBUG;
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "advapi32;"
        "shell32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../external_libraries"
    )
endif()

################################################################################
# Post build events
################################################################################
if(MXENGINE_UPDATE_PYTHON_LIB)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND 
        xcopy /d $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/../python> $<SHELL_PATH:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}> /e
    )
endif()