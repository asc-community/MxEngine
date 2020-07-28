set(MXENGINE_HEADERS
    "Core/Application/Application.h" 
    "Core/Application/Event.h" 
    "Core/Application/Config.h" 
    "Core/Application/GlobalConfig.h" 
    "Core/Application/Physics.h" 
    "Core/Application/Rendering.h" 
    "Core/Application/Runtime.h" 
    "Core/Application/Timer.h" 
    "Core/BoundingObjects/AABB.h" 
    "Core/BoundingObjects/BoundingBox.h" 
    "Core/BoundingObjects/BoundingSphere.h" 
    "Core/BoundingObjects/Capsule.h" 
    "Core/BoundingObjects/Cone.h" 
    "Core/BoundingObjects/Cylinder.h" 
    "Core/BoundingObjects/Frustrum.h" 
    "Core/BoundingObjects/Line.h" 
    "Core/Components/Audio/AudioListener.h" 
    "Core/Components/Audio/AudioSource.h" 
    "Core/Components/Camera/CameraBase.h" 
    "Core/Components/Camera/CameraController.h" 
    "Core/Components/Camera/FrustrumCamera.h" 
    "Core/Components/Camera/InputControl.h" 
    "Core/Components/Camera/OrthographicCamera.h" 
    "Core/Components/Camera/PerspectiveCamera.h" 
    "Core/Components/Camera/VRCameraController.h" 
    "Core/Components/Components.h" 
    "Core/Components/Instancing/Instance.h" 
    "Core/Components/Instancing/InstanceFactory.h" 
    "Core/Components/Lighting/LightBase.h" 
    "Core/Components/Physics/BoxCollider.h" 
    "Core/Components/Physics/CapsuleCollider.h" 
    "Core/Components/Physics/ColliderBase.h" 
    "Core/Components/Physics/CylinderCollider.h" 
    "Core/Components/Physics/RigidBody.h" 
    "Core/Components/Physics/SphereCollider.h" 
    "Core/Components/Rendering/DebugDraw.h" 
    "Core/Components/Rendering/MeshLOD.h" 
    "Core/Components/Rendering/MeshRenderer.h" 
    "Core/Components/Rendering/MeshSource.h" 
    "Core/Components/Rendering/Skybox.h" 
    "Core/Components/Lighting/DirectionalLight.h" 
    "Core/Components/Lighting/PointLight.h" 
    "Core/Components/Lighting/SpotLight.h" 
    "Core/Components/Script.h" 
    "Core/Components/Transform.h" 
    "Core/Components/Behaviour.h" 
    "Core/Rendering/DebugBuffer.h" 
    "Core/Rendering/Rectangle.h" 
    "Core/Rendering/RenderAdaptor.h" 
    "Core/Rendering/RenderPipeline.h" 
    "Core/Rendering/SkyboxObject.h" 
    "Core/Rendering/RenderController.h" 
    "Core/Runtime/RuntimeEditor.h" 
    "Core/Events/AppDestroyEvent.h" 
    "Core/Events/Events.h" 
    "Core/Events/RenderEvent.h" 
    "Core/Events/FpsUpdateEvent.h" 
    "Core/Events/UpdateEvent.h" 
    "Core/Events/KeyEvent.h" 
    "Core/Events/MouseEvent.h" 
    "Core/Events/WindowResizeEvent.h" 
    "Core/Events/EventBase.h" 
    "Core/Macro/Macro.h" 
    "Core/Resources/Mesh.h" 
    "Core/Resources/MeshData.h" 
    "Core/Resources/AssetManager.h" 
    "Core/Resources/SubMesh.h" 
    "Core/Resources/Material.h" 
    "Core/MxObject/MxObject.h" 
    "Platform/AudioAPI.h" 
    "Platform/GraphicAPI.h" 
    "Platform/PhysicsAPI.h" 
    "Platform/Modules/GraphicModule.h" 
    "Platform/Modules/AudioModule.h" 
    "Platform/Modules/PhysicsModule.h" 
    "Platform/OpenAL/ALUtilities.h" 
    "Platform/OpenAL/AudioBuffer.h" 
    "Platform/OpenAL/AudioPlayer.h" 
    "Platform/Bullet3/BoxShape.h" 
    "Platform/Bullet3/CapsuleShape.h" 
    "Platform/Bullet3/CylinderShape.h" 
    "Platform/Bullet3/NativeRigidBody.h" 
    "Platform/Bullet3/ShapeBase.h" 
    "Platform/Bullet3/SphereShape.h" 
    "Platform/Bullet3/Bullet3Utils.h"
    "Platform/OpenGL/CubeMap.h" 
    "Platform/OpenGL/FrameBuffer.h" 
    "Platform/OpenGL/RenderBuffer.h" 
    "Platform/OpenGL/GLUtilities.h" 
    "Platform/OpenGL/IndexBuffer.h" 
    "Platform/OpenGL/Shader.h" 
    "Platform/OpenGL/Texture.h" 
    "Platform/OpenGL/VertexArray.h" 
    "Platform/OpenGL/VertexBufferLayout.h" 
    "Platform/OpenGL/VertexBuffer.h" 
    "Platform/OpenGL/Renderer.h" 
    "Platform/Window/Window.h" 
    "Platform/Window/Input.h" 
    "Platform/Window/WindowManager.h" 
    "Library/Scripting/Python/PythonEngine.h"  
    "Library/Primitives/Primitives.h" 
    "Library/Primitives/Colors.h" 
    "Utilities/Audio/SupportedAudioTypes.h" 
    "Utilities/Audio/AudioLoader.h" 
    "Utilities/AbstractFactory/AbstractFactory.h" 
    "Utilities/ECS/Component.h" 
    "Utilities/ECS/ComponentFactory.h" 
    "Utilities/ECS/ComponentView.h" 
    "Utilities/FileSystem/FileManager.h" 
    "Utilities/FileSystem/File.h" 
    "Utilities/Image/Image.h" 
    "Utilities/Image/ImageManager.h" 
    "Utilities/Image/ImageConverter.h" 
    "Utilities/ImGui/Editors/ApplicationEditor.h" 
    "Utilities/ImGui/Editors/ComponentEditor.h" 
    "Utilities/ImGui/Editors/MxObjectEditor.h" 
    "Utilities/ImGui/Editors/RenderEditor.h" 
    "Utilities/ImGui/Editors/ResourceEditor.h" 
    "Utilities/ImGui/EventLogger.h" 
    "Utilities/ImGui/Viewport.h" 
    "Utilities/ImGui/GraphicConsole.h" 
    "Utilities/ImGui/ImGuiBase.h" 
    "Utilities/ImGui/ImGuiUtils.h" 
    "Utilities/ImGui/Layout.h" 
    "Utilities/ImGui/ProfilerGraph.h" 
    "Utilities/Json/Json.h" 
    "Utilities/LODGenerator/LODGenerator.h" 
    "Utilities/Logging/Logger.h" 
    "Utilities/Logging/LoggerData.h" 
    "Utilities/Logging/LogSettings.h" 
    "Utilities/Logging/Platform.h" 
    "Utilities/Memory/ChunkAllocator.h" 
    "Utilities/Memory/LinearAllocator.h" 
    "Utilities/Memory/Memory.h" 
    "Utilities/Memory/PoolAllocator.h" 
    "Utilities/Memory/RandomAllocator.h" 
    "Utilities/Memory/StackAllocator.h" 
    "Utilities/Array/Array2D.h" 
    "Utilities/Array/ArrayView.h" 
    "Utilities/EventDispatcher/EventDispatcher.h" 
    "Utilities/Format/Format.h" 
    "Utilities/Image/ImageLoader.h" 
    "Utilities/Math/Math.h" 
    "Utilities/ObjectLoader/ObjectLoader.h"  
    "Utilities/Profiler/Profiler.h" 
    "Utilities/Random/Random.h" 
    "Utilities/STL/MxFunction.h" 
    "Utilities/STL/MxHashMap.h" 
    "Utilities/STL/MxHashSet.h" 
    "Utilities/STL/MxMap.h" 
    "Utilities/STL/MxSet.h" 
    "Utilities/STL/MxString.h" 
    "Utilities/STL/MxVector.h" 
    "Utilities/UUID/UUID.h" 
    "Utilities/String/String.h" 
    "Utilities/Time/Time.h" 
    "Utilities/VectorPool/VectorPool.h" 
    "MxEngine.h"
)

