cmake_minimum_required(VERSION 3.3)

project(MxEngine VERSION 7.4.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/MP")
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("/MDd")
  else()
    add_compile_options("/MD")
  endif()
endif()

add_subdirectory(src)