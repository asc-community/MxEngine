function(install_mxengine_project EXECUTABLE_NAME)
    set(FULL_EXECUTABLE_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}${CMAKE_EXECUTABLE_SUFFIX})
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
    # copy executable to the source folder to access user project resources
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy "${FULL_EXECUTABLE_FILENAME}" "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    message(STATUS "copying shaders into '${EXECUTABLE_NAME}' project directory")
    # copy shader files to the executable folder
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${MxEngine_ROOT_DIR}/src/Platform/OpenGL/Shaders" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders"
    )
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${MxEngine_ROOT_DIR}/src/Platform/OpenGL/Shaders" "${CMAKE_CURRENT_BINARY_DIR}/Engine/Shaders"
    )

    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_CMAKE_BUILD)
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_PROJECT_SOURCE_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_PROJECT_BINARY_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${EXECUTABLE_NAME} PUBLIC MXENGINE_SHIPPING)
    endif()
endfunction()