set(MXENGINE_SOURCES
    "Core/Application/Config.cpp" 
    "Core/Application/GlobalConfig.cpp" 
    "Core/Application/Physics.cpp" 
    "Core/Application/Render.cpp" 
    "Core/Application/Application.cpp" 
    "Core/Components/Physics/CapsuleCollider.cpp" 
    "Core/Components/Physics/CylinderCollider.cpp"
    "Core/Components/Audio/AudioListener.cpp" 
    "Core/Components/Audio/AudioSource.cpp" 
    "Core/Components/Camera/CameraBase.cpp" 
    "Core/Components/Camera/CameraController.cpp" 
    "Core/Components/Camera/FrustrumCamera.cpp" 
    "Core/Components/Camera/InputControl.cpp" 
    "Core/Components/Camera/OrthographicCamera.cpp" 
    "Core/Components/Camera/PerspectiveCamera.cpp" 
    "Core/Components/Camera/VRCameraController.cpp" 
    "Core/Components/Instancing/InstanceFactory.cpp" 
    "Core/Components/Physics/BoxCollider.cpp" 
    "Core/Components/Physics/ColliderBase.cpp" 
    "Core/Components/Physics/RigidBody.cpp" 
    "Core/Components/Physics/SphereCollider.cpp" 
    "Core/Components/Rendering/MeshLOD.cpp" 
    "Core/Components/Rendering/MeshRenderer.cpp" 
    "Core/Components/Lighting/DirectionalLight.cpp" 
    "Core/Components/Lighting/PointLight.cpp" 
    "Core/Components/Lighting/SpotLight.cpp"
    "Core/Components/Script.cpp" 
    "Core/Components/Transform.cpp" 
    "Core/Components/Behaviour.cpp" 
    "Core/Rendering/DebugBuffer.cpp" 
    "Core/Rendering/Rectangle.cpp" 
    "Core/Rendering/RenderAdaptor.cpp" 
    "Core/Rendering/RenderController.cpp" 
    "Core/Rendering/SkyboxObject.cpp"
    "Core/Runtime/RuntimeEditor.cpp"  
    "Core/Macro/Macro.cpp"
    "Core/MxObject/MxObject.cpp" 
    "Core/Resources/Mesh.cpp" 
    "Core/Resources/MeshData.cpp" 
    "Core/Resources/AssetManager.cpp" 
    "Core/Resources/SubMesh.cpp"  
    "Platform/Modules/AudioModule.cpp" 
    "Platform/Modules/PhysicsModule.cpp" 
    "Platform/Modules/GraphicModule.cpp"
    "Platform/Bullet3/CapsuleShape.cpp" 
    "Platform/Bullet3/CylinderShape.cpp" 
    "Platform/Bullet3/BoxShape.cpp" 
    "Platform/Bullet3/NativeRigidBody.cpp" 
    "Platform/Bullet3/ShapeBase.cpp" 
    "Platform/Bullet3/SphereShape.cpp" 
    "Platform/OpenAL/ALUtilities.cpp" 
    "Platform/OpenAL/AudioBuffer.cpp" 
    "Platform/OpenAL/AudioPlayer.cpp" 
    "Platform/OpenGL/CubeMap.cpp" 
    "Platform/OpenGL/FrameBuffer.cpp"  
    "Platform/OpenGL/GLUtilities.cpp" 
    "Platform/OpenGL/IndexBuffer.cpp" 
    "Platform/OpenGL/RenderBuffer.cpp" 
    "Platform/OpenGL/Shader.cpp" 
    "Platform/OpenGL/Texture.cpp" 
    "Platform/OpenGL/VertexArray.cpp" 
    "Platform/OpenGL/VertexBufferLayout.cpp" 
    "Platform/OpenGL/VertexBuffer.cpp" 
    "Platform/OpenGL/Renderer.cpp" 
    "Platform/Window/Input.cpp" 
    "Platform/Window/Window.cpp" 
    "Platform/Window/WindowManager.cpp" 
    "Utilities/ImGui/Editors/ApplicationEditor.cpp" 
    "Utilities/ImGui/Editors/ComponentEditors/RenderingEditors.cpp" 
    "Utilities/Audio/AudioLoader.cpp" 
    "Utilities/FileSystem/File.cpp" 
    "Utilities/FileSystem/FileManager.cpp" 
    "Utilities/Image/Image.cpp" 
    "Utilities/Image/ImageLoader.cpp" 
    "Utilities/Image/ImageConverter.cpp" 
    "Utilities/Image/ImageManager.cpp" 
    "Utilities/ImGui/Editors/ComponentEditors/AudioEditors.cpp" 
    "Utilities/ImGui/Editors/ComponentEditors/CameraEditors.cpp" 
    "Utilities/ImGui/Editors/ComponentEditors/ComponentEditor.cpp" 
    "Utilities/ImGui/EventLogger.cpp" 
    "Utilities/ImGui/GraphicConsole.cpp" 
    "Utilities/ImGui/Editors/ComponentEditors/InstancingEditors.cpp" 
    "Utilities/ImGui/Editors/LightEditors.cpp" 
    "Utilities/ImGui/Editors/MxObjectEditor.cpp" 
    "Utilities/ImGui/Editors/ComponentEditors/PhysicsEditors.cpp" 
    "Utilities/ImGui/ProfilerGraph.cpp" 
    "Utilities/ImGui/Editors/RenderEditor.cpp" 
    "Utilities/ImGui/Editors/ResourceEditor.cpp" 
    "Utilities/ImGui/Viewport.cpp" 
    "Utilities/ImGui/ImGuiBase.cpp"
    "Utilities/Json/Json.cpp" 
    "Utilities/LODGenerator/LODGenerator.cpp" 
    "Utilities/Logging/Logger.cpp" 
    "Utilities/Logging/Platform.cpp" 
    "Utilities/Memory/Memory.cpp" 
    "Utilities/ObjectLoader/ObjectLoader.cpp" 
    "Utilities/Profiler/Profiler.cpp" 
    "Utilities/Random/Random.cpp" 
    "Utilities/STL/Vsnprintf.cpp" 
    "Utilities/UUID/UUID.cpp" 
    "Utilities/Time/Time.cpp"   
    "Library/Primitives/Primitives.cpp" 
    "Library/Scripting/Python/PythonEngine.cpp"   
)

