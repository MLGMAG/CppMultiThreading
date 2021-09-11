cmake_minimum_required(VERSION 3.19)
project(cppMultiThreading)

add_definitions("-std=c++17")

set(UNIX_SOURCE_PATH ${PROJECT_SOURCE_DIR}/unix)

add_subdirectory(unix)
