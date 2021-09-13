cmake_minimum_required(VERSION 3.19)
project(cppMultiThreading)

add_definitions("-std=c++17")

set(SOURCE_PATH ${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

set(WIN32_SOURCE_PATH ${PROJECT_SOURCE_DIR}/win32Api)
add_subdirectory(win32Api)

set(UNIX_SOURCE_PATH ${PROJECT_SOURCE_DIR}/unix)
add_subdirectory(unix)