set(PROJECT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(SUBMODULES_PATH ${PROJECT_ROOT_PATH}/submodules)

set(nlohmann_json ${SUBMODULES_PATH}/json)
set(glm ${SUBMODULES_PATH}/glm)
set(fmt ${SUBMODULES_PATH}/fmt)
set(stb ${SUBMODULES_PATH}/stb)
set(glfw ${SUBMODULES_PATH}/glfw)
set(glew ${SUBMODULES_PATH}/glew)
set(imgui ${SUBMODULES_PATH}/imgui)
set(miniaudio ${SUBMODULES_PATH}/miniaudio)
set(openal ${SUBMODULES_PATH}/openal-soft)
set(eastl ${SUBMODULES_PATH}/EASTL)
set(eabase ${SUBMODULES_PATH}/EABase)
set(assimp ${SUBMODULES_PATH}/assimp)
set(bullet3 ${SUBMODULES_PATH}/bullet3)

set(THIRD_PARTY_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH "")
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

find_package(Boost)
if (NOT Boost_FOUND)
    message(SEND_ERROR "Boost package was not found, please add boost to environment variables by setting BOOST_ROOT or set Boost_INCLUDE_DIR variable in CMake project")
    return()
endif()

if(MSVC)
    add_compile_definitions(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
endif()

add_compile_definitions(MXENGINE_BUILD_FROM_CMAKE)
if(MXENGINE_BUILD_SHIPPING)
    add_compile_definitions(MXENGINE_SHIPPING)
endif()

# bullet3
set(BUILD_CPU_DEMOS OFF)
set(BUILD_BULLET2_DEMOS OFF)
set(BUILD_OPENGL3_DEMOS OFF)
set(BUILD_UNIT_TESTS OFF)
set(USE_GRAPHICAL_BENCHMARK OFF)
set(BUILD_CLSOCKET OFF)
set(BUILD_ENET OFF)
set(BUILD_EXTRAS OFF)

# assimp
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_INSTALL OFF)

# glew
set(glew-cmake_BUILD_SHARED OFF)
set(glew-cmake_BUILD_STATIC ON)

set(THIRD_PARTY_INCLUDE_DIRS
    ${nlohmann_json}/single_include
    ${eabase}/include/Common  
    ${eastl}/include
    ${openal}/include
    ${fmt}/include
    ${glfw}/include
    ${glew}/include
    ${bullet3}/src
    ${assimp}/include
    ${miniaudio}/extras
    ${imgui}
    ${glm}
    ${stb}
    ${Boost_INCLUDE_DIRS}
)

set(fmt_binary_dir ${THIRD_PARTY_BUILD_DIR}/fmt)
set(bullet3_binary_dir ${THIRD_PARTY_BUILD_DIR}/bullet3)
set(assimp_binary_dir ${THIRD_PARTY_BUILD_DIR}/assimp)
set(glfw_binary_dir ${THIRD_PARTY_BUILD_DIR}/glfw)
set(glew_binary_dir ${THIRD_PARTY_BUILD_DIR}/glew)
set(imgui_binary_dir ${THIRD_PARTY_BUILD_DIR}/imgui)
set(eastl_binary_dir ${THIRD_PARTY_BUILD_DIR}/eastl)
set(eabase_binary_dir ${THIRD_PARTY_BUILD_DIR}/eabase)
set(openal_binary_dir ${THIRD_PARTY_BUILD_DIR}/openal)

add_subdirectory(${fmt} ${fmt_binary_dir})
add_subdirectory(${bullet3} ${bullet3_binary_dir})
add_subdirectory(${assimp} ${assimp_binary_dir})
add_subdirectory(${glfw} ${glfw_binary_dir})
add_subdirectory(${glew} ${glew_binary_dir})
add_subdirectory(${imgui} ${imgui_binary_dir})
add_subdirectory(${eastl} ${eastl_binary_dir})
add_subdirectory(${eabase} ${eabase_binary_dir})
add_subdirectory(${openal} ${openal_binary_dir})

set(THIRD_PARTY_BINARY_DIRS
    ${fmt_binary_dir}
    ${bullet3_binary_dir}
    ${assimp_binary_dir}
    ${glfw_binary_dir}
    ${glew_binary_dir}
    ${imgui_binary_dir}
    ${eastl_binary_dir}
    ${eabase_binary_dir} 
    ${openal_binary_dir}
)

set(THIRD_PARTY_LIBRARIES
    fmt
    libglew_static
    assimp
    glfw
    imgui
    EASTL
    OpenAL
    BulletDynamics
    BulletCollision
    LinearMath
)

set(MXENGINE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_INCLUDE_DIRS})
include_directories(${MXENGINE_INCLUDE_DIRS})
set(MxEngine_INCLUDE_DIR ${MXENGINE_INCLUDE_DIRS} PARENT_SCOPE)
set(MxEngine_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(LIBRARY_NAME MxEngine)
add_library(${LIBRARY_NAME} STATIC ${MXENGINE_SOURCES} ${MXENGINE_HEADERS})
add_library(${LIBRARY_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})

link_directories(${THIRD_PARTY_BINARY_DIRS})
target_link_libraries(${LIBRARY_NAME} ${THIRD_PARTY_LIBRARIES})

set(SHARED_LIBRARIES_PATHS
    ${openal_binary_dir}/OpenAL32${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(MxEngine_SHARED_LIBRARIES ${SHARED_LIBRARIES_PATHS} PARENT_SCOPE)

if(MSVC)
    add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy ${SHARED_LIBRARIES_PATHS} "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()