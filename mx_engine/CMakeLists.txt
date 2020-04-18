project(mx_engine CXX)

################################################################################
# Source groups
################################################################################
set(Headers
    "exports.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "dllmain.cpp"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${Headers}
    ${Sources}
)

################################################################################
# Target
################################################################################
set(CMAKE_SHARED_LIBRARY_SUFFIX ".pyd")
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

set(ROOT_NAMESPACE mx_engine)

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
        "${CMAKE_CURRENT_SOURCE_DIR}/../src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../boost_headers"
        "${CMAKE_CURRENT_SOURCE_DIR}/../python/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_TARGET_ARCH}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_WINDOWS;"
        "_USRDLL;"
        "UNICODE;"
    )
endif()

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
                /Gys
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
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
if(MXENGINE_UPDATE_PYTHON_LIB)
    set(MXENGINE_DYN_LIB_ABS_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND 
        copy $<SHELL_PATH:${MXENGINE_DYN_LIB_ABS_PATH}> $<SHELL_PATH:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Lib/mx_engine>
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    MxEngine
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
        "${OUTPUT_DIRECTORY}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../external_libraries"
        "${CMAKE_CURRENT_SOURCE_DIR}/../python/libs"
    )
endif()

