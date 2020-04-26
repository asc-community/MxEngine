include("${CMAKE_CURRENT_LIST_DIR}/Default.cmake")

set_config_specific_property("OUTPUT_DIRECTORY" "${CMAKE_SOURCE_DIR}$<$<NOT:$<STREQUAL:${CMAKE_TARGET_ARCH},Win32>>:/${CMAKE_TARGET_ARCH}>/${PROPS_CONFIG}")

if(MSVC)
    create_property_reader("DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY")
    create_property_reader("DEFAULT_CXX_RUNTIME_LIBRARY")
    create_property_reader("DEFAULT_CXX_EXCEPTION_HANDLING")
    create_property_reader("DEFAULT_CXX_DEBUG_INFORMATION_FORMAT")

    set_config_specific_property("DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY" "/MDd")
    set_config_specific_property("DEFAULT_CXX_RUNTIME_LIBRARY" "/MD")
    set_config_specific_property("DEFAULT_CXX_EXCEPTION_HANDLING" "/EHsc")
    set_config_specific_property("DEFAULT_CXX_DEBUG_INFORMATION_FORMAT" "/Zi")
endif()
