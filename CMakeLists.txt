cmake_minimum_required(VERSION 3.3)

project(MxEngine VERSION 7.4.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options("/MP")
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("/MDd")
  else()
    add_compile_options("/MD")
  endif()
endif()

set(Boost_INCLUDE_DIR "D:/boost" CACHE STRING "Boost root directory")

add_subdirectory(src)
add_subdirectory(samples/SandboxApplication)