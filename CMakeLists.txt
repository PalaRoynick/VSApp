cmake_minimum_required(VERSION 3.20)
project(VSApp C CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib/glfw)
add_subdirectory(lib/FFmpeg)

list(APPEND SOURCES
    src/main.cxx
    src/video_reader.cxx
)

add_executable(vsapp ${SOURCES})
target_link_libraries(vsapp FFmpeg glfw GL GLU X11)