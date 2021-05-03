set(MXENGINE_SOURCES
"Core/Config/Config.cpp" 
"Core/Config/GlobalConfig.cpp" 
"Core/Application/Physics.cpp" 
"Core/Application/Rendering.cpp" 
"Core/Application/Application.cpp" 
"Core/Components/Physics/CapsuleCollider.cpp" 
"Core/Components/Physics/CylinderCollider.cpp"
"Core/Components/Audio/AudioListener.cpp" 
"Core/Components/Audio/AudioSource.cpp" 
"Core/Components/Camera/CameraBase.cpp" 
"Core/Components/Camera/CameraController.cpp" 
"Core/Components/Camera/CameraEffects.cpp" 
"Core/Components/Camera/FrustrumCamera.cpp" 
"Core/Components/Camera/InputController.cpp" 
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
"Core/Components/Transform.cpp" 
"Core/Components/Behaviour.cpp" 
"Core/Rendering/RenderObjects/DebugBuffer.cpp" 
"Core/Rendering/RenderObjects/RectangleObject.cpp" 
"Core/Rendering/RenderAdaptor.cpp" 
"Core/Rendering/RenderController.cpp" 
"Core/Rendering/RenderObjects/SkyboxObject.cpp"
"Core/Runtime/RuntimeEditor.cpp"  
"Core/Runtime/ResourceReflection.cpp"
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
"Platform/OpenGL/VertexBuffer.cpp" 
"Platform/OpenGL/Renderer.cpp" 
"Platform/Window/Input.cpp" 
"Platform/Window/Window.cpp" 
"Platform/Window/WindowManager.cpp" 
"Utilities/ImGui/Editors/ApplicationEditor.cpp" 
"Utilities/Audio/AudioLoader.cpp" 
"Utilities/FileSystem/File.cpp" 
"Utilities/FileSystem/FileManager.cpp" 
"Utilities/Image/Image.cpp" 
"Utilities/Image/ImageLoader.cpp" 
"Utilities/Image/ImageConverter.cpp" 
"Utilities/Image/ImageManager.cpp" 
"Utilities/ImGui/Editors/ComponentEditor.cpp" 
"Utilities/ImGui/Editors/EditorExtra.cpp" 
"Utilities/ImGui/EventLogger.cpp" 
"Utilities/ImGui/GraphicConsole.cpp" 
"Utilities/ImGui/Editors/MxObjectEditor.cpp" 
"Utilities/ImGui/ProfilerGraph.cpp" 
"Utilities/ImGui/Editors/RenderEditor.cpp" 
"Utilities/ImGui/Editors/ResourceEditor.cpp" 
"Utilities/ImGui/Viewport.cpp" 
"Utilities/ImGui/ImGuiBase.cpp"
"Utilities/Json/Json.cpp" 
"Utilities/Logging/Logger.cpp" 
"Utilities/Logging/Platform.cpp" 
"Utilities/Memory/Memory.cpp" 
"Utilities/ObjectLoading/ObjectLoader.cpp" 
"Utilities/Profiler/Profiler.cpp" 
"Utilities/Random/Random.cpp" 
"Utilities/STL/Vsnprintf.cpp" 
"Utilities/UUID/UUID.cpp" 
"Utilities/Time/Time.cpp"   
"Library/Primitives/Primitives.cpp" 
"Core/Components/Camera/CameraSSR.cpp" 
"Core/Components/Camera/CameraToneMapping.cpp" 
"Core/Rendering/RenderUtilities/ShadowMapGenerator.cpp" 
"Utilities/Parsing/ShaderPreprocessor.cpp"
"Library/Noise/NoiseGenerator.cpp"
"Core/Components/Physics/CharacterController.cpp"
"Platform/Bullet3/CompoundShape.cpp" 
"Core/Components/Physics/CompoundCollider.cpp"
"Core/Rendering/DebugDataSubmitter.cpp"
"Core/Components/Scripting/Script.cpp" 
"Core/Runtime/RuntimeCompiler.cpp"
"Core/Serialization/SceneSerializer.cpp" 
"Utilities/ImGui/RenderStatistics.cpp" 
"Core/Components/Rendering/DebugDraw.cpp"
"Core/Components/Rendering/Skybox.cpp"
"Core/Resources/Material.cpp" 
"Core/Components/Rendering/MeshSource.cpp" 
"Core/Serialization/Serializer.cpp" 
"Core/Components/Components.cpp" 
"Core/Serialization/ResourceSerialization.cpp" 
"Core/Serialization/SerializationExtra.cpp" 
"Core/Serialization/Deserializer.cpp" 
"Utilities/ObjectLoading/ObjectSaver.cpp"
"Core/Application/Scene.cpp"
"Core/Components/Camera/CameraSSGI.cpp" 
"Platform/OpenGL/ShaderBase.cpp" 
"Platform/OpenGL/ComputeShader.cpp" 
"Platform/OpenGL/BufferBase.cpp"
"Platform/Compute/Compute.cpp" 
"Core/Components/Rendering/ParticleSystem.cpp" 
"Core/Components/Camera/CameraSSAO.cpp" 
"Platform/OpenGL/VertexAttribute.cpp" 
"Core/Serialization/Cloning.cpp" "Core/Resources/BufferAllocator.cpp")

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
set(imguizmo ${SUBMODULES_PATH}/ImGuizmo)
set(portable_file_dialogs ${SUBMODULES_PATH}/portable-file-dialogs)
set(stduuid ${SUBMODULES_PATH}/stduuid)
set(runtime_compiled_cpp ${SUBMODULES_PATH}/RuntimeCompiledCPlusPlus/Aurora)
set(rttr ${SUBMODULES_PATH}/rttr)
set(allocators ${SUBMODULES_PATH}/Allocators)

