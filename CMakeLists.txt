cmake_minimum_required(VERSION 3.20)
project(VSApp C CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib/glfw)

add_executable(vsapp src/main.cxx)
target_link_libraries(vsapp glfw GL GLU X11)