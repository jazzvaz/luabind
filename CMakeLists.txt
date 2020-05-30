# Boost Software License http://www.boost.org/LICENSE_1_0.txt
# Copyright (c) 2010 The Luabind Authors

cmake_minimum_required(VERSION 3.16)
project(luabind)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(LUABIND_HEADER_TEST "Compile-test header files" OFF)
option(LUABIND_BUILD_TESTS "Build executable tests" OFF)
option(LUABIND_BUILD_SHARED "Build shared library" ON)
option(LUABIND_BUILD_EXAMPLES "Build examples" OFF)
option(LUABIND_BUILD_DOC "Build documentation" OFF)

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")
add_compile_options("$<$<CONFIG:RELEASE>:-DNDEBUG>")

if(LUABIND_BUILD_SHARED)
    add_definitions(-DLUABIND_DYNAMIC_LINK)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /permissive-")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/master/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include("${CMAKE_BINARY_DIR}/conan.cmake")

if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
endif()

conan_add_remote(
    NAME charliejiang
    URL https://api.bintray.com/conan/charliejiang/conan
)
conan_cmake_run(
    REQUIRES
        luajit/2.0.5@charliejiang/stable
        doctest/2.3.8
    OPTIONS
        luajit:shared=True
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)

include(GNUInstallDirs)
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")

include(luabind.cmake)

add_subdirectory(test)

if(LUABIND_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(LUABIND_BUILD_DOC)
    add_subdirectory(doc)
endif()