set(THIRD_PARTY_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH "")
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_STATIC_LINKER_FLAGS "/Zi /VERBOSE")
    endif()
endif()
# bullet3
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
set(BUILD_ENET OFF CACHE BOOL "" FORCE)
set(USE_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
if(MSVC)
    set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
endif()

# assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)

# glew
set(glew-cmake_BUILD_SHARED ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_STATIC OFF CACHE BOOL "" FORCE)

# rcc++
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# rttr
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(BUILD_INSTALLER OFF CACHE BOOL "" FORCE)
set(BUILD_PACKAGE OFF CACHE BOOL "" FORCE)
set(USE_PCH OFF CACHE BOOL "" FORCE)
set(BUILD_WEBSITE_DOCU OFF CACHE BOOL "" FORCE)

set(THIRD_PARTY_INCLUDE_DIRS
    ${nlohmann_json}/single_include
    ${eabase}/include/Common  
    ${eastl}/include
    ${openal}/include
    ${fmt}/include
    ${glfw}/include
    ${glew}/include
    ${stduuid}/include
    ${stduuid}/gsl
    ${bullet3}/src
    ${assimp}/include
    ${miniaudio}/extras
    ${rttr}/src
    ${portable_file_dialogs}
    ${runtime_compiled_cpp}
    ${allocators}
    ${imguizmo}
    ${imgui}
    ${glm}
    ${stb}
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
set(imguizmo_binary_dir ${THIRD_PARTY_BUILD_DIR}/imguizmo)
set(runtime_compiled_cpp_binary_dir ${THIRD_PARTY_BUILD_DIR}/runtime_compiled_cpp)
set(rttr_binary_dir ${THIRD_PARTY_BUILD_DIR}/rttr)

add_subdirectory(${fmt} ${fmt_binary_dir})
add_subdirectory(${bullet3} ${bullet3_binary_dir})
add_subdirectory(${assimp} ${assimp_binary_dir})
add_subdirectory(${glew} ${glew_binary_dir})
add_subdirectory(${imgui} ${imgui_binary_dir})
add_subdirectory(${eastl} ${eastl_binary_dir})
add_subdirectory(${eabase} ${eabase_binary_dir})
add_subdirectory(${openal} ${openal_binary_dir})
add_subdirectory(${imguizmo} ${imguizmo_binary_dir})
add_subdirectory(${runtime_compiled_cpp} ${runtime_compiled_cpp_binary_dir})
add_subdirectory(${rttr} ${rttr_binary_dir})

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory(${glfw} ${glfw_binary_dir})
if(WIN32)
    set_target_properties(glfw PROPERTIES IMPORT_SUFFIX ".lib")
endif()
target_include_directories(ImGuizmo PUBLIC ${imgui})

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
    ${imguizmo_binary_dir}
    ${runtime_compiled_cpp_binary_dir}
    ${rttr_binary_dir}
)

set(THIRD_PARTY_LIBRARIES
    fmt
    libglew_shared
    assimp
    IrrXML
    glfw
    imgui
    EASTL
    OpenAL
    ImGuizmo
    BulletDynamics BulletCollision LinearMath
    RuntimeObjectSystem RuntimeCompiler
    rttr_core
)

set(SHARED_LIBRARIES
    OpenAL
    rttr_core
    libglew_shared
    glfw
)

if(TARGET zlibstatic)
    set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} zlibstatic)
