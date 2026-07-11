find_package(Qt5 COMPONENTS Core)
find_package(Qt5 COMPONENTS Widgets)

add_library(Qt5 INTERFACE IMPORTED GLOBAL)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

target_link_libraries(Qt5 INTERFACE
    Qt5::Core Qt5::Widgets
)

list(APPEND HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/app/Application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/player/MediaPlayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/ControlPanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MainWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/VideoWidget.h
)

list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app/Application.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/player/MediaPlayer.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/ControlPanel.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MainWindow.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/VideoWidget.cxx
)

add_library(vsapp-qt5 ${HEADERS} ${SOURCES})

target_link_libraries(vsapp-qt5 Qt5 FFmpeg)