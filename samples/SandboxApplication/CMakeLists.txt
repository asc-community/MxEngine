set(PROJECT_HEADER_FILES
    "Gameplay/Arc170Object.h"
    "Gameplay/CameraObject.h"
    "Gameplay/CubeObject.h"
    "Gameplay/DeathStarObject.h"
    "Gameplay/DestroyerObject.h"
    "Gameplay/GridObject.h"
    "Gameplay/Lights.h"
    "Gameplay/SandboxScene.h"
    "Gameplay/SoundObject.h"
    "Gameplay/SphereObject.h"
    "Gameplay/SurfaceObject.h"
    "Gameplay/UserObjects.h"
    "Sandbox.h"
)

set(PROJECT_SOURCE_FILES
    "Sandbox.cpp"
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

set(EXECUTABLE_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}${CMAKE_EXECUTABLE_SUFFIX})
# copy executable to the source folder to access user project resources
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy "${EXECUTABLE_FILENAME}" "${CMAKE_CURRENT_SOURCE_DIR}"
)
# copy all shared libraries to the current binary directory in case user wants to ship the executable
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${MxEngine_SHARED_LIBRARIES} "${CMAKE_CURRENT_BINARY_DIR}"
)
# copy all shared libraries to the source folder in case user want to launch application with his resources from there
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${MxEngine_SHARED_LIBRARIES} "${CMAKE_CURRENT_SOURCE_DIR}"
)