endif()
if(TARGET IrrXML)
    set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} IrrXML)
endif()

set(MXENGINE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_INCLUDE_DIRS})
include_directories(${MXENGINE_INCLUDE_DIRS})

set(LIBRARY_NAME MxEngine)
add_library(${LIBRARY_NAME} STATIC ${MXENGINE_SOURCES})

set_target_properties(${LIBRARY_NAME} PROPERTIES
    COMPILE_PDB_NAME ${LIBRARY_NAME}
    COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

link_directories(${THIRD_PARTY_BINARY_DIRS})
target_link_libraries(${LIBRARY_NAME} ${THIRD_PARTY_LIBRARIES})

# Boost library - optional, only in engine core
find_package(Boost)
if (NOT MXENGINE_NO_BOOST AND Boost_FOUND)
    target_include_directories(${LIBRARY_NAME} PUBLIC ${Boost_INCLUDE_DIR})
    target_compile_definitions(${LIBRARY_NAME} PUBLIC MXENGINE_USE_BOOST)
else()
    message(WARNING "Boost library is not used in build, some engine utilities are disabled")
endif()

# RCCPP setup
list(JOIN MXENGINE_INCLUDE_DIRS "," RCCPP_INCLUDE_DIRS)
target_compile_definitions(${LIBRARY_NAME} PUBLIC
    MXENGINE_RCCPP_INCLUDE_DIRS="${RCCPP_INCLUDE_DIRS}"
)

set(LIBRARY_COUNT 0)
foreach(LIB_NAME ${LIBRARY_NAME} ${THIRD_PARTY_LIBRARIES})

    target_compile_definitions(${LIBRARY_NAME} PUBLIC
        MXENGINE_LIB_NAME${LIBRARY_COUNT}="$<TARGET_FILE_NAME:${LIB_NAME}>"
        MXENGINE_LIB_DIR${LIBRARY_COUNT}="$<TARGET_FILE_DIR:${LIB_NAME}>"
    )
    math(EXPR LIBRARY_COUNT "${LIBRARY_COUNT}+1")
endforeach()
target_compile_definitions(${LIBRARY_NAME} PUBLIC MXENGINE_RCCPP_LIBRARY_COUNT=${LIBRARY_COUNT})

set(MxEngine_SHARED_LIBRARIES ${SHARED_LIBRARIES} PARENT_SCOPE)
set(MxEngine_INCLUDE_DIR ${MXENGINE_INCLUDE_DIRS} PARENT_SCOPE)
set(MxEngine_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)