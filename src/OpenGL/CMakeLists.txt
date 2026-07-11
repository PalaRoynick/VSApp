list(APPEND SOURCES
    main.cxx
    video_reader.cxx
)

add_executable(vsapp ${SOURCES})

target_link_libraries(vsapp FFmpeg glfw GL GLU X11)