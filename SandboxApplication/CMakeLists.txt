project(SandboxApplication CXX)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Gameplay/Arc170Object.h"
    "Gameplay/CubeObject.h"
    "Gameplay/DeathStarObject.h"
    "Gameplay/DestroyerObject.h"
    "Gameplay/SandboxScene.h"
    "Gameplay/SphereObject.h"
    "Gameplay/UserObjects.h"
    "python36.dll"
    "Resources/scripts/init.py"
    "Resources/scripts/load.py"
    "Resources/scripts/update.py"
    "Sandbox.cpp"
    "Sandbox.h"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

set(ROOT_NAMESPACE SandboxApplication)

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
        "${CMAKE_CURRENT_SOURCE_DIR}/../src;"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "GLEW_STATIC;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /MDd;
                /Od;
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
        )
    endif()

    if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /PROFILE
            >
            /DEBUG:FULL;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    MxEngine
    mx_engine
)

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
target_link_libraries(${PROJECT_NAME} PUBLIC "MxEngine")

if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${OUTPUT_DIRECTORY};"
        "${CMAKE_CURRENT_SOURCE_DIR}/../python/libs;"
    )
endif()

