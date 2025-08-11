cmake_minimum_required(VERSION 3.10)

project(YTracing VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(examples)