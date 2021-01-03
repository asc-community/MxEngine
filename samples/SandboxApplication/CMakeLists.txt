set(PROJECT_HEADER_FILES
    "Gameplay/SandboxScene.h"
    "Sandbox.h"
)

set(PROJECT_SOURCE_FILES
    "Gameplay/Arc170Object.cpp"
    "Gameplay/CameraObject.cpp"
    "Gameplay/CubeObject.cpp"
    "Gameplay/GridObject.cpp"
    "Gameplay/Lights.cpp"
    "Gameplay/SoundObject.cpp"
    "Gameplay/SphereObject.cpp"
    "Gameplay/SurfaceObject.cpp"
    "Gameplay/PBRObject.cpp"
    "Sandbox.cpp"
    "Runtime/ScriptExample.cpp"
)

set(EXECUTABLE_NAME "SandboxApplication")

set(PROJECT_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MxEngine_INCLUDE_DIR}
)

set(PROJECT_LIBRARIES
    MxEngine
)

set(PROJECT_LIBRARY_DIRECTORIES
    ${CMAKE_CURRENT_BINARY_DIR}
)

include_directories(${PROJECT_INCLUDE_DIRECTORIES})
add_executable(${EXECUTABLE_NAME} ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES})
link_directories(${PROJECT_LIBRARY_DIRECTORIES})
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${PROJECT_LIBRARIES})

include(${MxEngine_CMAKE_UTILS_DIR}/project_install.cmake)
install_mxengine_project(${EXECUTABLE_NAME})