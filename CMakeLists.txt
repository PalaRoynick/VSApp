cmake_minimum_required(VERSION 3.20)
project(VSApp C CXX)

set(CMAKE_CXX_STANDARD 23)

option(VSAPP_ENABLE_OPENGL    "Build with OpenGL"    OFF)
option(VSAPP_ENABLE_SANITIZER "Build with memory sanitizer" OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

if (VSAPP_ENABLE_SANITIZER)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

find_package(PkgConfig REQUIRED)

add_compile_options("-D__STDC_CONSTANT_MACROS")

add_subdirectory(lib)

if (VSAPP_ENABLE_OPENGL)
    add_subdirectory(lib/glfw)
    add_subdirectory(src/OpenGL)
else()
    add_subdirectory(src)
endif()