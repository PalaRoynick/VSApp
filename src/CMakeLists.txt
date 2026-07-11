add_subdirectory(Qt5)

list(APPEND
    SOURCES_FFMPEG
    ${CMAKE_CURRENT_SOURCE_DIR}/FrameConverter.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/MediaDecoder.cxx
)

add_executable(vsapp ${SOURCES_FFMPEG} main.cxx)
target_link_libraries(vsapp vsapp-qt5)