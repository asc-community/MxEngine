function(install_mxengine_project EXECUTABLE_NAME)
    set(FULL_EXECUTABLE_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}${CMAKE_EXECUTABLE_SUFFIX})

    # copy all shared libraries to the current binary and source directories in case user wants to ship the executable
    foreach(LIB_NAME ${LIBRARY_NAME} ${MxEngine_SHARED_LIBRARIES})
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy "$<TARGET_FILE:${LIB_NAME}>" "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>"
        || (exit 0)
    )
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy "$<TARGET_FILE:${LIB_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}"
        || (exit 0)
    )
    endforeach()

    # copy executable to the source folder to access user project resources
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}"
        || (exit 0)
    )

    message(STATUS "copying shaders into '${EXECUTABLE_NAME}' project directory")
    # copy shader files to the executable folder
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${MxEngine_ROOT_DIR}/src/Platform/OpenGL/Shaders" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders"
        || (exit 0)
    )
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${MxEngine_ROOT_DIR}/src/Platform/OpenGL/Shaders" "${CMAKE_CURRENT_BINARY_DIR}/Engine/Shaders"
        || (exit 0)
    )

    message(STATUS "copying textures into '${EXECUTABLE_NAME}' project directory")
    # copy shader files to the executable folder
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${MxEngine_ROOT_DIR}/src/Platform/OpenGL/Textures" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Textures"
        || (exit 0)
    )
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${MxEngine_ROOT_DIR}/src/Platform/OpenGL/Textures" "${CMAKE_CURRENT_BINARY_DIR}/Engine/Textures"
        || (exit 0)
    )

    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_CMAKE_BUILD)
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_PROJECT_SOURCE_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_PROJECT_BINARY_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_SHIPPING)
    endif()
    
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${EXECUTABLE_NAME} PUBLIC "/MDd")
        else()
            target_compile_options(${EXECUTABLE_NAME} PUBLIC "/MD")
        endif()
    endif()
endfunction()
