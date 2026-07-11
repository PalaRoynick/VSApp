cmake_minimum_required(VERSION 3.20)
project(FFmpeg)

set(FFMPEG_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/install")

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${FFMPEG_INSTALL_DIR}/lib/pkgconfig")

list(PREPEND CMAKE_PREFIX_PATH "${FFMPEG_INSTALL_DIR}")

pkg_check_modules(AVCODEC       REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT      REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVFILTER      REQUIRED IMPORTED_TARGET libavfilter)
pkg_check_modules(AVDEVICE      REQUIRED IMPORTED_TARGET libavdevice)
pkg_check_modules(AVUTIL        REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWRESAMPLE    REQUIRED IMPORTED_TARGET libswresample)
pkg_check_modules(SWSCALE       REQUIRED IMPORTED_TARGET libswscale)


message(STATUS "=======================================================")
message(STATUS "Paths found by pkg-config for FFMPEG:")
message(STATUS "=======================================================")

set(FFMPEG_MODULES AVCODEC AVFORMAT AVFILTER AVDEVICE AVUTIL SWRESAMPLE SWSCALE)

foreach(MOD IN LISTS FFMPEG_MODULES)
    message(STATUS "--- ${MOD} ---")
    message(STATUS "  Includes: ${${MOD}_INCLUDE_DIRS}")
    message(STATUS "  Lib Dirs: ${${MOD}_LIBRARY_DIRS}")
    message(STATUS "  Link Libs: ${${MOD}_LINK_LIBRARIES}")
endforeach()

message(STATUS "=======================================================")


add_library(FFmpeg INTERFACE IMPORTED GLOBAL)

target_link_libraries(FFmpeg INTERFACE
    PkgConfig::AVCODEC
    PkgConfig::AVFORMAT
    PkgConfig::AVFILTER
    PkgConfig::AVDEVICE
    PkgConfig::AVUTIL
    PkgConfig::SWRESAMPLE
    PkgConfig::SWSCALE
)