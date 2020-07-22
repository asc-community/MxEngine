function(install_mxengine_project EXECUTABLE_NAME)
    if(MSVC)
        set_property(TARGET ${EXECUTABLE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    
    set(FULL_EXECUTABLE_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}${CMAKE_EXECUTABLE_SUFFIX})
    # copy executable to the source folder to access user project resources
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy "${FULL_EXECUTABLE_FILENAME}" "${CMAKE_CURRENT_SOURCE_DIR}"
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
endfunction()