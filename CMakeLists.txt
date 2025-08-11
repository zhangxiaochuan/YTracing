cmake_minimum_required(VERSION 3.10)

project(YTracing VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

add_subdirectory(src)
add_subdirectory(tools)

if(NOT TARGET uninstall)
    message(STATUS "Adding uninstall target")

    # 在 build 目录下创建一个 install_manifest.txt 文件，
    # `make install` 会将所有安装的文件路径记录在其中。
    set(MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt")

    # 创建一个自定义的 uninstall 目标
    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
            COMMENT "Uninstalling all installed files from ${MANIFEST}..."
    )

    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
            IMMEDIATE @ONLY
    )
endif()